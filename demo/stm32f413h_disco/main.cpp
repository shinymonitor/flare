// STM32F413H-DISCO
// This code implements a simple event predictor using the FLARE algorithm. 
// It reads 8-bit input from GPIO pins D0-D7, detects events on GPIO pin A0, and outputs predictions on GPIO pin A1. 
// GPIO D0-D7 : 8-bit input
// GPIO A0    : Event detect
// GPIO A1    : Event predict

#include "stm32f4xx_hal.h"

extern "C"
{
#include "flare.h"
#include "flare_config.h"
}

static FLARE_Context ctx = {0};

#define D0_GPIO_Port GPIOF
#define D0_Pin       GPIO_PIN_6
#define D1_GPIO_Port GPIOF
#define D1_Pin       GPIO_PIN_7
#define D2_GPIO_Port GPIOG
#define D2_Pin       GPIO_PIN_13
#define D3_GPIO_Port GPIOF
#define D3_Pin       GPIO_PIN_10
#define D4_GPIO_Port GPIOB
#define D4_Pin       GPIO_PIN_6
#define D5_GPIO_Port GPIOE
#define D5_Pin       GPIO_PIN_6
#define D6_GPIO_Port GPIOB
#define D6_Pin       GPIO_PIN_0
#define D7_GPIO_Port GPIOC
#define D7_Pin       GPIO_PIN_13
#define DETECT_GPIO_Port GPIOC
#define DETECT_Pin       GPIO_PIN_0
#define PREDICT_GPIO_Port GPIOA
#define PREDICT_Pin       GPIO_PIN_1

static void GPIO_Init_Custom(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Pin = D0_Pin;
    HAL_GPIO_Init(D0_GPIO_Port, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = D1_Pin;
    HAL_GPIO_Init(D1_GPIO_Port, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = D2_Pin;
    HAL_GPIO_Init(D2_GPIO_Port, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = D3_Pin;
    HAL_GPIO_Init(D3_GPIO_Port, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = D4_Pin;
    HAL_GPIO_Init(D4_GPIO_Port, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = D5_Pin;
    HAL_GPIO_Init(D5_GPIO_Port, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = D6_Pin;
    HAL_GPIO_Init(D6_GPIO_Port, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = D7_Pin;
    HAL_GPIO_Init(D7_GPIO_Port, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = DETECT_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    HAL_GPIO_Init(DETECT_GPIO_Port, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = PREDICT_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(PREDICT_GPIO_Port, &GPIO_InitStruct);
    HAL_GPIO_WritePin(PREDICT_GPIO_Port, PREDICT_Pin, GPIO_PIN_RESET);
}

static inline uint8_t ReadInputBus() {
    uint8_t value = 0;
    value |= HAL_GPIO_ReadPin(D0_GPIO_Port, D0_Pin) << 0;
    value |= HAL_GPIO_ReadPin(D1_GPIO_Port, D1_Pin) << 1;
    value |= HAL_GPIO_ReadPin(D2_GPIO_Port, D2_Pin) << 2;
    value |= HAL_GPIO_ReadPin(D3_GPIO_Port, D3_Pin) << 3;
    value |= HAL_GPIO_ReadPin(D4_GPIO_Port, D4_Pin) << 4;
    value |= HAL_GPIO_ReadPin(D5_GPIO_Port, D5_Pin) << 5;
    value |= HAL_GPIO_ReadPin(D6_GPIO_Port, D6_Pin) << 6;
    value |= HAL_GPIO_ReadPin(D7_GPIO_Port, D7_Pin) << 7;
    return value;
}

int main(void) {
    HAL_Init();
    GPIO_Init_Custom();
    FLARE_init(&ctx, 64, 0);
    while (1) {
        uint8_t input = ReadInputBus();
        uint8_t detect = HAL_GPIO_ReadPin(DETECT_GPIO_Port, DETECT_Pin);
        uint8_t predict = FLARE_update(&ctx, input, detect);
        HAL_GPIO_WritePin(PREDICT_GPIO_Port, PREDICT_Pin, predict ? GPIO_PIN_SET : GPIO_PIN_RESET);
    }
}