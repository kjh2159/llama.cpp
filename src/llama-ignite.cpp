#include "llama-ignite.h"
#include "llama-context.h"

void llama_ignite_set_active(struct llama_context * ctx, bool active){
    if (!ctx) return;
    ctx->igparams.is_ignite_active = active;
}

bool llama_ignite_get_active(struct llama_context * ctx){
    
}