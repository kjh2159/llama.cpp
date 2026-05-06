#pragma once

/*
 * This file is written to manage ignite parameters in internal graph compute operations.
 * Please refer to common_params in common.h for CLI-facing details.
 */

#include <stdbool.h>
#include <cstdint>

#include "llama.h"

#ifdef __cplusplus
extern "C" {
#endif

struct llama_context;  // opaque
struct llama_igparams; // opaque

void llama_ignite_set_active(struct llama_context * ctx, bool active);
bool llama_ignite_get_active(struct llama_context * ctx);
void llama_ignite_set_layer_pause(struct llama_context * ctx, uint16_t ms);
uint16_t llama_ignite_get_layer_pause(struct llama_context * ctx);

#ifdef __cplusplus
}
#endif

bool init_ignite_params(struct llama_context * ctx, llama_igparams * igparams);
struct llama_igparams * get_ignite_params(struct llama_context * ctx);
bool init_ignite_filename(struct llama_context * ctx);
void ignite_params_system_info(const llama_igparams * igparams);
