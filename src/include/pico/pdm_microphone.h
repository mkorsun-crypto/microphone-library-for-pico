/*
 * Copyright (c) 2021 Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _PICO_PDM_MICROPHONE_H_
#define _PICO_PDM_MICROPHONE_H_

#include "hardware/pio.h"
#include "pico/stdlib.h"

#ifdef __cplusplus
extern "C" {
#endif

// Настройки микрофона (I2S disguised as PDM)
struct pdm_microphone_config {
    uint gpio_data;
    uint gpio_clk; // Это будет BCLK (SCK)
    uint gpio_ws;  // НОВОЕ: Это будет LRCLK (WS)
    PIO pio;
    uint pio_sm;
    uint sample_rate;
    uint sample_buffer_size;
};

int pdm_microphone_init(const struct pdm_microphone_config *config);
void pdm_microphone_start(void);
int pdm_microphone_read(int16_t* buffer, size_t samples);
void pdm_microphone_set_samples_ready_handler(void (*handler)(void));

#ifdef __cplusplus
}
#endif

#endif
