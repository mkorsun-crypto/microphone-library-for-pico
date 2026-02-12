#ifndef PDM_MICROPHONE_H
#define PDM_MICROPHONE_H

#include "pico/stdlib.h"

#ifdef __cplusplus
extern "C" {
#endif

struct pdm_microphone_config {
    uint gpio_data;
    uint gpio_clk;
    PIO pio;
    uint pio_sm;
    uint sample_rate;
    uint sample_buffer_size;
};

// ВАЖНО: Возвращает int
int pdm_microphone_init(const struct pdm_microphone_config *config);

void pdm_microphone_deinit(void);

// ВАЖНО: Возвращает int (было void)
int pdm_microphone_start(void);

void pdm_microphone_stop(void);

typedef void (*pdm_samples_ready_handler_t)(void);
void pdm_microphone_set_samples_ready_handler(pdm_samples_ready_handler_t handler);

int16_t* pdm_microphone_read(void);

void pdm_microphone_set_filter_max_volume(uint8_t max_volume);
void pdm_microphone_set_filter_gain(uint8_t gain);

#ifdef __cplusplus
}
#endif

#endif
