#include "llama-ignite.h"

#include "llama-context.h"

#include "hard/utils.h"

#include <cstring>
#include <iostream>

void llama_ignite_set_active(struct llama_context * ctx, bool active) {
    if (!ctx) {
        return;
    }

    auto * ig = ctx->get_ignite_params();
    if (ig == nullptr) {
        return;
    }

    ig->is_ignite_active = active;
}

bool llama_ignite_get_active(struct llama_context * ctx) {
    if (!ctx) {
        return false;
    }

    auto * ig = ctx->get_ignite_params();
    return ig != nullptr ? ig->is_ignite_active : false;
}

void llama_ignite_set_layer_pause(struct llama_context * ctx, uint16_t ms) {
    if (!ctx) {
        return;
    }

    auto * ig = ctx->get_ignite_params();
    if (ig == nullptr) {
        return;
    }

    ig->layer_pause = ms;
    ctx->set_ignite_params(ig);
}

uint16_t llama_ignite_get_layer_pause(struct llama_context * ctx) {
    if (!ctx) {
        return 0;
    }

    auto * ig = ctx->get_ignite_params();
    return ig != nullptr ? ig->layer_pause : 0;
}

bool init_ignite_params(struct llama_context * ctx, llama_igparams * igparams) {
    if (!ctx || !igparams) {
        return false;
    }

    ctx->set_ignite_params(igparams);
    return true;
}

struct llama_igparams * get_ignite_params(struct llama_context * ctx) {
    if (!ctx) {
        return nullptr;
    }

    return ctx->get_ignite_params();
}

bool init_ignite_filename(struct llama_context * ctx) {
    if (!ctx) {
        return false;
    }

    struct llama_igparams * ig = ctx->get_ignite_params();
    if (ig == nullptr) {
        return false;
    }

    const bool fixed_config = (ig->cpu_clk_idx_p == ig->cpu_clk_idx_d) && (ig->ram_clk_idx_p == ig->ram_clk_idx_d);
    const bool tp = (ig->token_pause > 0);
    const bool pp = (ig->phase_pause > 0);
    const bool lp = (ig->layer_pause > 0);
    const bool qi = (ig->query_interval > 0);
    char mode = 0b00000000;

    mode |= (!fixed_config) ? (1 << 0) : 0;
    mode |= pp ? (1 << 1) : 0;
    mode |= lp ? (1 << 2) : 0;
    mode |= tp ? (1 << 3) : 0;
    mode |= qi ? (1 << 4) : 0;

    std::string output_hard;
    std::string output_infer;

    switch (mode) {
    case 0:
        output_hard = joinPaths(ig->output_dir, "stream_llama.cpp_" + std::to_string(ig->cpu_clk_idx_p) + "-" + std::to_string(ig->ram_clk_idx_p) + "_hard.txt");
        output_infer = joinPaths(ig->output_dir, "stream_llama.cpp_" + std::to_string(ig->cpu_clk_idx_p) + "-" + std::to_string(ig->ram_clk_idx_p) + "_infer.txt");
        break;
    case 1:
        output_hard = joinPaths(ig->output_dir, "stream_llama.cpp_" + std::to_string(ig->cpu_clk_idx_p) + "-" + std::to_string(ig->ram_clk_idx_p) + "_to_" + std::to_string(ig->cpu_clk_idx_d) + "-" + std::to_string(ig->ram_clk_idx_d) + "_hard.txt");
        output_infer = joinPaths(ig->output_dir, "stream_llama.cpp_" + std::to_string(ig->cpu_clk_idx_p) + "-" + std::to_string(ig->ram_clk_idx_p) + "_to_" + std::to_string(ig->cpu_clk_idx_d) + "-" + std::to_string(ig->ram_clk_idx_d) + "_infer.txt");
        break;
    case 2:
        output_hard = joinPaths(ig->output_dir, "stream_llama.cpp_" + std::to_string(ig->cpu_clk_idx_p) + "-" + std::to_string(ig->ram_clk_idx_p) + "_pp_" + std::to_string(ig->phase_pause) + "_hard.txt");
        output_infer = joinPaths(ig->output_dir, "stream_llama.cpp_" + std::to_string(ig->cpu_clk_idx_p) + "-" + std::to_string(ig->ram_clk_idx_p) + "_pp_" + std::to_string(ig->phase_pause) + "_infer.txt");
        break;
    case 4:
        output_hard = joinPaths(ig->output_dir, "stream_llama.cpp_" + std::to_string(ig->cpu_clk_idx_p) + "-" + std::to_string(ig->ram_clk_idx_p) + "_lp_" + std::to_string(ig->layer_pause) + "_hard.txt");
        output_infer = joinPaths(ig->output_dir, "stream_llama.cpp_" + std::to_string(ig->cpu_clk_idx_p) + "-" + std::to_string(ig->ram_clk_idx_p) + "_lp_" + std::to_string(ig->layer_pause) + "_infer.txt");
        break;
    case 5:
        output_hard = joinPaths(ig->output_dir, "stream_llama.cpp_" + std::to_string(ig->cpu_clk_idx_p) + "-" + std::to_string(ig->ram_clk_idx_p) + "_to_" + std::to_string(ig->cpu_clk_idx_d) + "-" + std::to_string(ig->ram_clk_idx_d) + "_lp_" + std::to_string(ig->layer_pause) + "_hard.txt");
        output_infer = joinPaths(ig->output_dir, "stream_llama.cpp_" + std::to_string(ig->cpu_clk_idx_p) + "-" + std::to_string(ig->ram_clk_idx_p) + "_to_" + std::to_string(ig->cpu_clk_idx_d) + "-" + std::to_string(ig->ram_clk_idx_d) + "_lp_" + std::to_string(ig->layer_pause) + "_infer.txt");
        break;
    case 8:
        output_hard = joinPaths(ig->output_dir, "stream_llama.cpp_" + std::to_string(ig->cpu_clk_idx_p) + "-" + std::to_string(ig->ram_clk_idx_p) + "_tp_" + std::to_string(ig->token_pause) + "_hard.txt");
        output_infer = joinPaths(ig->output_dir, "stream_llama.cpp_" + std::to_string(ig->cpu_clk_idx_p) + "-" + std::to_string(ig->ram_clk_idx_p) + "_tp_" + std::to_string(ig->token_pause) + "_infer.txt");
        break;
    case 16:
        output_hard = joinPaths(ig->output_dir, "stream_llama.cpp_" + std::to_string(ig->cpu_clk_idx_p) + "-" + std::to_string(ig->ram_clk_idx_p) + "_qi_" + std::to_string(ig->query_interval) + "_hard.txt");
        output_infer = joinPaths(ig->output_dir, "stream_llama.cpp_" + std::to_string(ig->cpu_clk_idx_p) + "-" + std::to_string(ig->ram_clk_idx_p) + "_qi_" + std::to_string(ig->query_interval) + "_infer.txt");
        break;
    case 17:
        output_hard = joinPaths(ig->output_dir, "stream_llama.cpp_" + std::to_string(ig->cpu_clk_idx_p) + "-" + std::to_string(ig->ram_clk_idx_p) + "_to_" + std::to_string(ig->cpu_clk_idx_d) + "-" + std::to_string(ig->ram_clk_idx_d) + "_qi_" + std::to_string(ig->query_interval) + "_hard.txt");
        output_infer = joinPaths(ig->output_dir, "stream_llama.cpp_" + std::to_string(ig->cpu_clk_idx_p) + "-" + std::to_string(ig->ram_clk_idx_p) + "_to_" + std::to_string(ig->cpu_clk_idx_d) + "-" + std::to_string(ig->ram_clk_idx_d) + "_qi_" + std::to_string(ig->query_interval) + "_infer.txt");
        break;
    case 20:
        output_hard = joinPaths(ig->output_dir, "stream_llama.cpp_" + std::to_string(ig->cpu_clk_idx_p) + "-" + std::to_string(ig->ram_clk_idx_p) + "_lp_" + std::to_string(ig->layer_pause) + "_qi_" + std::to_string(ig->query_interval) + "_hard.txt");
        output_infer = joinPaths(ig->output_dir, "stream_llama.cpp_" + std::to_string(ig->cpu_clk_idx_p) + "-" + std::to_string(ig->ram_clk_idx_p) + "_lp_" + std::to_string(ig->layer_pause) + "_qi_" + std::to_string(ig->query_interval) + "_infer.txt");
        break;
    case 21:
        output_hard = joinPaths(ig->output_dir, "stream_llama.cpp_" + std::to_string(ig->cpu_clk_idx_p) + "-" + std::to_string(ig->ram_clk_idx_p) + "_to_" + std::to_string(ig->cpu_clk_idx_d) + "-" + std::to_string(ig->ram_clk_idx_d) + "_lp_" + std::to_string(ig->layer_pause) + "_qi_" + std::to_string(ig->query_interval) + "_hard.txt");
        output_infer = joinPaths(ig->output_dir, "stream_llama.cpp_" + std::to_string(ig->cpu_clk_idx_p) + "-" + std::to_string(ig->ram_clk_idx_p) + "_to_" + std::to_string(ig->cpu_clk_idx_d) + "-" + std::to_string(ig->ram_clk_idx_d) + "_lp_" + std::to_string(ig->layer_pause) + "_qi_" + std::to_string(ig->query_interval) + "_infer.txt");
        break;
    case 3:
    case 6:
    case 7:
    case 9:
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
    default:
        std::cerr << "[ERROR] Not Controlled Configuration\n";
        return false;
    }

    ig->fixed_config = fixed_config;
    std::strcpy(ig->output_path_hard, output_hard.c_str());
    std::strcpy(ig->output_path_infer, output_infer.c_str());
    return true;
}

void ignite_params_system_info(const llama_igparams * igparams) {
    if (!igparams) {
        return;
    }

    printf("%s: device name\t\t\t= %s\n\r", __func__, igparams->device_name);
    printf("%s: ignite active status\t\t= %s\n\r", __func__, igparams->is_ignite_active ? "ON" : "OFF");
    printf("%s: backend compute profile\t= %s\n\r", __func__, igparams->backend_compute_profile ? "ON" : "OFF");
    printf("%s: backend op breakdown\t\t= %s\n\r", __func__, igparams->backend_op_breakdown ? "ON" : "OFF");
    printf("%s: strict generation\t\t= %s\n\r", __func__, igparams->strict_limit ? "ON" : "OFF");
    printf("%s: enable thinking\t\t= %s\n\r", __func__, igparams->enable_thinking ? "ON" : "OFF");
    printf("%s: prefill CPU/RAM clock idx\t= %d / %d\n\r", __func__, igparams->cpu_clk_idx_p, igparams->ram_clk_idx_p);
    printf("%s: decode CPU/RAM clock idx\t= %d / %d\n\r", __func__, igparams->cpu_clk_idx_d, igparams->ram_clk_idx_d);
    printf("%s: input dataset path\t\t= %s\n\r", __func__, igparams->input_path);
    printf("%s: resource output file\t\t= %s\n\r", __func__, igparams->output_path_hard);
    printf("%s: llm output file\t\t= %s\n\r", __func__, igparams->output_path_infer);
}
