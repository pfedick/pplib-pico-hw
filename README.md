# pplib-pico-hw
Hierbei handelt es sich um eine Hardware-Bibliothek für den Raspberry Pico, die teilweise auf PPLIB Version 8 aufsetzt (siehe https://github.com/pfedick/pplib/tree/pplib8).

Sie enthält folgende Module:
- ST7789 Display Treiber (farbiges TFT)
- SSD1306 Display Treiber (Monochromes OLED)
- SSD1351 Display Treiber (farbiges OLED)
- Rotary Encoder Steuerung
- WS2812 Steuerung von programmierbaren RGB-LEDs


## Einbinding im CMake-File:

```cmake
# PPLIB Configuration
set(PPLIB_ENABLE_PICO ON CACHE BOOL "Build Pico module in PPLIB" FORCE)
add_subdirectory(path/to/pplib)

# PPLIB-PICO-HW Configuration
add_subdirectory(path/to/pplib-pico-hw)
target_link_libraries(your_target PRIVATE pplib-pico-hw)
```
