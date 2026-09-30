/*******************************************************************************
 * This file is part of "Patrick's Programming Library" for Raspberry Pico,
 * based on PPLib Version 8.
 * Web: https://github.com/pfedick/pplib-pico-hw
 *******************************************************************************
 * Copyright (c) 2026, Patrick Fedick <patrick@pfp.de>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    1. Redistributions of source code must retain the above copyright notice, this
 *       list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright notice,
 *       this list of conditions and the following disclaimer in the documentation
 *       and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER AND CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 *******************************************************************************/

#ifndef PPLIB_PICO_HW_ST7789_H
#define PPLIB_PICO_HW_ST7789_H
#pragma once

#include "hardware/dma.h"
#include "hardware/spi.h"
#include "pico/binary_info.h"
#include "pico/stdlib.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "pplib/grafix/drawable.h"
#include "pplib/grafix/color.h"

namespace pplib::pico::hw
{
/**@class ST7789
 * @brief Class to control the ST7789 TFT display controller.
 *
 * This class provides an interface to initialize and control the ST7789 TFT display controller.
 * It supports various configurations, including SPI communication, orientation settings, and
 * brightness control. The class also provides methods for drawing graphics and text on the display.
 */
class ST7789
{
public:
    static constexpr uint8_t UNUSED_PIN = 255;
    enum class SPIMode : uint8_t
    {
        Mode0 = 0, // CPOL=0, CPHA=0
        Mode1 = 1, // CPOL=0, CPHA=1
        Mode2 = 2, // CPOL=1, CPHA=0
        Mode3 = 3  // CPOL=1, CPHA=1
    };
    class Config
    {
    public:
        spi_inst_t* spi_num;
        uint32_t spi_speed = 20000000; // Default 20 MHz
        uint8_t pin_spi_dc;
        uint8_t pin_spi_cs;
        uint8_t pin_spi_rst;
        uint8_t pin_spi_sck;
        uint8_t pin_spi_data;
        uint8_t pin_spi_blk;               // can be UNUSED_PIN if not used
        SPIMode spi_mode = SPIMode::Mode0; // Default SPI mode
    };

    enum class Orientation : uint8_t
    {
        Portrait,         // 0°
        Landscape,        // 90° im Uhrzeigersinn, Default
        InvertedPortrait, // 180°
        InvertedLandscape // 270° im Uhrzeigersinn
    };

private:
    uint8_t* oled_dma[2];
    spi_inst_t* spi_num;
    volatile unsigned int dma_tx;
    uint16_t my_width;
    uint16_t my_height;
    uint8_t current_buffer;
    uint8_t spi_dc;
    uint8_t spi_cs;
    uint8_t spi_blk;
    SPIMode spi_mode;

    // dma_channel_config config; // kann weg nach init
    Orientation orientation;

    void write(const uint8_t cmd, const uint8_t* data, size_t len);
    void flush_dma(uint8_t* ptr, size_t len);
    void init_tft(const Config& config);
    inline size_t get_buffer_size() const
    {
        return my_width * my_height * 2;
    }
    void init_pwm();

public:
    ST7789();
    ~ST7789();

    void init(uint16_t width, uint16_t height, const Config& config, bool useDoubleBuffer = false);
    void setBrightness(uint8_t brightness);

    /**@brief Set the VCOM voltage for the display.
     * @param vcom The VCOM voltage value (0x00 to 0x3F).
     * @note The VCOM voltage range is from 0x00 (0.1V) to 0x3F (1.675V).
     */
    void setVCom(uint8_t vcom); // Wert von 0x00 (0.1V) bis 0x3F (1.675V)
    void setOrientation(Orientation o);
    uint8_t* get_buffer() const;
    inline uint16_t width()
    {
        return my_width;
    };
    inline uint16_t height()
    {
        return my_height;
    };

    void refresh();
    void sync();
    pplib::grafix::Drawable getDrawable();
    void clear(pplib::grafix::Color color = pplib::grafix::Color(0, 0, 0));
};

} // namespace pplib::pico::hw

#endif // PPLIB_PICO_HW_ST7789_H