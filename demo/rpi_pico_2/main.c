// RPi Pico 2
// This code implements a simple event predictor using the FLARE algorithm. 
// It reads 8-bit input from GPIO pins 0-7, detects events on GPIO pin 8, and outputs predictions on GPIO pin 9. 
// The built-in LED is used to indicate that the program is running.
// GPIO 0-7 : 8-bit input
// GPIO 8   : Event detect
// GPIO 9   : Event predict

#include "flare_config.h"
#include "flare.h"
#include "pico/stdlib.h"

static FLARE_Context ctx = {0};

int main() {
    const uint LED_PIN = PICO_DEFAULT_LED_PIN;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    FLARE_init(&ctx, 64, 0);

    for (uint gpio = 0; gpio < 8; ++gpio) {
        gpio_init(gpio);
        gpio_set_dir(gpio, GPIO_IN);
        gpio_pull_down(gpio);
    }

    gpio_init(8);
    gpio_set_dir(8, GPIO_IN);
    gpio_pull_down(8);

    gpio_init(9);
    gpio_set_dir(9, GPIO_OUT);

    uint8_t input = 0;

    gpio_put(LED_PIN, 1);

    while (true) {
        input = 0;
        for (uint gpio = 0; gpio < 8; ++gpio) input |= (gpio_get(gpio) << gpio);
        gpio_put(9, FLARE_update(&ctx, input, gpio_get(8)));
    }
}