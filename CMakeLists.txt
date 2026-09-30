cmake_minimum_required(VERSION 3.20)
project(pplib-pico-hw C CXX ASM)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PICO_HW_SOURCES
    src/ds3231.cpp
    src/lightstrip.cpp
    src/oled_ssd1306.cpp
    src/rotary_encoder.cpp
    src/ssd1351.cpp
    src/st7789.cpp
)

add_library(pplib_pico_hw STATIC ${PICO_HW_SOURCES})
add_library(pplib::pico::hw ALIAS pplib_pico_hw)

# PIO-Header für WS2812 direkt in dieser Library erzeugen
pico_generate_pio_header(pplib_pico_hw ${CMAKE_CURRENT_SOURCE_DIR}/src/ws2812.pio)

target_include_directories(pplib_pico_hw PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}> # Enthält generiertes ws2812.pio.h
)

target_compile_definitions(pplib_pico_hw PUBLIC
    PICO_BUILD=1
    _GNU_SOURCE=1
    _DEFAULT_SOURCE=1
)

target_link_libraries(pplib_pico_hw PUBLIC
    pplib_pico
    pico_stdlib
    pico_aon_timer
    hardware_spi
    hardware_i2c
    hardware_pio
    hardware_dma
    hardware_pwm
    hardware_clocks
)