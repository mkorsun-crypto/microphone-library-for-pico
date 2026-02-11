/*
 * Copyright (c) 2021 Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 * * MODIFIED FOR I2S MICROPHONE (INMP441)
 * PINS: BCLK=10, LRCLK=11, DATA=12
 */

// ВАЖНО: Подключаем библиотеку I2S, а не PDM
#include "pico/i2s_microphone.h"
#include "usb_microphone.h"

// configuration
// Используем структуру для I2S
const struct i2s_microphone_config config = {
  .gpio_data = 12,   // Твой пин SD
  .gpio_bclk = 10,   // Твой пин SCK
  .gpio_lrclk = 11,  // Твой пин WS
  .pio = pio0,
  .pio_sm = 0,
  .sample_rate = SAMPLE_RATE,
  //.sample_buffer_size = SAMPLE_BUFFER_SIZE, // В некоторых версиях либы это не нужно, но если ошибка - раскомментируй
};

// variables
int16_t sample_buffer[SAMPLE_BUFFER_SIZE]; // I2S обычно отдает int16_t (или int32_t), а не uint16_t

// callback functions
void on_i2s_samples_ready();
void on_usb_microphone_tx_ready();

int main(void)
{
  // initialize and start the I2S microphone
  // Меняем все функции с pdm_ на i2s_
  i2s_microphone_init(&config);
  i2s_microphone_set_samples_ready_handler(on_i2s_samples_ready);
  i2s_microphone_start();

  // initialize the USB microphone interface
  usb_microphone_init();
  usb_microphone_set_tx_ready_handler(on_usb_microphone_tx_ready);

  while (1) {
    // run the USB microphone task continuously
    usb_microphone_task();
  }

  return 0;
}

void on_i2s_samples_ready()
{
  // Callback from library when all the samples in the library
  // internal sample buffer are ready for reading.
  //
  // Read new samples into local buffer.
  i2s_microphone_read(sample_buffer, SAMPLE_BUFFER_SIZE);
}

void on_usb_microphone_tx_ready()
{
  // Callback from TinyUSB library when all data is ready
  // to be transmitted.
  //
  // Write local buffer to the USB microphone
  usb_microphone_write(sample_buffer, sizeof(sample_buffer));
}
