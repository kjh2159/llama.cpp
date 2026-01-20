#pragma once

/*
 * This file is written to manage ignite parameters in internal graph compute operations
 * Thus, other parameters for ignite (e.g., csv_path, device_name, etc.) are not included here.
 * Please refer to common_params in common.h for more details.
 */

#include <stdbool.h>
#include <chrono>
#include <cstring>

#include "llama.h"
#include "hard/utils.h"

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
struct llama_igparams * get_ignite_params(struct llama_context * ctx);