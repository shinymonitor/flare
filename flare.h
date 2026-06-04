#ifndef FLARE_H_
#define FLARE_H_

#define FLARE_VERSION "0.1.0"

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#define QMTIK_Q_Type FLARE_Input_Type
#define QMTIK_Q_Type_MAX FLARE_Input_Type_MAX
#define QMTIK_Q_Type_MIN FLARE_Input_Type_MIN
#define QMTIK_I FLARE_W
#define QMTIK_H FLARE_H
#define QMTIK_L FLARE_L
#define QMTIK_O 2
#define QMTIK_A_SCALE FLARE_A_SCALE
#define QMTIK_W_SCALE FLARE_W_SCALE
#define QMTIK_LEAKY_RELU_ACTV
#define QMTIK_SOFT_MAX_PP
#define QMTIK_ARGMAX_COST
#define QMTIK_INIT_ALPHA FLARE_INIT_ALPHA
#ifdef FLARE_LR_NO_DECAY
#define QMTIK_LR_NO_DECAY
#else
#ifdef FLARE_LR_STEP_DECAY
#define QMTIK_LR_STEP_DECAY
#endif
#ifdef FLARE_LR_EXPONENTIAL_DECAY
#define QMTIK_LR_EXPONENTIAL_DECAY
#endif
#define QMTIK_LR_DECAY_RATE FLARE_LR_DECAY_RATE
#define QMTIK_LR_DECAY_STEPS FLARE_LR_DECAY_STEPS
#endif
#define QMTIK_L2_LAMBDA FLARE_L2_LAMBDA
#define QMTIK_GRADIENT_CLIP FLARE_GRADIENT_CLIP
#define QMTIK_PRUNE_THRESHOLD FLARE_PRUNE_THRESHOLD
#define QMTIK_BETA1 FLARE_BETA1
#define QMTIK_BETA2 FLARE_BETA2
#define QMTIK_EPS FLARE_EPS
#define QMTIK_ENABLE_TRAINING
#include "qmtik.h"

#define FLARE_Type QMTIK_B_Type

typedef struct {
    FLARE_Type* context;
    size_t context_capacity;
    size_t rollback;
    bool predicted;
    size_t predicted_counter;
    QMTIK_B_Network network;
    QMTIK_B_Sample sample;
    FLARE_Type infer_output[2];
} FLARE_Context;

static inline bool FLARE_init(FLARE_Context* ctx, size_t rollback, uint32_t seed) {
    ctx->context_capacity = rollback + FLARE_W;
    ctx->rollback = rollback;

    ctx->context = (FLARE_Type*)calloc(ctx->context_capacity, sizeof(FLARE_Type));
    if (!ctx->context) return false;

    QMTIK_B_init_weights(&ctx->network, seed);
    return true;
}

static inline bool FLARE_update(FLARE_Context* ctx, FLARE_Type input, bool event_detect) {
    size_t needed = ctx->rollback + FLARE_W;
    if (ctx->context_capacity != needed) {
        FLARE_Type* new_context = (FLARE_Type*)realloc(ctx->context, needed * sizeof(FLARE_Type));
        if (!new_context) return false;
        ctx->context = new_context;
        if (needed > ctx->context_capacity) memset(ctx->context + ctx->context_capacity, 0, (needed - ctx->context_capacity) * sizeof(FLARE_Type));
        ctx->context_capacity = needed;
    }
    memmove(ctx->context, ctx->context + 1, (ctx->context_capacity - 1) * sizeof(FLARE_Type));
    ctx->context[ctx->context_capacity - 1] = input;
 
    QMTIK_B_load_input(&ctx->network, ctx->context + ctx->rollback);
    QMTIK_B_forward(&ctx->network);
    QMTIK_B_get_output(&ctx->network, ctx->infer_output);
    bool prediction = ctx->infer_output[1] > ctx->infer_output[0];

    if (prediction && !ctx->predicted) memcpy(ctx->sample.input, ctx->context, FLARE_W * sizeof(FLARE_Type));

    if (ctx->predicted) {
        if (event_detect) {
            ctx->predicted = false;
            ctx->predicted_counter = 0;
        } else if (ctx->predicted_counter > 0) {
            --ctx->predicted_counter;
        } else {
            ctx->sample.output[0] = (FLARE_Type)FLARE_Input_Type_MAX;
            ctx->sample.output[1] = (FLARE_Type)0;
            QMTIK_B_accumulate_gradients(&ctx->network, &ctx->sample);
            QMTIK_B_apply_gradients(&ctx->network);
            ctx->predicted = false;
            ctx->predicted_counter = 0;
        }
    } else {
        if (prediction) {
            ctx->predicted         = true;
            ctx->predicted_counter = ctx->rollback;
        } else if (event_detect) {
            ctx->sample.output[0] = (FLARE_Type)0;
            ctx->sample.output[1] = (FLARE_Type)FLARE_Input_Type_MAX;
            memcpy(ctx->sample.input, ctx->context, FLARE_W * sizeof(FLARE_Type));
            QMTIK_B_accumulate_gradients(&ctx->network, &ctx->sample);
            QMTIK_B_apply_gradients(&ctx->network);
        }
    }
 
    return prediction;
}

static inline void FLARE_free(FLARE_Context* ctx) {
    free(ctx->context);
    ctx->context = NULL;
    ctx->context_capacity = 0;
}

#endif /* FLARE_H_ */