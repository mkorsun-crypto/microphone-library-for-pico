/*
 * I2S Microphone Driver (Imposter as PDM)
 */

#include <string.h>
#include "pico/pdm_microphone.h"
#include "pdm_microphone.pio.h"
#include "hardware/dma.h"
#include "hardware/irq.h"

// Variables
static struct pdm_microphone_config _config;
static uint _dma_channel;
static int32_t* _raw_buffer;
static void (*_samples_ready_handler)(void) = NULL;

// PIO Configuration
static void pdm_microphone_init_pio() {
    uint offset = pio_add_program(_config.pio, &pdm_microphone_data_program);
    pio_sm_config c = pdm_microphone_data_program_get_default_config(offset);

    // Pin setup
    // Side-set pins (SCK, WS)
    sm_config_set_sideset_pins(&c, _config.gpio_clk); 
    // Input pin (DATA)
    sm_config_set_in_pins(&c, _config.gpio_data);
    
    // Init GPIO
    pio_gpio_init(_config.pio, _config.gpio_clk);
    pio_gpio_init(_config.pio, _config.gpio_ws);
    pio_gpio_init(_config.pio, _config.gpio_data);
    
    // Set directions
    pio_sm_set_consecutive_pindirs(_config.pio, _config.pio_sm, _config.gpio_clk, 2, true); // SCK, WS output
    pio_sm_set_consecutive_pindirs(_config.pio, _config.pio_sm, _config.gpio_data, 1, false); // Data input

    // Shift settings: INMP441 sends MSB first.
    // We shift Left, Autopush enabled at 32 bits
    sm_config_set_in_shift(&c, false, true, 32); 
    sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_RX);

    // Clock divider calculation
    // We need 64 clocks per sample (32 bits * 2 channels)
    float div = clock_get_hz(clk_sys) / (_config.sample_rate * 64.0f);
    sm_config_set_clkdiv(&c, div);

    pio_sm_init(_config.pio, _config.pio_sm, offset, &c);
}

// DMA Handler
void __isr dma_handler() {
    dma_hw->ints0 = 1u << _dma_channel; // Clear interrupt
    if (_samples_ready_handler) {
        _samples_ready_handler();
    }
    // Re-trigger DMA
    dma_channel_set_write_addr(_dma_channel, _raw_buffer, true);
}

int pdm_microphone_init(const struct pdm_microphone_config *config) {
    memcpy(&_config, config, sizeof(struct pdm_microphone_config));
    
    // Allocate internal buffer for raw 32-bit I2S data (Stereo)
    // Size is * 2 because we get L and R channels
    _raw_buffer = malloc(_config.sample_buffer_size * 2 * sizeof(int32_t));
    if (_raw_buffer == NULL) return -1;

    pdm_microphone_init_pio();

    // DMA Setup
    _dma_channel = dma_claim_unused_channel(true);
    dma_channel_config c = dma_channel_get_default_config(_dma_channel);
    channel_config_set_transfer_data_size(&c, DMA_SIZE_32);
    channel_config_set_read_increment(&c, false); // Read from PIO FIFO (fixed addr)
    channel_config_set_write_increment(&c, true); // Write to buffer
    channel_config_set_dreq(&c, pio_get_dreq(_config.pio, _config.pio_sm, false));

    dma_channel_configure(
        _dma_channel,
        &c,
        _raw_buffer,
        &_config.pio->rxf[_config.pio_sm],
        _config.sample_buffer_size * 2, // Count
        false // Don't start yet
    );

    // Enable IRQ
    dma_channel_set_irq0_enabled(_dma_channel, true);
    irq_set_exclusive_handler(DMA_IRQ_0, dma_handler);
    irq_set_enabled(DMA_IRQ_0, true);

    return 0;
}

void pdm_microphone_start(void) {
    dma_channel_start(_dma_channel);
    pio_sm_set_enabled(_config.pio, _config.pio_sm, true);
}

void pdm_microphone_set_samples_ready_handler(void (*handler)(void)) {
    _samples_ready_handler = handler;
}

int pdm_microphone_read(int16_t* buffer, size_t samples) {
    // Convert 32-bit I2S Stereo to 16-bit Mono (or Stereo)
    // INMP441 data is high-aligned in the 32-bit word.
    // L channel is at even indices, R at odd.
    // Let's just take Left channel for now.
    
    for (size_t i = 0; i < samples; i++) {
        int32_t raw_val = _raw_buffer[i * 2]; // Left channel
        // Shift down to 16 bit. 
        // Data is usually 24-bit left aligned in 32-bit frame.
        // So bits [31:8] are valid.
        // We want int16, so we take top 16 bits: [31:16].
        // Simple right shift by 16.
        buffer[i] = (int16_t)(raw_val >> 16); 
    }
    return samples;
}
