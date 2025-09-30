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
#include <chrono>
#include <future>
#include <atomic>

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

// dvfs library
#include "hard/record.h"
#include "hard/dvfs.h"


#include <thread>
#include <tuple>
#include "json.hpp" // JSON library

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
std::atomic_bool sigterm(false);

void ctx_kv_cache_clear(struct llama_context * ctx) {
    llama_kv_cache_clear(ctx);
}

std::tuple<int, double, int, double> llama_perf_context_print_custom(const struct llama_context * ctx, const std::string & output_filename, std::chrono::time_point<std::chrono::system_clock> start_sys_time) {
    const auto data = llama_perf_context(ctx);
    const double t_end_ms = 1e-3 * ggml_time_us();

    // ("%s:        load time = %10.2f ms\n", __func__, data.t_load_ms);
    // ("%s: prompt eval time = %10.2f ms / %5d tokens (%8.2f ms per token, %8.2f tokens per second)\n",
    //         __func__, data.t_p_eval_ms, data.n_p_eval, data.t_p_eval_ms / data.n_p_eval, 1e3 / data.t_p_eval_ms * data.n_p_eval);
    // LLAMA_LOG_INFO("%s:        eval time = %10.2f ms / %5d runs   (%8.2f ms per token, %8.2f tokens per second)\n",
    //         __func__, data.t_eval_ms, data.n_eval, data.t_eval_ms / data.n_eval, 1e3 / data.t_eval_ms * data.n_eval);
    // LLAMA_LOG_INFO("%s:       total time = %10.2f ms / %5d tokens\n", __func__, (t_end_ms - data.t_start_ms), (data.n_p_eval + data.n_eval));

    // Open the CSV file in append mode
    
    
    // Convert time_point to time_t (seconds since epoch)
    auto now_sys_time = std::chrono::system_clock::now();
    auto sys_time = std::chrono::duration_cast<std::chrono::milliseconds>(now_sys_time-start_sys_time).count();
    // system time, prefill speed, decode speed, prefill tokens, decode tokens, ttft
    std::ofstream file(output_filename, std::ios::app);
    if (file.is_open()) {
        file << std::to_string(sys_time) << "," << ( 1e3 / data.t_p_eval_ms *data.n_p_eval ) << "," << (1e3 / data.t_eval_ms * data.n_eval ) << "," 
              << data.n_p_eval << ","<< data.n_eval << "," << (data.t_p_eval_ms)<<"\n";
        file.close();
    } else {
        // LLAMA_LOG_INFO("Failed to open file: %s\n", output_filename.c_str());
    }

    return std::make_tuple(data.n_p_eval, 1e3 / data.t_p_eval_ms * data.n_p_eval, data.n_eval, 1e3 / data.t_eval_ms * data.n_eval);
}


// A parsing function for "questions.json" with very simple way
// The following is JSON file type:
// {
//   "questions": [
//     "the first content of question",
//     "the second content of question",
//     "the third content of question"
//   ]
// }


using json = nlohmann::json;

std::vector<std::string> loadQuestions(const std::string &filename) {
    std::vector<std::string> questions;
    std::ifstream file(filename);
    
    if (!file) {
        std::cerr << "Failed to open " << filename << ". Exiting.\n";
        return questions;
    }

    try {
        json jsonData;
        file >> jsonData; // JSON parsing

        if (jsonData.contains("questions") && jsonData["questions"].is_array()) {
            for (const auto& item : jsonData["questions"]) {
                if (item.is_string()) {
                    questions.push_back(item.get<std::string>());
                }
            }
        } else {
            std::cerr << "Invalid JSON format: 'data' key missing or not an array\n";
        }
    } catch (const std::exception &e) {
        std::cerr << "JSON parsing error: " << e.what() << "\n";
    }

    return questions;
}

/*
int check_hardware(const std::string device_name){
    // initialization
    // const std::string device_name = "Fold_4";
    const DVFS dvfs(device_name);
	
    // get cpu freq
    for(const auto freqs: dvfs.get_cpu_freq()){
	std::cout << "cluster: " << freqs.first << " --- { ";
	for (std::size_t i=0; i<freqs.second.size(); ++i){
    	    std::cout << freqs.second[i] << " ";
	}
	std::cout << "}\n";
    }
    
    // get ram freq
    std::cout << "DDR Freq: { ";
    for(const auto freq: dvfs.get_ddr_freq()){
	std::cout << freq << " ";
    }
    std::cout << "}\n";

	// get hard info names
	std::string names = get_records_names(dvfs);
	std::cout << names << std::endl;

    // get hard info (without systime)
	std::vector<std::string> o = get_hard_records(dvfs); //(not including systime)
														 //record_hard function includes systime
    
	// get system time
	auto now1 = std::chrono::system_clock::now();
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	auto now2 = std::chrono::system_clock::now();
	auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(now2-now1).count();

	// get hard info with systime
	std::cout << std::endl;
	now1 = std::chrono::system_clock::now();
	o = get_hard_records(dvfs);
	now2 = std::chrono::system_clock::now();
	millis = std::chrono::duration_cast<std::chrono::milliseconds>(now2-now1).count();
	o.insert(o.begin(), std::to_string(millis));
	for (std::size_t i=0; i<o.size(); ++i) std::cout << o[i] << ", ";
	std::cout << "\n" << millis << std::endl;


	// write hard_info file
	bool sigterm = false;
	record_hard(sigterm, dvfs);	
	
	
	return 0;
}
*/


static void print_usage(int argc, char ** argv) {
    (void) argc;

    LOG("\nexample usage:\n");
    LOG("\n  text generation:     %s -m your_model.gguf -p \"I believe the meaning of life is\" -n 128\n", argv[0]);
    LOG("\n  chat (conversation): %s -m your_model.gguf -p \"You are a helpful assistant\" -cnv\n", argv[0]);
    LOG("\n  (Specifying json file paht: -json-path <path>)\n");
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
        sigterm = true;
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

static std::string chat_reset_and_format(struct llama_model * model, std::vector<common_chat_msg> & chat_msgs, const std::string & role, const std::string & content) {
    common_chat_msg new_msg{role, content};
    auto formatted = common_chat_format_single(model, g_params->chat_template, chat_msgs, new_msg, role == "user");
    // chat_msgs.clear();
    chat_msgs.push_back({role, content});
    LOG_DBG("formatted: '%s'\n", formatted.c_str());
    // std::cout << "[ ";
    // for (size_t i = 0; i < chat_msgs.size(); i++) {
    //     std::cout << chat_msgs[i];
    //     if (i < chat_msgs.size() - 1) std::cout << ", ";
    // }
    // std::cout << " ]\n";
    return formatted;
}

int main(int argc, char ** argv) {
    auto start_sys_time = std::chrono::system_clock::now();
    common_params params;
    g_params = &params;
    if (!common_params_parse(argc, argv, params, LLAMA_EXAMPLE_MAIN, print_usage)) {
        return 1;
    }

    // --- cmd line: read json file path --- 
    std::string jsonFilePath = "questions.json";
    for (int i = 1; i < argc; i++) {
        if (std::string(argv[i]) == "--json-path" && i + 1 < argc) {
            jsonFilePath = argv[i + 1];
        }
    }
    std::string device_name = ".json";
    for (int i = 1; i < argc; i++) {
        if (std::string(argv[i]) == "--device-name" && i + 1 < argc) {
            device_name = argv[i + 1];
            //check_hardware(device_name);
        }
    }
    std::string output_csv_path, output_txt_path;
    for (int i = 1; i < argc; i++) {
        if (std::string(argv[i]) == "--output-path" && i + 1 < argc) {
            output_csv_path = argv[i + 1]; 
            output_txt_path = argv[i + 1];
            size_t tmp_pos = output_csv_path.find_last_of(".");
            if (tmp_pos!= std::string::npos) {
                output_csv_path.insert(tmp_pos, "_infer"); 
                output_txt_path.replace(tmp_pos, std::string::npos, "_hard.txt");
            }
            // ex) --output-path /abc/mypath/data.csv -> output_csv_path = "/abc/mypath/data_infer.csv"
            break;
            //check_hardware(device_name);
        } else {
            // system time, prefill speed, decode speed, prefill tokens, decode tokens, ttft
            output_csv_path = std::string(INFER_RECORD_FILE);
        }
    }
    
    // double dp_itvl = 0.0;  // initial value
    // for (int i = 1; i < argc; i++) {
    //     if (std::string(argv[i]) == "--dp-itvl" && i + 1 < argc) {
    //         dp_itvl =  std::stod(argv[i + 1]) / 1000.0;
    //     }
    // }

    std::ofstream file(output_csv_path, std::ios::app);
    if (file.is_open() && output_csv_path!="") {
        file << "sys_time" << "," <<"prefil_time(token/sec)"<< "," << "decode_time(token/sec)" << "," 
              << "prefil_tokens" << ","<< "decode_tokens" << "," << "ttft" <<"\n";
        file.close();
    }
    
    int cpu_freq_idx;
    for (int i = 1; i < argc; i++) {
        if (std::string(argv[i]) == "--cpu-freq" && i + 1 < argc) {
            cpu_freq_idx = std::stoi(argv[i + 1]);
            //check_hardware(device_name);
        }
    }

    int ram_freq_idx;
    for (int i = 1; i < argc; i++) {
        if (std::string(argv[i]) == "--ram-freq" && i + 1 < argc) {
            ram_freq_idx = std::stoi(argv[i + 1]);
            //check_hardware(device_name);
        }
    }

// --------------------------------------------------
// 하드용 initialization
    DVFS dvfs(device_name);
    // set file path
    if (output_txt_path != "") { dvfs.output_filename = output_txt_path; } 
    else { dvfs.output_filename = std::string(HARD_RECORD_FILE); }
    
    // set cpu & ram freqs
    const std::vector<int> cpu_freq_indices = dvfs.get_cpu_freqs_conf(cpu_freq_idx);
    dvfs.set_cpu_freq(cpu_freq_indices);
    dvfs.set_ram_freq(ram_freq_idx);

// --------------------------------------------------

// ----------------------------------------------------------------
// bg hard recording 시작
    // clang 19.1.7 not supported
    //std::future<void> result = std::async(std::launch::async, record_hard, sigterm, dvfs);
    //std::packaged_task<void()> task([&dvfs] { record_hard(std::ref(sigterm), dvfs); });
    //std::future<void> result = task.get_future();
    //std::thread(std::move(task)).detach();
    std::thread record_thread = std::thread(record_hard, std::ref(sigterm), dvfs);
// ----------------------------------------------------------------


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
    auto * ggml_threadpool_new_fn = (decltype(ggml_threadpool_new) *) ggml_backend_reg_get_proc_address(reg, "ggml_threadpool_new");
    auto * ggml_threadpool_free_fn = (decltype(ggml_threadpool_free) *) ggml_backend_reg_get_proc_address(reg, "ggml_threadpool_free");

    struct ggml_threadpool_params tpp_batch = ggml_threadpool_params_from_cpu_params(params.cpuparams_batch);
    struct ggml_threadpool_params tpp = ggml_threadpool_params_from_cpu_params(params.cpuparams);
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
    const int n_ctx = llama_n_ctx(ctx);
    if (n_ctx > n_ctx_train) {
        LOG_WRN("%s: model was trained on only %d context tokens (%d specified)\n", __func__, n_ctx_train, n_ctx);
    }

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

    std::string path_session = params.path_prompt_cache;
    std::vector<llama_token> session_tokens;
    if (!path_session.empty()) {
        LOG_INF("%s: attempting to load saved session from '%s'\n", __func__, path_session.c_str());
        if (!file_exists(path_session)) {
            LOG_INF("%s: session file does not exist, will create.\n", __func__);
        } else if (file_is_empty(path_session)) {
            LOG_INF("%s: The session file is empty. A new session will be initialized.\n", __func__);
        } else {
            session_tokens.resize(n_ctx);
            size_t n_token_count_out = 0;
            if (!llama_state_load_file(ctx, path_session.c_str(), session_tokens.data(), session_tokens.capacity(), &n_token_count_out)) {
                LOG_ERR("%s: failed to load session file '%s'\n", __func__, path_session.c_str());
                return 1;
            }
            session_tokens.resize(n_token_count_out);
            LOG_INF("%s: loaded a session with prompt size of %d tokens\n", __func__, (int)session_tokens.size());
        }
    }

    const bool add_bos = llama_vocab_get_add_bos(vocab);
    if (!llama_model_has_encoder(model)) {
        GGML_ASSERT(!llama_vocab_get_add_eos(vocab));
    }
    LOG_DBG("n_ctx: %d, add_bos: %d\n", n_ctx, add_bos);

    std::vector<llama_token> embd_inp;
    {
        auto prompt = (params.conversation_mode && params.enable_chat_template)
            ? chat_add_and_format(model, chat_msgs, "system", params.prompt.empty() ? DEFAULT_SYSTEM_MESSAGE : params.prompt)
            : params.prompt;
        if (params.interactive_first || !params.prompt.empty() || session_tokens.empty()) {
            LOG_DBG("tokenize the prompt\n");
            embd_inp = common_tokenize(ctx, prompt, true, true);
        } else {
            LOG_DBG("use session tokens\n");
            embd_inp = session_tokens;
        }
        LOG_DBG("prompt: \"%s\"\n", prompt.c_str());
        LOG_DBG("tokens: %s\n", string_from(ctx, embd_inp).c_str());
    }
    if (embd_inp.empty()) {
        if (add_bos) {
            embd_inp.push_back(llama_vocab_bos(vocab));
            LOG_WRN("embd_inp was considered empty and bos was added: %s\n", string_from(ctx, embd_inp).c_str());
        } else {
            LOG_ERR("input is empty\n");
            return -1;
        }
    }
    if ((int) embd_inp.size() > n_ctx - 4) {
        LOG_ERR("%s: prompt is too long (%d tokens, max %d)\n", __func__, (int) embd_inp.size(), n_ctx - 4);
        return 1;
    }

    size_t n_matching_session_tokens = 0;
    if (!session_tokens.empty()) {
        for (llama_token id : session_tokens) {
            if (n_matching_session_tokens >= embd_inp.size() || id != embd_inp[n_matching_session_tokens]) {
                break;
            }
            n_matching_session_tokens++;
        }
        if (params.prompt.empty() && n_matching_session_tokens == embd_inp.size()) {
            LOG_INF("%s: using full prompt from session file\n", __func__);
        } else if (n_matching_session_tokens >= embd_inp.size()) {
            LOG_INF("%s: session file has exact match for prompt!\n", __func__);
        } else if (n_matching_session_tokens < (embd_inp.size() / 2)) {
            LOG_WRN("%s: session file has low similarity to prompt (%zu / %zu tokens); will mostly be reevaluated\n",
                    __func__, n_matching_session_tokens, embd_inp.size());
        } else {
            LOG_INF("%s: session file matches %zu / %zu tokens of prompt\n",
                    __func__, n_matching_session_tokens, embd_inp.size());
        }
        llama_kv_cache_seq_rm(ctx, -1, n_matching_session_tokens, -1);
    }
    if (!embd_inp.empty() && n_matching_session_tokens == embd_inp.size() && session_tokens.size() > embd_inp.size()) {
        LOG_DBG("recalculate the cached logits (do): session_tokens.resize( %zu )\n", embd_inp.size() - 1);
        session_tokens.resize(embd_inp.size() - 1);
    }
    if (params.n_keep < 0 || params.n_keep > (int) embd_inp.size()) {
        params.n_keep = (int)embd_inp.size();
    } else {
        params.n_keep += add_bos;
    }
    if (params.conversation_mode) {
        params.interactive_first = true;
    }
    if (params.interactive_first) {
        params.interactive = true;
    }
    if (params.verbose_prompt) {
        LOG_INF("%s: prompt: '%s'\n", __func__, params.prompt.c_str());
        LOG_INF("%s: number of tokens in prompt = %zu\n", __func__, embd_inp.size());
        for (int i = 0; i < (int) embd_inp.size(); i++) {
            LOG_INF("%6d -> '%s'\n", embd_inp[i], common_token_to_piece(ctx, embd_inp[i]).c_str());
        }
        if (params.n_keep > add_bos) {
            LOG_INF("%s: static prompt based on n_keep: '", __func__);
            for (int i = 0; i < params.n_keep; i++) {
                LOG_CNT("%s", common_token_to_piece(ctx, embd_inp[i]).c_str());
            }
            LOG_CNT("'\n");
        }
        LOG_INF("\n");
    }

    // --- Input json file instead of cli input ---
    std::vector<std::string> json_questions;
    size_t current_question_index = 0;
    if (params.interactive) {
        json_questions = loadQuestions(jsonFilePath);
        if (json_questions.empty()) {
            LOG_ERR("No questions loaded from %s. Exiting interactive mode.\n", jsonFilePath.c_str());
            return 1;
        }
    }
    // --- JSON questions load done ---

    // ctrl+C handling
    {
#if defined (__unix__) || (defined (__APPLE__) && defined (__MACH__))
        struct sigaction sigint_action;
        sigint_action.sa_handler = sigint_handler;
        sigemptyset(&sigint_action.sa_mask);
        sigint_action.sa_flags = 0;
        sigaction(SIGINT, &sigint_action, NULL);
#elif defined (_WIN32)
        auto console_ctrl_handler = +[](DWORD ctrl_type) -> BOOL {
            return (ctrl_type == CTRL_C_EVENT) ? (sigint_handler(SIGINT), true) : false;
        };
        SetConsoleCtrlHandler(reinterpret_cast<PHANDLER_ROUTINE>(console_ctrl_handler), true);
#endif
    }

    if (params.interactive) {
        LOG_INF("%s: interactive mode on.\n", __func__);
        if (!params.antiprompt.empty()) {
            for (const auto & antiprompt : params.antiprompt) {
                LOG_INF("Reverse prompt: '%s'\n", antiprompt.c_str());
                if (params.verbose_prompt) {
                    auto tmp = common_tokenize(ctx, antiprompt, false, true);
                    for (int i = 0; i < (int) tmp.size(); i++) {
                        LOG_INF("%6d -> '%s'\n", tmp[i], common_token_to_piece(ctx, tmp[i]).c_str());
                    }
                }
            }
        }
        if (params.input_prefix_bos) {
            LOG_INF("Input prefix with BOS\n");
        }
        if (!params.input_prefix.empty()) {
            LOG_INF("Input prefix: '%s'\n", params.input_prefix.c_str());
            if (params.verbose_prompt) {
                auto tmp = common_tokenize(ctx, params.input_prefix, true, true);
                for (int i = 0; i < (int) tmp.size(); i++) {
                    LOG_INF("%6d -> '%s'\n", tmp[i], common_token_to_piece(ctx, tmp[i]).c_str());
                }
            }
        }
        if (!params.input_suffix.empty()) {
            LOG_INF("Input suffix: '%s'\n", params.input_suffix.c_str());
            if (params.verbose_prompt) {
                auto tmp = common_tokenize(ctx, params.input_suffix, false, true);
                for (int i = 0; i < (int) tmp.size(); i++) {
                    LOG_INF("%6d -> '%s'\n", tmp[i], common_token_to_piece(ctx, tmp[i]).c_str());
                }
            }
        }
    }

    // --- timer variables ---
    std::chrono::steady_clock::time_point inference_start_time;
    bool inference_started = false;

    smpl = common_sampler_init(model, sparams);
    if (!smpl) {
        LOG_ERR("%s: failed to initialize sampling subsystem\n", __func__);
        return 1;
    }
    LOG_INF("sampler seed: %u\n", common_sampler_get_seed(smpl));
    LOG_INF("sampler params: \n%s\n", sparams.print().c_str());
    LOG_INF("sampler chain: %s\n", common_sampler_print(smpl).c_str());
    LOG_INF("generate: n_ctx = %d, n_batch = %d, n_predict = %d, n_keep = %d\n", n_ctx, params.n_batch, params.n_predict, params.n_keep);

    int ga_i = 0;
    const int ga_n = params.grp_attn_n;
    const int ga_w = params.grp_attn_w;
    if (ga_n != 1) {
        GGML_ASSERT(ga_n > 0 && "grp_attn_n must be positive");
        GGML_ASSERT(ga_w % ga_n == 0 && "grp_attn_w must be a multiple of grp_attn_n");
        LOG_INF("self-extend: n_ctx_train = %d, grp_attn_n = %d, grp_attn_w = %d\n", n_ctx_train, ga_n, ga_w);
    }
    LOG_INF("\n");

    if (params.interactive) {
        const char * control_message;
        if (params.multiline_input) {
            control_message = " - To return control to the AI, end your input with '\\'.\n"
                              " - To return control without starting a new line, end your input with '/'.\n";
        } else {
            control_message = " - Press Return to return control to the AI.\n"
                              " - To return control without starting a new line, end your input with '/'.\n"
                              " - If you want to submit another line, end your input with '\\'.\n";
        }
        LOG_INF("== Running in interactive mode. ==\n");
#if defined (__unix__) || (defined (__APPLE__) && defined (__MACH__)) || defined (_WIN32)
        LOG_INF(" - Press Ctrl+C to interject at any time.\n");
#endif
        LOG_INF("%s", control_message);
        if (params.conversation_mode && params.enable_chat_template && params.prompt.empty()) {
            LOG_INF(" - Using default system message. To change it, set a different value via -p PROMPT or -f FILE argument.\n");
        }
        LOG_INF("\n");
        is_interacting = params.interactive_first;
    }

    bool is_antiprompt = false;
    bool input_echo = true;
    bool display = true;
    bool need_to_save_session = !path_session.empty() && n_matching_session_tokens < embd_inp.size();
    int n_past = 0;
    int n_remain = params.n_predict;
    int n_consumed = 0;
    int n_session_consumed = 0;

    std::vector<int> input_tokens;  g_input_tokens = &input_tokens;
    std::vector<int> output_tokens; g_output_tokens = &output_tokens;
    std::ostringstream output_ss;    g_output_ss = &output_ss;
    std::ostringstream assistant_ss;

    console::set_display(console::prompt);
    display = params.display_prompt;

    std::vector<llama_token> embd;
    std::vector<std::vector<llama_token>> antiprompt_ids;
    antiprompt_ids.reserve(params.antiprompt.size());
    for (const std::string & antiprompt : params.antiprompt) {
        antiprompt_ids.emplace_back(common_tokenize(ctx, antiprompt, false, true));
    }

    if (llama_model_has_encoder(model)) {
        int enc_input_size = embd_inp.size();
        llama_token * enc_input_buf = embd_inp.data();
        if (llama_encode(ctx, llama_batch_get_one(enc_input_buf, enc_input_size))) {
            LOG_ERR("%s : failed to eval\n", __func__);
            return 1;
        }
        llama_token decoder_start_token_id = llama_model_decoder_start_token(model);
        if (decoder_start_token_id == LLAMA_TOKEN_NULL) {
            decoder_start_token_id = llama_vocab_bos(vocab);
        }
        embd_inp.clear();
        embd_inp.push_back(decoder_start_token_id);
    }


    while ((n_remain != 0 && !is_antiprompt) || params.interactive) {
        if (!embd.empty()) {
            int max_embd_size = n_ctx - 4;
            if ((int) embd.size() > max_embd_size) {
                const int skipped_tokens = (int) embd.size() - max_embd_size;
                embd.resize(max_embd_size);
                console::set_display(console::error);
                LOG_WRN("<<input too long: skipped %d token%s>>", skipped_tokens, skipped_tokens != 1 ? "s" : "");
                console::set_display(console::reset);
            }
            if (ga_n == 1) {
                if (n_past + (int) embd.size() >= n_ctx) {
                    if (!params.ctx_shift) {
                        LOG_DBG("\n\n%s: context full and context shift is disabled => stopping\n", __func__);
                        break;
                    }
                    if (params.n_predict == -2) {
                        LOG_DBG("\n\n%s: context full and n_predict == -%d => stopping\n", __func__, params.n_predict);
                        break;
                    }
                    const int n_left = n_past - params.n_keep;
                    const int n_discard = n_left / 2;
                    LOG_DBG("context full, swapping: n_past = %d, n_left = %d, n_ctx = %d, n_keep = %d, n_discard = %d\n",
                            n_past, n_left, n_ctx, params.n_keep, n_discard);
                    llama_kv_cache_seq_rm(ctx, 0, params.n_keep, params.n_keep + n_discard);
                    llama_kv_cache_seq_add(ctx, 0, params.n_keep + n_discard, n_past, -n_discard);
                    n_past -= n_discard;
                    LOG_DBG("after swap: n_past = %d\n", n_past);
                    LOG_DBG("embd: %s\n", string_from(ctx, embd).c_str());
                    LOG_DBG("clear session path\n");
                    path_session.clear();
                    
                }
            } else {
                while (n_past >= ga_i + ga_w) {
                    const int ib = (ga_n * ga_i) / ga_w;
                    const int bd = (ga_w / ga_n) * (ga_n - 1);
                    const int dd = (ga_w / ga_n) - ib * bd - ga_w;
                    LOG_DBG("\n");
                    LOG_DBG("shift: [%6d, %6d] + %6d -> [%6d, %6d]\n", ga_i, n_past, ib * bd, ga_i + ib * bd, n_past + ib * bd);
                    LOG_DBG("div:   [%6d, %6d] / %6d -> [%6d, %6d]\n", ga_i + ib * bd, ga_i + ib * bd + ga_w, ga_n, (ga_i + ib * bd) / ga_n, (ga_i + ib * bd + ga_w) / ga_n);
                    LOG_DBG("shift: [%6d, %6d] + %6d -> [%6d, %6d]\n", ga_i + ib * bd + ga_w, n_past + ib * bd, dd, ga_i + ib * bd + ga_w + dd, n_past + ib * bd + dd);
                    llama_kv_cache_seq_add(ctx, 0, ga_i, n_past, ib * bd);
                    llama_kv_cache_seq_div(ctx, 0, ga_i + ib * bd, ga_i + ib * bd + ga_w, ga_n);
                    llama_kv_cache_seq_add(ctx, 0, ga_i + ib * bd + ga_w, n_past + ib * bd, dd);
                    n_past -= bd;
                    ga_i += ga_w / ga_n;
                    LOG_DBG("\nn_past_old = %d, n_past = %d, ga_i = %d\n\n", n_past + bd, n_past, ga_i);
                }
            }
            if (n_session_consumed < (int) session_tokens.size()) {
                size_t i = 0;
                for (; i < embd.size(); i++) {
                    if (embd[i] != session_tokens[n_session_consumed]) {
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
                    embd.erase(embd.begin(), embd.begin() + i);
                }
            }
            for (int i = 0; i < (int) embd.size(); i += params.n_batch) {
                int n_eval = (int) embd.size() - i;
                if (n_eval > params.n_batch) {
                    n_eval = params.n_batch;
                }
                LOG_DBG("eval: %s\n", string_from(ctx, embd).c_str());
                if (llama_decode(ctx, llama_batch_get_one(&embd[i], n_eval))) {
                    LOG_ERR("%s : failed to eval\n", __func__);
                    return 1;
                }
                n_past += n_eval;
                LOG_DBG("n_past = %d\n", n_past);
                if (params.n_print > 0 && n_past % params.n_print == 0) {
                    LOG_DBG("\n\033[31mTokens consumed so far = %d / %d \033[0m\n", n_past, n_ctx);
                }
            }
            if (!embd.empty() && !path_session.empty()) {
                session_tokens.insert(session_tokens.end(), embd.begin(), embd.end());
                n_session_consumed = session_tokens.size();
            }
        }
        embd.clear();
        if ((int) embd_inp.size() <= n_consumed && !is_interacting) {
            if (!path_session.empty() && need_to_save_session && !params.prompt_cache_ro) {
                need_to_save_session = false;
                llama_state_save_file(ctx, path_session.c_str(), session_tokens.data(), session_tokens.size());
                LOG_DBG("saved session to %s\n", path_session.c_str());
            }
            const llama_token id = common_sampler_sample(smpl, ctx, -1);
            common_sampler_accept(smpl, id, true);
            embd.push_back(id);
            input_echo = true;
            --n_remain;
            LOG_DBG("n_remain: %d\n", n_remain);
        } else {
            LOG_DBG("embd_inp.size(): %d, n_consumed: %d\n", (int) embd_inp.size(), n_consumed);
            while ((int) embd_inp.size() > n_consumed) {
                embd.push_back(embd_inp[n_consumed]);
                common_sampler_accept(smpl, embd_inp[n_consumed], false);
                ++n_consumed;
                if ((int) embd.size() >= params.n_batch) {
                    break;
                }
            }
        }
        if (input_echo && display) {
            for (auto id : embd) {
                const std::string token_str = common_token_to_piece(ctx, id, params.special);
                //std::this_thread::sleep_for(std::chrono::duration<double>(dp_itvl));
                LOG("%s", token_str.c_str());
                if (embd.size() > 1) {
                    input_tokens.push_back(id);
                } else {
                    output_tokens.push_back(id);
                    output_ss << token_str;
                }
            }
        }
        if (input_echo && (int) embd_inp.size() == n_consumed) {
            console::set_display(console::reset);
            display = true;
        }
        if ((int) embd_inp.size() <= n_consumed) {
            if (!params.antiprompt.empty()) {
                const int n_prev = 32;
                const std::string last_output = common_sampler_prev_str(smpl, ctx, n_prev);
                is_antiprompt = false;
                for (std::string & antiprompt : params.antiprompt) {
                    size_t extra_padding = params.interactive ? 0 : 2;
                    size_t search_start_pos = last_output.length() > static_cast<size_t>(antiprompt.length() + extra_padding)
                        ? last_output.length() - static_cast<size_t>(antiprompt.length() + extra_padding)
                        : 0;
                    if (last_output.find(antiprompt, search_start_pos) != std::string::npos) {
                        if (params.interactive) {
                            is_interacting = true;
                        }
                        is_antiprompt = true;
                        break;
                    }
                }
                llama_token last_token = common_sampler_last(smpl);
                for (std::vector<llama_token> ids : antiprompt_ids) {
                    if (ids.size() == 1 && last_token == ids[0]) {
                        if (params.interactive) {
                            is_interacting = true;
                        }
                        is_antiprompt = true;
                        break;
                    }
                }
                if (is_antiprompt) {
                    LOG_DBG("found antiprompt: %s\n", last_output.c_str());
                }
            }
            if (llama_vocab_is_eog(vocab, common_sampler_last(smpl))) {
                LOG_DBG("found an EOG token\n");
                if (params.interactive) {
                    if (!params.antiprompt.empty()) {
                        const auto first_antiprompt = common_tokenize(ctx, params.antiprompt.front(), false, true);
                        embd_inp.insert(embd_inp.end(), first_antiprompt.begin(), first_antiprompt.end());
                        is_antiprompt = true;
                    }
                    if (params.enable_chat_template) {
                        chat_add_and_format(model, chat_msgs, "assistant", assistant_ss.str());
                    }
                    is_interacting = true;
                    LOG("\n");
                }
            }
            if (params.conversation_mode) {
                const auto id = common_sampler_last(smpl);
                assistant_ss << common_token_to_piece(ctx, id, false);
            }
            // -------------------------------
            // Print inference time for previous question
            if (n_past > 0 && is_interacting) {
                if (inference_started) {
                    auto inference_end_time = std::chrono::steady_clock::now();
                    auto inference_duration = std::chrono::duration_cast<std::chrono::milliseconds>(inference_end_time - inference_start_time).count();
                    // LOG_INF("Inference time for previous question: %lld ms\n", inference_duration);
                    common_perf_print(ctx, smpl);
                    if(output_csv_path!=""){
                        llama_perf_context_print_custom(ctx, output_csv_path, start_sys_time);
                    }
                    //check_hardware(device_name);
                    // common_sampler_free(smpl);
                    inference_started = false;
                }
                // -------------------------------
                LOG_DBG("waiting for user input\n");
                if (params.conversation_mode) {
                    LOG("\n> ");
                }
                if (params.input_prefix_bos) {
                    LOG_DBG("adding input prefix BOS token\n");
                    embd_inp.push_back(llama_vocab_bos(vocab));
                }
                std::string buffer;
                if (!params.input_prefix.empty() && !params.conversation_mode) {
                    LOG_DBG("appending input prefix: '%s'\n", params.input_prefix.c_str());
                    LOG("%s", params.input_prefix.c_str());
                }
                // Use next question from JSON file
                if (current_question_index < json_questions.size()) {
                    buffer = json_questions[current_question_index++];
                    ctx_kv_cache_clear(ctx);
                    llama_perf_context_reset(ctx);
                    LOG_INF("Using question from file: %s\n", buffer.c_str());
                    LOG("%s\n", buffer.c_str());
                    // Record the begining time of inference for a new question
                    inference_start_time = std::chrono::steady_clock::now();
                    inference_started = true;
                } else {
                    LOG_INF("No more questions available in %s. Exiting interactive mode.\n", jsonFilePath.c_str());
                    break;
                }
                const size_t original_size = embd_inp.size();
                if (params.escape) {
                    string_process_escapes(buffer);
                }
                bool format_chat = params.conversation_mode && params.enable_chat_template;
                chat_add_and_format(model, chat_msgs, "system", params.prompt.empty() ? DEFAULT_SYSTEM_MESSAGE : params.prompt);
                std::string user_inp = format_chat
                    ? chat_reset_and_format(model, chat_msgs, "user", std::move(buffer))
                    : std::move(buffer);
                // std::cout << "buffer" << buffer<<"/n";
                // std::cout << user_inp ;
                const auto line_pfx = common_tokenize(ctx, params.input_prefix, false, true);
                const auto line_inp = common_tokenize(ctx, user_inp, false, format_chat);
                const auto line_sfx = common_tokenize(ctx, params.input_suffix, false, true);
                LOG_DBG("input tokens: %s\n", string_from(ctx, line_inp).c_str());
                embd_inp.insert(embd_inp.end(), line_pfx.begin(), line_pfx.end());
                embd_inp.insert(embd_inp.end(), line_inp.begin(), line_inp.end());
                embd_inp.insert(embd_inp.end(), line_sfx.begin(), line_sfx.end());
                for (size_t i = original_size; i < embd_inp.size(); ++i) {
                    const llama_token token = embd_inp[i];
                    output_tokens.push_back(token);
                    output_ss << common_token_to_piece(ctx, token);
                }
                assistant_ss.str("");
                // n_remain -= line_inp.size();
                LOG_DBG("n_remain: %d\n", n_remain);
                input_echo = false;
            }
            if (n_past > 0) {
                if (is_interacting) {
                    common_sampler_reset(smpl);
                }
                is_interacting = false;
            }
        }
        if (!embd.empty() && llama_vocab_is_eog(vocab, embd.back()) && !(params.interactive)) {
            LOG(" [end of text]\n");
            break;
        }
        if (params.interactive && n_remain <= 0 && params.n_predict >= 0) {
            n_remain = params.n_predict;
            is_interacting = true;
        }
    }

    if (!path_session.empty() && params.prompt_cache_all && !params.prompt_cache_ro) {
        LOG("\n%s: saving final output to session file '%s'\n", __func__, path_session.c_str());
        llama_state_save_file(ctx, path_session.c_str(), session_tokens.data(), session_tokens.size());
    }

    sigterm = true; // hard_record done
    //result.get(); // wait for termination
    dvfs.unset_cpu_freq();
    dvfs.unset_ram_freq();
    record_thread.join();

    LOG("\n\n");
    common_perf_print(ctx, smpl);

    common_sampler_free(smpl);
    llama_backend_free();

    ggml_threadpool_free_fn(threadpool);
    ggml_threadpool_free_fn(threadpool_batch);

    return 0;
}
