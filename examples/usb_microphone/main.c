#include <string.h> // Добавлено для memcpy
#include "pdm_microphone.h" 
#include "usb_microphone.h"
#include "pico/stdlib.h" // Нужно для определения типов и SAMPLE_RATE если он там

// Если SAMPLE_RATE не определен в других .h файлах, определим его здесь
#ifndef SAMPLE_RATE
#define SAMPLE_RATE 16000
#endif

#ifndef SAMPLE_BUFFER_SIZE
#define SAMPLE_BUFFER_SIZE 256
#endif

// configuration
const struct pdm_microphone_config config = {
    .gpio_data = 12, 
    .gpio_clk = 10,  
    // .gpio_ws = 11,  <-- УДАЛЕНО: этого поля нет в вашей библиотеке PDM
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
    stdio_init_all(); // Полезно для отладки

    pdm_microphone_init(&config);
    pdm_microphone_set_samples_ready_handler(on_pdm_samples_ready);
    
    // Теперь start возвращает int, но в main его можно просто вызвать
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
    // ИСПРАВЛЕНО: Ваша функция не принимает аргументов, а возвращает указатель
    int16_t* raw_data = pdm_microphone_read();
    
    // Копируем данные в локальный буфер, если он нужен
    if (raw_data) {
        memcpy(sample_buffer, raw_data, SAMPLE_BUFFER_SIZE * sizeof(int16_t));
    }
}

void on_usb_microphone_tx_ready()
{
    usb_microphone_write(sample_buffer, sizeof(sample_buffer));
}
