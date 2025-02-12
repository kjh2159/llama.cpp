#include "arg.h"
#include "common.h"
#include "console.h"
#include "log.h"
#include "sampling.h"
#include "llama.h"

#include <cstdio>
#include <cstring>
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#if defined (__unix__) || (defined (__APPLE__) && defined (__MACH__))
#include <signal.h>
#include <unistd.h>
#elif defined (_WIN32)
#define WIN32_LEAN_AND_MEAN
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <signal.h>
#endif

#if defined(_MSC_VER)
#pragma warning(disable: 4244 4267) // possible loss of data
#endif

static const char * DEFAULT_SYSTEM_MESSAGE = "You are a helpful assistant";

static llama_context           ** g_ctx;
static llama_model             ** g_model;
static common_sampler          ** g_smpl;
static common_params            * g_params;
static std::vector<llama_token> * g_input_tokens;
static std::ostringstream       * g_output_ss;
static std::vector<llama_token> * g_output_tokens;
static bool is_interacting  = false;
static bool need_insert_eot = false;

static void print_usage(int argc, char ** argv) {
    (void) argc;

    LOG("\nexample usage:\n");
    LOG("\n  text generation:     %s -m your_model.gguf -p \"I believe the meaning of life is\" -n 128\n", argv[0]);
    LOG("\n  chat (conversation): %s -m your_model.gguf -p \"You are a helpful assistant\" -cnv\n", argv[0]);
    // ↓↓↓ CSV 예시 사용법 안내 (추가)
    LOG("\n  csv batch test:      %s -m your_model.gguf --csv questions.csv --csv-limit 5 -n 128\n", argv[0]);
    LOG("\n");
}

static std::vector<std::string> load_questions_from_csv(const std::string & path_csv) {
    std::vector<std::string> questions;
    std::ifstream ifs(path_csv);

    if (!ifs.is_open()) {
        std::cerr << "Failed to open CSV file: " << path_csv << std::endl;
        return questions;
    }

    std::string line;
    while (std::getline(ifs, line)) {
        std::cout << "Read line: " << line << std::endl;  // <-- CSV에서 읽은 내용 출력
        if (!line.empty()) {
            questions.push_back(line);
        }
    }

    std::cout << "Total questions loaded: " << questions.size() << std::endl; // 전체 개수 확인
    return questions;
}


static bool file_exists(const std::string & path) {
    std::ifstream f(path.c_str());
    return f.good();
}

static bool file_is_empty(const std::string & path) {
    std::ifstream f;
    f.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    f.open(path.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
    return f.tellg() == 0;
}

#if defined (__unix__) || (defined (__APPLE__) && defined (__MACH__)) || defined (_WIN32)
static void sigint_handler(int signo) {
    if (signo == SIGINT) {
        if (!is_interacting && g_params->interactive) {
            is_interacting  = true;
            need_insert_eot = true;
        } else {
            console::cleanup();
            LOG("\n");
            common_perf_print(*g_ctx, *g_smpl);

            LOG("Interrupted by user\n");
            common_log_pause(common_log_main());

            _exit(130);
        }
    }
}
#endif

static std::string chat_add_and_format(struct llama_model * model, std::vector<common_chat_msg> & chat_msgs, const std::string & role, const std::string & content) {
    common_chat_msg new_msg{role, content};
    auto formatted = common_chat_format_single(model, g_params->chat_template, chat_msgs, new_msg, role == "user");
    chat_msgs.push_back({role, content});
    LOG_DBG("formatted: '%s'\n", formatted.c_str());
    return formatted;
}

int main(int argc, char ** argv) {
    common_params params;
    g_params = &params;
    if (!common_params_parse(argc, argv, params, LLAMA_EXAMPLE_MAIN, print_usage)) {
        return 1;
    }

    // ↓↓↓ CSV 관련 옵션(파일 경로, 질문 개수 제한) 파싱 로직 추가
    std::string path_csv;
    int csv_question_limit = 0; // 0이면 제한 없음
    {
        // 간단히 argv를 직접 스캔
        for (int i = 1; i < argc; i++) {
            std::string arg = argv[i];
            if (arg == "--csv-path") {
                if (++i >= argc) {
                    fprintf(stderr, "Error: missing CSV file after --csv\n");
                    return 1;
                }
                path_csv = argv[i];
            } else if (arg == "--csv-limit") {
                if (++i >= argc) {
                    fprintf(stderr, "Error: missing limit after --csv-limit\n");
                    return 1;
                }
                csv_question_limit = std::stoi(argv[i]);
            }
        }
    }
    // ↑↑↑ CSV 관련 옵션 추가 끝

    common_init();

    auto & sparams = params.sampling;

    console::init(params.simple_io, params.use_color);
    atexit([]() { console::cleanup(); });

    if (params.logits_all) {
        LOG_ERR("************\n");
        LOG_ERR("%s: please use the 'perplexity' tool for perplexity calculations\n", __func__);
        LOG_ERR("************\n\n");
        return 0;
    }

    if (params.embedding) {
        LOG_ERR("************\n");
        LOG_ERR("%s: please use the 'embedding' tool for embedding calculations\n", __func__);
        LOG_ERR("************\n\n");
        return 0;
    }

    if (params.n_ctx != 0 && params.n_ctx < 8) {
        LOG_WRN("%s: warning: minimum context size is 8, using minimum size.\n", __func__);
        params.n_ctx = 8;
    }

    if (params.rope_freq_base != 0.0) {
        LOG_WRN("%s: warning: changing RoPE frequency base to %g.\n", __func__, params.rope_freq_base);
    }

    if (params.rope_freq_scale != 0.0) {
        LOG_WRN("%s: warning: scaling RoPE frequency by %g.\n", __func__, params.rope_freq_scale);
    }

    LOG_INF("%s: llama backend init\n", __func__);

    llama_backend_init();
    llama_numa_init(params.numa);

    llama_model * model = nullptr;
    llama_context * ctx = nullptr;
    common_sampler * smpl = nullptr;

    g_model = &model;
    g_ctx = &ctx;
    g_smpl = &smpl;

    std::vector<common_chat_msg> chat_msgs;

    // load the model and apply lora adapter, if any
    LOG_INF("%s: load the model and apply lora adapter, if any\n", __func__);
    common_init_result llama_init = common_init_from_params(params);

    model = llama_init.model.get();
    ctx = llama_init.context.get();

    if (model == NULL) {
        LOG_ERR("%s: error: unable to load model\n", __func__);
        return 1;
    }

    const llama_vocab * vocab = llama_model_get_vocab(model);

    LOG_INF("%s: llama threadpool init, n_threads = %d\n", __func__, (int) params.cpuparams.n_threads);

    auto * reg = ggml_backend_dev_backend_reg(ggml_backend_dev_by_type(GGML_BACKEND_DEVICE_TYPE_CPU));
    auto * ggml_threadpool_new_fn  = (decltype(ggml_threadpool_new)  *) ggml_backend_reg_get_proc_address(reg, "ggml_threadpool_new");
    auto * ggml_threadpool_free_fn = (decltype(ggml_threadpool_free) *) ggml_backend_reg_get_proc_address(reg, "ggml_threadpool_free");

    struct ggml_threadpool_params tpp_batch = ggml_threadpool_params_from_cpu_params(params.cpuparams_batch);
    struct ggml_threadpool_params tpp       = ggml_threadpool_params_from_cpu_params(params.cpuparams);

    set_process_priority(params.cpuparams.priority);

    struct ggml_threadpool * threadpool_batch = NULL;
    if (!ggml_threadpool_params_match(&tpp, &tpp_batch)) {
        threadpool_batch = ggml_threadpool_new_fn(&tpp_batch);
        if (!threadpool_batch) {
            LOG_ERR("%s: batch threadpool create failed : n_threads %d\n", __func__, tpp_batch.n_threads);
            return 1;
        }
        tpp.paused = true;
    }

    struct ggml_threadpool * threadpool = ggml_threadpool_new_fn(&tpp);
    if (!threadpool) {
        LOG_ERR("%s: threadpool create failed : n_threads %d\n", __func__, tpp.n_threads);
        return 1;
    }

    llama_attach_threadpool(ctx, threadpool, threadpool_batch);

    const int n_ctx_train = llama_model_n_ctx_train(model);
    const int n_ctx       = llama_n_ctx(ctx);

    if (n_ctx > n_ctx_train) {
        LOG_WRN("%s: model was trained on only %d context tokens (%d specified)\n", __func__, n_ctx_train, n_ctx);
    }

    // auto enable conversation mode if chat template is available
    const bool has_chat_template = !common_get_builtin_chat_template(model).empty() || !params.chat_template.empty();
    if (params.conversation_mode == COMMON_CONVERSATION_MODE_AUTO) {
        if (has_chat_template) {
            LOG_INF("%s: chat template is available, enabling conversation mode (disable it with -no-cnv)\n", __func__);
            params.conversation_mode = COMMON_CONVERSATION_MODE_ENABLED;
        } else {
            params.conversation_mode = COMMON_CONVERSATION_MODE_DISABLED;
        }
    }
    if (params.conversation_mode && !has_chat_template) {
        LOG_WRN("%s: chat template is not available or is not supported. This may cause the model to output suboptimal responses\n", __func__);
    }
    if (params.conversation_mode) {
        if (params.enable_chat_template) {
            LOG_INF("%s: chat template example:\n%s\n", __func__, common_chat_format_example(model, params.chat_template).c_str());
        } else {
            LOG_INF("%s: in-suffix/prefix is specified, chat template will be disabled\n", __func__);
        }
    }

    {
        LOG_INF("\n");
        LOG_INF("%s\n", common_params_get_system_info(params).c_str());
        LOG_INF("\n");
    }

    // 샘플러 초기화
    smpl = common_sampler_init(model, sparams);
    if (!smpl) {
        LOG_ERR("%s: failed to initialize sampling subsystem\n", __func__);
        return 1;
    }

    LOG_INF("sampler seed: %u\n",     common_sampler_get_seed(smpl));
    LOG_INF("sampler params: \n%s\n", sparams.print().c_str());
    LOG_INF("sampler chain: %s\n",    common_sampler_print(smpl).c_str());

    LOG_INF("generate: n_ctx = %d, n_batch = %d, n_predict = %d, n_keep = %d\n",
            n_ctx, params.n_batch, params.n_predict, params.n_keep);

    // group-attention 관련
    const int ga_n = params.grp_attn_n;
    const int ga_w = params.grp_attn_w;
    if (ga_n != 1) {
        GGML_ASSERT(ga_n > 0);
        GGML_ASSERT(ga_w % ga_n == 0);
        LOG_INF("self-extend: n_ctx_train = %d, grp_attn_n = %d, grp_attn_w = %d\n", n_ctx_train, ga_n, ga_w);
    }
    LOG_INF("\n");

    // ctrl+C handling
#if defined (__unix__) || (defined (__APPLE__) && defined (__MACH__)) || defined (_WIN32)
    {
        struct sigaction sigint_action;
#if defined(_WIN32)
        // 윈도우의 경우 조금 다르게 처리하지만, 여기서는 간단히 예시
        auto console_ctrl_handler = +[](DWORD ctrl_type) -> BOOL {
            return (ctrl_type == CTRL_C_EVENT) ? (sigint_handler(SIGINT), true) : false;
        };
        SetConsoleCtrlHandler(reinterpret_cast<PHANDLER_ROUTINE>(console_ctrl_handler), true);
#else
        sigint_action.sa_handler = sigint_handler;
        sigemptyset(&sigint_action.sa_mask);
        sigint_action.sa_flags = 0;
        sigaction(SIGINT, &sigint_action, NULL);
#endif
    }
#endif

    // ↓↓↓ CSV 또는 단일 프롬프트/대화 입력을 위한 질문 목록 준비
    std::vector<std::string> all_questions;
    if (!path_csv.empty()) {
        // CSV 파일에서 질문 목록 읽기
        std::cout << "load csv";
        all_questions = load_questions_from_csv(path_csv);
        if (all_questions.empty()) {
            LOG_ERR("No questions found in CSV (or failed to load)\n");

            // [수정됨] 직접 cleanup label로 점프하지 않고, 리소스 해제 후 return
            common_sampler_free(smpl);
            llama_backend_free();
            ggml_threadpool_free_fn(threadpool);
            ggml_threadpool_free_fn(threadpool_batch);
            return 1;
        }
    } else {
        // CSV 미지정 시, 기존처럼 params.prompt 사용
        if (!params.prompt.empty()) {
            all_questions.push_back(params.prompt);
        }
    }

    // csv_question_limit이 0이면 제한 없음
    int question_count = (int) all_questions.size();
    if (csv_question_limit > 0 && csv_question_limit < question_count) {
        question_count = csv_question_limit;
    }

    // [수정됨] 대화 모드가 아닌데, 질문이 없는 경우 -> 바로 종료
    if (question_count == 0 && !params.interactive) {
        LOG_INF("No prompt or CSV questions provided; nothing to do.\n");

        common_sampler_free(smpl);
        llama_backend_free();
        ggml_threadpool_free_fn(threadpool);
        ggml_threadpool_free_fn(threadpool_batch);
        return 0;
    }

    // ----------------------------------------------------------------------------
    // **질문 반복**: CSV의 여러 줄 또는 단일 prompt 하나
    // ----------------------------------------------------------------------------
    for (int q_idx = 0; q_idx < question_count; q_idx++) {
        LOG_INF("\n-------------------------\n");
        LOG_INF("[ Question %d/%d ]\n", q_idx + 1, (int)all_questions.size());
        if (!path_csv.empty()) {
            LOG_INF("Prompt from CSV: %s\n", all_questions[q_idx].c_str());
        }
        LOG_INF("-------------------------\n");

        // prompt 설정
        params.prompt = all_questions[q_idx];

        // 세션 캐시(프롬프트 캐시) 사용 시 파일명
        std::string path_session = params.path_prompt_cache;
        std::vector<llama_token> session_tokens;

        if (!path_session.empty()) {
            LOG_INF("%s: attempting to load saved session from '%s'\n", __func__, path_session.c_str());
            if (!file_exists(path_session)) {
                LOG_INF("%s: session file does not exist, will create.\n", __func__);
            } else if (file_is_empty(path_session)) {
                LOG_INF("%s: The session file is empty. A new session will be initialized.\n", __func__);
            } else {
                // 파일이 존재 & 비어있지 않음 -> 불러오기
                session_tokens.resize(params.n_ctx);
                size_t n_token_count_out = 0;
                if (!llama_state_load_file(ctx, path_session.c_str(), session_tokens.data(), session_tokens.capacity(), &n_token_count_out)) {
                    LOG_ERR("%s: failed to load session file '%s'\n", __func__, path_session.c_str());
                } else {
                    session_tokens.resize(n_token_count_out);
                    LOG_INF("%s: loaded a session with prompt size of %d tokens\n", __func__, (int)session_tokens.size());
                }
            }
        }

        // 질문마다 KV 캐시를 **독립적으로** 쓰고 싶다면, 여기서 리셋
        llama_kv_cache_seq_rm(ctx, 0, 0, -1);
        common_sampler_reset(smpl);

        const bool add_bos = llama_vocab_get_add_bos(vocab);
        if (!llama_model_has_encoder(model)) {
            GGML_ASSERT(!llama_vocab_get_add_eos(vocab));
        }

        std::vector<llama_token> embd_inp;
        {
            // 대화 모드인 경우, system 메시지/템플릿을 쓸지 결정
            auto prompt = (params.conversation_mode && params.enable_chat_template)
                ? chat_add_and_format(model, chat_msgs, "system",
                                      params.prompt.empty() ? DEFAULT_SYSTEM_MESSAGE : params.prompt)
                : params.prompt;

            // **사용자 prompt**(혹은 system 메시지)가 아예 없을 수도 있음
            if (params.interactive_first || !params.prompt.empty() || session_tokens.empty()) {
                // 통상적 케이스: 스트링 프롬프트 토큰화
                embd_inp = common_tokenize(ctx, prompt, true, true);
            } else {
                // 세션 토큰 재활용
                embd_inp = session_tokens;
            }

            if (embd_inp.empty()) {
                if (add_bos) {
                    embd_inp.push_back(llama_vocab_bos(vocab));
                }
            }

            if ((int) embd_inp.size() > params.n_ctx - 4) {
                LOG_ERR("%s: prompt is too long (%d tokens, max %d)\n", __func__, (int) embd_inp.size(), params.n_ctx - 4);
                // 다음 질문으로 넘어가도록
                continue;
            }
        }

        if (params.n_keep < 0 || params.n_keep > (int) embd_inp.size()) {
            params.n_keep = (int) embd_inp.size();
        } else {
            params.n_keep += add_bos; // always keep the BOS token
        }
        if (params.conversation_mode) {
            params.interactive_first = true;
        }
        if (params.interactive_first) {
            params.interactive = true;
        }

        std::vector<llama_token> input_tokens;
        std::vector<llama_token> output_tokens;
        std::ostringstream       output_ss;
        g_input_tokens  = &input_tokens;
        g_output_tokens = &output_tokens;
        g_output_ss     = &output_ss;

        bool is_antiprompt        = false;
        bool input_echo           = true;
        bool display              = true;
        bool need_to_save_session = !path_session.empty();

        int n_past             = 0;
        int n_remain           = params.n_predict;
        int n_consumed         = 0;
        int n_session_consumed = 0;

        // antiprompt
        std::vector<std::vector<llama_token>> antiprompt_ids;
        antiprompt_ids.reserve(params.antiprompt.size());
        for (const std::string & ap : params.antiprompt) {
            antiprompt_ids.emplace_back(common_tokenize(ctx, ap, false, true));
        }

        // 대화형 모드 준비
        if (params.interactive) {
            is_interacting = params.interactive_first;
        }
        std::ostringstream assistant_ss; // 대화에서 system/assistant 메시지 축적용

        // int ga_i = 0; // grp_attn용
        // [수정됨] 사용하지 않으면 주석/삭제

        // --------------------------------------------------------------------------------
        // 아래부터는 “토큰 생성” 메인 while 루프 (원본 코드를 그대로 재현)
        // --------------------------------------------------------------------------------

        // 인코더가 있는 모델(예: alpaca2) -> prompt 먼저 encode
        if (llama_model_has_encoder(model)) {
            int enc_input_size = embd_inp.size();
            llama_token * enc_input_buf = embd_inp.data();

            if (llama_encode(ctx, llama_batch_get_one(enc_input_buf, enc_input_size))) {
                LOG_ERR("%s : failed to eval\n", __func__);
                // 다음 질문
                continue;
            }
            llama_token decoder_start_token_id = llama_model_decoder_start_token(model);
            if (decoder_start_token_id == LLAMA_TOKEN_NULL) {
                decoder_start_token_id = llama_vocab_bos(vocab);
            }
            embd_inp.clear();
            embd_inp.push_back(decoder_start_token_id);
        }

        // [수정됨] llama_decode 실패 시 이 while을 빠져나가 “다음 질문”으로 건너뛰기 위한 플래그
        bool decode_failed = false;

        while ((n_remain != 0 && !is_antiprompt) || params.interactive) {
            // 입력 토큰(embd)이 있으면 eval
            if (!embd_inp.empty()) {
                int max_embd_size = n_ctx - 4;
                if ((int) embd_inp.size() > max_embd_size) {
                    int skipped = (int) embd_inp.size() - max_embd_size;
                    embd_inp.resize(max_embd_size);
                    console::set_display(console::error);
                    LOG_WRN("<<input too long: skipped %d tokens>>", skipped);
                    console::set_display(console::reset);
                }

                // grp_attn = 1(기본) 일 때
                if (ga_n == 1) {
                    if (n_past + (int)embd_inp.size() >= n_ctx) {
                        // context full -> 멈추거나 컨텍스트 쉬프트
                        if (!params.ctx_shift) {
                            LOG_DBG("\n\n%s: context full and no shift => stopping generation\n", __func__);
                            break;
                        }
                    }
                }
                // grp_attn != 1 인 경우는 원본 동일

                // 세션 토큰 재활용
                if (n_session_consumed < (int) session_tokens.size()) {
                    size_t i = 0;
                    for (; i < embd_inp.size(); i++) {
                        if (embd_inp[i] != session_tokens[n_session_consumed]) {
                            session_tokens.resize(n_session_consumed);
                            break;
                        }
                        n_past++;
                        n_session_consumed++;
                        if (n_session_consumed >= (int) session_tokens.size()) {
                            ++i;
                            break;
                        }
                    }
                    if (i > 0) {
                        embd_inp.erase(embd_inp.begin(), embd_inp.begin() + i);
                    }
                }

                // 실제 eval
                for (int i = 0; i < (int) embd_inp.size(); i += params.n_batch) {
                    int n_eval = (int) embd_inp.size() - i;
                    if (n_eval > params.n_batch) n_eval = params.n_batch;

                    // decode
                    if (llama_decode(ctx, llama_batch_get_one(&embd_inp[i], n_eval))) {
                        LOG_ERR("%s : failed to eval\n", __func__);
                        decode_failed = true;
                        break; // for 루프 break
                    }
                    n_past += n_eval;
                }

                if (decode_failed) {
                    break; // while 루프 탈출
                }

                if (!embd_inp.empty() && !path_session.empty()) {
                    session_tokens.insert(session_tokens.end(), embd_inp.begin(), embd_inp.end());
                    n_session_consumed = session_tokens.size();
                }

                embd_inp.clear();
            }

            if ((int) embd_inp.size() <= n_consumed && !is_interacting) {
                // 아직 예측해야 할 토큰이 남은 경우 => 샘플링
                if (!path_session.empty() && need_to_save_session && !params.prompt_cache_ro) {
                    need_to_save_session = false;
                    llama_state_save_file(ctx, path_session.c_str(),
                        session_tokens.data(), session_tokens.size());
                    LOG_DBG("saved session to %s\n", path_session.c_str());
                }
                llama_token id = common_sampler_sample(smpl, ctx, -1);
                common_sampler_accept(smpl, id, /*accept_grammar=*/true);
                embd_inp.push_back(id);
                input_echo = true;
                --n_remain;
            } else {
                // 프롬프트(또는 사용자 입력) 소진
                while ((int) embd_inp.size() > n_consumed) {
                    embd_inp.push_back(embd_inp[n_consumed]);
                    common_sampler_accept(smpl, embd_inp[n_consumed], false);
                    ++n_consumed;
                    if ((int)embd_inp.size() >= params.n_batch) {
                        break;
                    }
                }
            }

            // decode 실패했으면 즉시 while 탈출
            if (decode_failed) {
                break;
            }

            // 출력
            if (input_echo && display) {
                for (auto id : embd_inp) {
                    const std::string token_str = common_token_to_piece(ctx, id, params.special);
                    LOG("%s", token_str.c_str()); // 콘솔 출력
                    output_tokens.push_back(id);
                    output_ss << token_str;
                }
            }
            if (input_echo && (int) embd_inp.size() == n_consumed) {
                console::set_display(console::reset);
                display = true;
            }

            // anti-prompt 체크
            if (!params.antiprompt.empty()) {
                const int n_prev = 32;
                const std::string last_output = common_sampler_prev_str(smpl, ctx, n_prev);
                is_antiprompt = false;
                for (auto & ap : params.antiprompt) {
                    size_t extra_padding = params.interactive ? 0 : 2;
                    size_t start_pos = last_output.size() > ap.size() + extra_padding
                                     ? last_output.size() - (ap.size() + extra_padding)
                                     : 0;
                    if (last_output.find(ap, start_pos) != std::string::npos) {
                        if (params.interactive) {
                            is_interacting = true;
                        }
                        is_antiprompt = true;
                        break;
                    }
                }
                // 토큰 단위 anti-prompt
                llama_token last_id = common_sampler_last(smpl);
                for (auto & ids : antiprompt_ids) {
                    if (ids.size() == 1 && last_id == ids[0]) {
                        if (params.interactive) {
                            is_interacting = true;
                        }
                        is_antiprompt = true;
                        break;
                    }
                }
            }

            // EOG
            if (llama_vocab_is_eog(vocab, common_sampler_last(smpl))) {
                if (params.interactive) {
                    if (!params.antiprompt.empty()) {
                        auto & ap = params.antiprompt.front();
                        auto tmp = common_tokenize(ctx, ap, false, true);
                        embd_inp.insert(embd_inp.end(), tmp.begin(), tmp.end());
                        is_antiprompt = true;
                    }
                    LOG("\n");
                    is_interacting = true;
                }
            }

            if (params.conversation_mode) {
                auto id = common_sampler_last(smpl);
                assistant_ss << common_token_to_piece(ctx, id, false);
            }

            if (n_past > 0 && is_interacting) {
                LOG_DBG("waiting for user input\n");
                if (params.conversation_mode) {
                    LOG("\n> ");
                }
                if (params.input_prefix_bos) {
                    embd_inp.push_back(llama_vocab_bos(vocab));
                }
                std::string buffer;
                if (!params.input_prefix.empty() && !params.conversation_mode) {
                    LOG("%s", params.input_prefix.c_str());
                }

                console::set_display(console::user_input);
                display = params.display_prompt;

                std::string line;
                bool another_line = true;
                do {
                    another_line = console::readline(line, params.multiline_input);
                    buffer += line;
                } while (another_line);

                console::set_display(console::reset);
                display = true;

                if (buffer.size() > 1) {
                    if (!params.input_suffix.empty() && !params.conversation_mode) {
                        LOG("%s", params.input_suffix.c_str());
                    }
                    if (params.escape) {
                        string_process_escapes(buffer);
                    }
                    bool format_chat = params.conversation_mode && params.enable_chat_template;
                    std::string user_inp = format_chat
                        ? chat_add_and_format(model, chat_msgs, "user", std::move(buffer))
                        : std::move(buffer);

                    auto line_pfx = common_tokenize(ctx, params.input_prefix, false, true);
                    auto line_inp = common_tokenize(ctx, user_inp,            false, format_chat);
                    auto line_sfx = common_tokenize(ctx, params.input_suffix, false, true);

                    if (need_insert_eot && format_chat) {
                        llama_token eot = llama_vocab_eot(vocab);
                        embd_inp.push_back(eot == LLAMA_TOKEN_NULL ? llama_vocab_eos(vocab) : eot);
                        need_insert_eot = false;
                    }
                    embd_inp.insert(embd_inp.end(), line_pfx.begin(), line_pfx.end());
                    embd_inp.insert(embd_inp.end(), line_inp.begin(), line_inp.end());
                    embd_inp.insert(embd_inp.end(), line_sfx.begin(), line_sfx.end());
                    assistant_ss.str("");
                    n_remain -= line_inp.size();
                }

                input_echo = false;
            }

            if (llama_vocab_is_eog(vocab, embd_inp.back()) && !(params.interactive)) {
                LOG(" [end of text]\n");
                break;
            }
            if (params.interactive && n_remain <= 0 && params.n_predict >= 0) {
                n_remain = params.n_predict;
                is_interacting = true;
            }
        } // while (생성)

        // decode 실패 시 => 나머지 출력 스킵, 그냥 다음 질문
        if (decode_failed) {
            LOG_ERR("Decode failed, skipping this question.\n");
            continue;
        }

        LOG("\n");
        common_perf_print(ctx, smpl);

    } // for (q_idx)

    // 전체 반복 끝

    // cleanup
    common_sampler_free(smpl);
    llama_backend_free();

    ggml_threadpool_free_fn(threadpool);
    ggml_threadpool_free_fn(threadpool_batch);

    return 0;
}
