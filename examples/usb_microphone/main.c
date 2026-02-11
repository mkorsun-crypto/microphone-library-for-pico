#include "pico/pdm_microphone.h" // Используем "поддельный" PDM
#include "usb_microphone.h"

// configuration
const struct pdm_microphone_config config = {
  .gpio_data = 12, // SD
  .gpio_clk = 10,  // SCK (BCLK)
  .gpio_ws = 11,   // WS (LRCLK) -> ДОБАВИЛИ ЭТУ СТРОКУ
  .pio = pio0,
  .pio_sm = 0,
  .sample_rate = SAMPLE_RATE,
  .sample_buffer_size = SAMPLE_BUFFER_SIZE,
};

int16_t sample_buffer[SAMPLE_BUFFER_SIZE];

void on_pdm_samples_ready();
void on_usb_microphone_tx_ready();

int main(void)
{
  pdm_microphone_init(&config);
  pdm_microphone_set_samples_ready_handler(on_pdm_samples_ready);
  pdm_microphone_start();

  usb_microphone_init();
  usb_microphone_set_tx_ready_handler(on_usb_microphone_tx_ready);

  while (1) {
    usb_microphone_task();
  }
  return 0;
}

void on_pdm_samples_ready()
{
  // Читаем данные через наш подменный драйвер
  pdm_microphone_read(sample_buffer, SAMPLE_BUFFER_SIZE);
}

void on_usb_microphone_tx_ready()
{
  usb_microphone_write(sample_buffer, sizeof(sample_buffer));
}
