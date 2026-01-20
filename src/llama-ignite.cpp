#include "llama-ignite.h"
#include "llama-context.h"


// ignite params does not dynamically allocate resources internally
// llama_igparams::llama_igparams(){}
// llama_igparams::~llama_igparams(){}

// ignite activation
void llama_ignite_set_active(struct llama_context * ctx, bool active){
    if (!ctx) return;
    ctx->igparams.is_ignite_active = active;
}
bool llama_ignite_get_active(struct llama_context * ctx){
    return ctx->igparams.is_ignite_active;
}

// layer-pause (in-module)
void llama_ignite_set_layer_pause(struct llama_context * ctx,  uint16_t ms){
    if (!ctx) return;
    ctx->igparams.layer_pause = ms;
}
uint16_t llama_ignite_get_layer_pause(struct llama_context * ctx){
    return ctx->igparams.layer_pause;
}

bool init_ignite_params(struct llama_context * ctx, llama_igparams* igparams) {
    if (!ctx || !igparams) return false;

    ctx->set_ignite_params(igparams);

    return true;
}

struct llama_igparams *  get_ignite_params(struct llama_context * ctx) {
    return ctx->get_ignite_params();
}

bool init_ignite_filename(struct llama_context * ctx) {
    struct llama_igparams * _cp = ctx->get_ignite_params();

    // variable initialization: For File Naming
    bool fixed_config = (_cp->cpu_clk_idx_p == _cp->cpu_clk_idx_d) && (_cp->ram_clk_idx_p == _cp->ram_clk_idx_d);
    bool tp = (_cp->token_pause > 0);
    bool pp = (_cp->phase_pause > 0);
    bool lp = (_cp->layer_pause > 0);
    bool qi = (_cp->query_interval > 0);
    char mode = 0b00000000; // 1byte

    // 0-th bit: clock control
    // 1-st bit: phase-pause
    // 2-nd bit: layer-pause
    // 3-rd bit: token-pause
    // 4-th bit: query-interval
    // ex) 0b0101 : clock config control + layer pause
    //     3 <-> 0

    // [control mode checker]
    mode |= (!fixed_config) ? (1 << 0) : 0;
    mode |= pp ? (1 << 1) : 0;
    mode |= lp ? (1 << 2) : 0;
    mode |= tp ? (1 << 3) : 0;
    mode |= qi ? (1 << 4) : 0;

    std::string output_hard;
    std::string output_infer;
    // TODO: change file naming rule
    switch (mode) {
    case 0:
        // Fixed Config
        output_hard = joinPaths(_cp->output_dir, "stream_llama.cpp_" + std::to_string(_cp->cpu_clk_idx_p) + "-" + std::to_string(_cp->ram_clk_idx_p) + "_hard.txt");
        output_infer = joinPaths(_cp->output_dir, "stream_llama.cpp_" + std::to_string(_cp->cpu_clk_idx_p) + "-" + std::to_string(_cp->ram_clk_idx_p) + "_infer.txt");
        break;
    case 1:
        // Only Config Control
        output_hard = joinPaths(_cp->output_dir, "stream_llama.cpp_"  + std::to_string(_cp->cpu_clk_idx_p) + "-" + std::to_string(_cp->ram_clk_idx_p) + "_to_" + std::to_string(_cp->cpu_clk_idx_d) + "-" + std::to_string(_cp->ram_clk_idx_d) + "_hard.txt");
        output_infer = joinPaths(_cp->output_dir, "stream_llama.cpp_" + std::to_string(_cp->cpu_clk_idx_p) + "-" + std::to_string(_cp->ram_clk_idx_p) + "_to_" + std::to_string(_cp->cpu_clk_idx_d) + "-" + std::to_string(_cp->ram_clk_idx_d) + "_infer.txt");
        break;
    case 2:
        // Only Phase Pause
        output_hard = joinPaths(_cp->output_dir, "stream_llama.cpp_" + std::to_string(_cp->cpu_clk_idx_p) + "-" + std::to_string(_cp->ram_clk_idx_p) + "_pp_" + std::to_string(_cp->phase_pause) + "_hard.txt");
        output_infer = joinPaths(_cp->output_dir, "stream_llama.cpp_" + std::to_string(_cp->cpu_clk_idx_p) + "-" + std::to_string(_cp->ram_clk_idx_p) + "_pp_" + std::to_string(_cp->phase_pause) + "_infer.txt");
        break;
    case 4:
        // Only Layer Pause
        output_hard = joinPaths(_cp->output_dir, "stream_llama.cpp_" + std::to_string(_cp->cpu_clk_idx_p) + "-" + std::to_string(_cp->ram_clk_idx_p) + "_lp_" + std::to_string(_cp->layer_pause) + "_hard.txt");
        output_infer = joinPaths(_cp->output_dir, "stream_llama.cpp_" + std::to_string(_cp->cpu_clk_idx_p) + "-" + std::to_string(_cp->ram_clk_idx_p) + "_lp_" + std::to_string(_cp->layer_pause) + "_infer.txt");
        break;
    case 5:
        // Config Control + Layer Pause
        output_hard = joinPaths(_cp->output_dir, "stream_llama.cpp_" + std::to_string(_cp->cpu_clk_idx_p) + "-" + std::to_string(_cp->ram_clk_idx_p) + "_to_" + std::to_string(_cp->cpu_clk_idx_d) + "-" + std::to_string(_cp->ram_clk_idx_d) + "_lp_" + std::to_string(_cp->layer_pause) + "_hard.txt");
        output_infer = joinPaths(_cp->output_dir, "stream_llama.cpp_" + std::to_string(_cp->cpu_clk_idx_p) + "-" + std::to_string(_cp->ram_clk_idx_p) + "_to_" + std::to_string(_cp->cpu_clk_idx_d) + "-" + std::to_string(_cp->ram_clk_idx_d) + "_lp_" + std::to_string(_cp->layer_pause) + "_infer.txt");
        break;
    case 8:
        // Only Token Pause
        output_hard = joinPaths(_cp->output_dir, "stream_llama.cpp_" +  std::to_string(_cp->cpu_clk_idx_p) + "-" + std::to_string(_cp->ram_clk_idx_p) + "_tp_" + std::to_string(_cp->token_pause) + "_hard.txt");
        output_infer = joinPaths(_cp->output_dir, "stream_llama.cpp_" + std::to_string(_cp->cpu_clk_idx_p) + "-" + std::to_string(_cp->ram_clk_idx_p) + "_tp_" + std::to_string(_cp->token_pause) + "_infer.txt");
        break;
    case 16:
        // Only query interval
        output_hard = joinPaths(_cp->output_dir, "stream_llama.cpp_" + std::to_string(_cp->cpu_clk_idx_p) + "-" + std::to_string(_cp->ram_clk_idx_p) + "_qi_" + std::to_string(_cp->query_interval) + "_hard.txt");
        output_infer = joinPaths(_cp->output_dir, "stream_llama.cpp_" + std::to_string(_cp->cpu_clk_idx_p) + "-" + std::to_string(_cp->ram_clk_idx_p) + "_qi_" + std::to_string(_cp->query_interval) + "_infer.txt");
        break;
    case 17:
        // Config Control + Query Interval
        output_hard = joinPaths(_cp->output_dir, "stream_llama.cpp_" + std::to_string(_cp->cpu_clk_idx_p) + "-" + std::to_string(_cp->ram_clk_idx_p) + "_to_" + std::to_string(_cp->cpu_clk_idx_d) + "-" + std::to_string(_cp->ram_clk_idx_d) + "_qi_" + std::to_string(_cp->query_interval) + "_hard.txt");
        output_infer = joinPaths(_cp->output_dir, "stream_llama.cpp_" + std::to_string(_cp->cpu_clk_idx_p) + "-" + std::to_string(_cp->ram_clk_idx_p) + "_to_" + std::to_string(_cp->cpu_clk_idx_d) + "-" + std::to_string(_cp->ram_clk_idx_d) + "_qi_" + std::to_string(_cp->query_interval) + "_infer.txt");
        break;
    case 20:
        // Layer Pause + Query Interval
        output_hard = joinPaths(_cp->output_dir, "stream_llama.cpp_" + std::to_string(_cp->cpu_clk_idx_p) + "-" + std::to_string(_cp->ram_clk_idx_p) + "_lp_" + std::to_string(_cp->layer_pause) + "_qi_" + std::to_string(_cp->query_interval) + "_hard.txt");
        output_infer = joinPaths(_cp->output_dir, "stream_llama.cpp_" + std::to_string(_cp->cpu_clk_idx_p) + "-" + std::to_string(_cp->ram_clk_idx_p) + "_lp_" + std::to_string(_cp->layer_pause) + "_qi_" + std::to_string(_cp->query_interval) + "_infer.txt");
        break;
    case 21:
        // Config Control + Layer Pause + Query Interval
        output_hard = joinPaths(_cp->output_dir, "stream_llama.cpp_" + std::to_string(_cp->cpu_clk_idx_p) + "-" + std::to_string(_cp->ram_clk_idx_p) + "_to_" + std::to_string(_cp->cpu_clk_idx_d) + "-" + std::to_string(_cp->ram_clk_idx_d) + "_lp_" + std::to_string(_cp->layer_pause) + "_qi_" + std::to_string(_cp->query_interval) + "_hard.txt");
        output_infer = joinPaths(_cp->output_dir, "stream_llama.cpp_" + std::to_string(_cp->cpu_clk_idx_p) + "-" + std::to_string(_cp->ram_clk_idx_p) + "_to_" + std::to_string(_cp->cpu_clk_idx_d) + "-" + std::to_string(_cp->ram_clk_idx_d) + "_lp_" + std::to_string(_cp->layer_pause) + "_qi_" + std::to_string(_cp->query_interval) + "_infer.txt");
        break;
    
    case 3:  // Config Control + Phase Pause
    case 6:  // Phase Pause + Layer Pause
    case 7:  // Config Control + Pase Pause + Layer Pause
    case 9:  // Config Control + Token Pause
    case 10: // Phase Pause + Token Pause
    case 11: // Config Control + Phase Pause + Token Pause
    case 12: // Layer Pause + Token Pause
    case 13: // Config Control + Layer Pause + Token Pause
    case 14: // Phase Pause + Layer Pause + Token Pause
    case 15: // Config Control + Phase Pause + Layer Pause + Token Pause
    default:
        // Not controlled cases
        std::cerr << "[ERROR] Not Controlled Configuration\n";
        return false;
    }

    _cp->fixed_config = fixed_config;
    std::strcpy(_cp->output_path_hard, output_hard.c_str());
    std::strcpy(_cp->output_path_infer, output_infer.c_str());
    
    // _cp->output_path_hard = output_hard;
    // _cp->output_path_infer = output_infer;
    std::cout << "[IGNITE] Hard Filename: " << _cp->output_path_hard << "\n\r";
    return true;
}