#pragma once

/*
 * This file is written to manage ignite parameters in internal graph compute operations
 * Thus, other parameters for ignite (e.g., csv_path, device_name, etc.) are not included here.
 * Please refer to common_params in common.h for more details.
 */

#include <stdbool.h>
#include <chrono>

#include "llama.h"

#ifdef __cplusplus
extern "C"{
#endif

struct llama_context; // opaque
struct llama_igparams; // opaque

void llama_ignite_set_active(struct llama_context * ctx, bool active);
bool llama_ignite_get_active(struct llama_context * ctx);
void llama_ignite_set_layer_pause(struct llama_context * ctx,  uint16_t ms);
uint16_t llama_ignite_get_layer_pause(struct llama_context * ctx);

#ifdef __cplusplus
}
#endif

// The ignite parameters are managed by llama_context internally
// Thus, other parameters for ignite (e.g., csv_path, device_name, etc.) are not included here.
// Please refer to `llama.h` for more details.
bool init_ignite_params(struct llama_context * ctx, llama_igparams* igparams);



// struct llama_igparams {
//     llama_igparams();
//     ~llama_igparams();

// // graph internal parameters
//     bool is_ignite_active = false;
//     uint16_t layer_pause = 0; // ms

// // graph external parameters
//     // basic additional parameters
//     bool strict_limit = false;
//     bool enable_thinking = false;

//     // llm plane
//     int phase_pause = 0; // ms
//     int token_pause = 0; // ms
//     int query_interval = 0; // ms
//     bool prefill_phase = true; // prefill phase or not
//     double prefill_speed = 0.0; // tokens/s
//     double decode_speed = 0.0; // tokens/s

//     // basic measure configs
//     int csv_limit = 0;       // limit of CSV questions (0=no limit) // deprecated in future
//     std::string json_path = "questions.json"; // deprecated in future
//     std::string output_csv_path; // deprecated in future
//     std::string input_path = ""; // path = dir/file.ext
//     std::string output_dir = "";
//     std::string output_path_hard = "";
//     std::string output_path_infer = "";
    
//     // [OPT. 1] resource plane (static ignite)
//     std::string device_name;
//     int cpu_clk_idx_p = 0; // prefill + cpu
//     int ram_clk_idx_p = 0; // prefill + ram
//     int cpu_clk_idx_d = 0; // decode + cpu
//     int ram_clk_idx_d = 0; // decode + ram
//     bool fixed_config = false;

//     // [OPT. 2] resource plane (agent ignite)
//     double time_slot = 0.5; // s
//     double temp_threshold = 80.0; // Celsius
//     std::vector<double> temp_history = {}; // temperature history
//     int temp_cap = 10; // max length of temperature history
//     double temp_alpha = 0.6; // for EMA
//     int max_cpu_clk_idx = 0; // fixed by device
//     int cur_cpu_clk_idx = 0; // dynamic
//     int max_ram_clk_idx = 0; // fixed by device
//     int cur_ram_clk_idx = 0; // dynamic
// };