#include <stdint.h>

#define FLARE_Input_Type int8_t
#define FLARE_Input_Type_MAX INT8_MAX
#define FLARE_Input_Type_MIN INT8_MIN

#define FLARE_W 32
#define FLARE_H 16
#define FLARE_L 2

#define FLARE_A_SCALE 0.25f
#define FLARE_W_SCALE 0.02f
#define FLARE_INIT_ALPHA 0.001f
#define FLARE_LR_NO_DECAY
#define FLARE_L2_LAMBDA 0.0f
#define FLARE_GRADIENT_CLIP 1e13f
#define FLARE_PRUNE_THRESHOLD 0.0f
#define FLARE_BETA1 0.9f
#define FLARE_BETA2 0.999f
#define FLARE_EPS 1e-8f
