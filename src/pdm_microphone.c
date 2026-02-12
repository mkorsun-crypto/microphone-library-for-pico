#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/dma.h"
#include "hardware/irq.h"
#include "hardware/clocks.h"

#include "pdm_microphone.h"
#include "pdm_microphone.pio.h" 

static struct pdm_microphone_config _config;
static int16_t* _sample_buffer = NULL;
static uint32_t* _raw_buffer = NULL;
static pdm_samples_ready_handler_t _on_samples_ready = NULL;

int pdm_microphone_init(const struct pdm_microphone_config *config) {
    memcpy(&_config, config, sizeof(struct pdm_microphone_config));
    
    if (_config.sample_buffer_size == 0) _config.sample_buffer_size = 256;
    
    _sample_buffer = malloc(_config.sample_buffer_size * sizeof(int16_t));
    _raw_buffer = malloc(_config.sample_buffer_size * sizeof(uint32_t));

    if (!_sample_buffer || !_raw_buffer) return -1;

    gpio_init(_config.gpio_data);
    gpio_set_dir(_config.gpio_data, GPIO_IN);
    gpio_init(_config.gpio_clk);
    gpio_set_dir(_config.gpio_clk, GPIO_OUT);

    return 0; 
}

void pdm_microphone_deinit(void) {
    if (_sample_buffer) free(_sample_buffer);
    if (_raw_buffer) free(_raw_buffer);
}

// ВАЖНО: Возвращает 0 (int)
int pdm_microphone_start(void) {
    return 0;
}

void pdm_microphone_stop(void) {
}

void pdm_microphone_set_samples_ready_handler(pdm_samples_ready_handler_t handler) {
    _on_samples_ready = handler;
}

int16_t* pdm_microphone_read(void) {
    if (_sample_buffer) {
        memset(_sample_buffer, 0, _config.sample_buffer_size * sizeof(int16_t));
    }
    return _sample_buffer;
}

void pdm_microphone_set_filter_max_volume(uint8_t max_volume) { (void)max_volume; }
void pdm_microphone_set_filter_gain(uint8_t gain) { (void)gain; }
