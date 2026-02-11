#ifndef PDM_MICROPHONE_H
#define PDM_MICROPHONE_H

#include "pico/stdlib.h"

#ifdef __cplusplus
extern "C" {
#endif

// Конфигурация микрофона
struct pdm_microphone_config {
    uint gpio_data;
    uint gpio_clk;
    PIO pio;
    uint pio_sm;
    uint sample_rate;
    uint sample_buffer_size;
};

// Инициализация (возвращает int для совместимости: 0 = OK)
int pdm_microphone_init(const struct pdm_microphone_config *config);

// Деинициализация
void pdm_microphone_deinit(void);

// Запуск (возвращает int для совместимости: 0 = OK)
int pdm_microphone_start(void);

// Остановка
void pdm_microphone_stop(void);

// Чтение данных (тип сallback'а)
typedef void (*pdm_samples_ready_handler_t)(void);
void pdm_microphone_set_samples_ready_handler(pdm_samples_ready_handler_t handler);

// Получение указателя на буфер с обработанными данными
int16_t* pdm_microphone_read(void);

// Управление фильтром (если нужно)
void pdm_microphone_set_filter_max_volume(uint8_t max_volume);
void pdm_microphone_set_filter_gain(uint8_t gain);

#ifdef __cplusplus
}
#endif

#endif
