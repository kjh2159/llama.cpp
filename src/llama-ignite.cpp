#include "llama-ignite.h"
#include "llama-context.h"

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
    ctx->igparams.lp = ms;
}
uint16_t llama_ignite_get_layer_pause(struct llama_context * ctx){
    return ctx->igparams.lp;
}