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