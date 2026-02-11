cmake_minimum_required(VERSION 3.12)

# Объявляем исполняемый файл и список исходных кодов
add_executable(usb_microphone
    main.c
    usb_descriptors.c
    usb_microphone.c
)

# Указываем, где искать заголовочные файлы (.h)
target_include_directories(usb_microphone PRIVATE ${CMAKE_CURRENT_LIST_DIR})

# Подключаем библиотеки.
# ВАЖНО: Оставляем pico_pdm_microphone. 
# Система будет думать, что это PDM, но внутри будет работать наш новый I2S код.
target_link_libraries(usb_microphone PRIVATE 
    tinyusb_device 
    tinyusb_board 
    pico_pdm_microphone
)

# Команда для создания файла прошивки .uf2
pico_add_extra_outputs(usb_microphone)
