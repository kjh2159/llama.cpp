#pragma once
#include <stdbool.h>

#include "llama.h"

#ifdef __cplusplus
extern "C"{
#endif

struct llama_context; // opaque

void llama_ignite_set_active(struct llama_context * ctx, bool active);
bool llama_ignite_get_active(struct llama_context * ctx);

#ifdef __cplusplus
}
#endif

struct llama_igparams {
    bool is_ignite_active = false;
};