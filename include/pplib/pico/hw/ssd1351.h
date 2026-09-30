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

#ifndef PPLIB_PICO_HW__SSD1351_H
#define PPLIB_PICO_HW__SSD1351_H

#pragma once

#include "hardware/dma.h"
#include "hardware/spi.h"
#include "pico/binary_info.h"
#include "pico/stdlib.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <pplib/grafix/drawable.h>

namespace pplib::pico::hw
{
typedef enum ssd_commands
{
    SSD_CMD_SETCOLUMN = 0x15,
    SSD_CMD_WRITERAM = 0x5C,
    SSD_CMD_READRAM = 0x5D,
    SSD_CMD_SETROW = 0x75,
    SSD_CMD_SCROLL_SETUP = 0x96,
    SSD_CMD_SCROLL_START = 0x9E,
    SSD_CMD_SCROLL_STOP = 0x9F,
    SSD_CMD_SETREMAP = 0xA0,
    SSD_CMD_STARTLINE = 0xA1,
    SSD_CMD_DISPLAYOFFSET = 0xA2,
    SSD_CMD_PIXELSOFF = 0xA4, // All off
    SSD_CMD_PIXELSON = 0xA5,  // All on
    SSD_CMD_NONINVERT = 0xA6,
    SSD_CMD_INVERT = 0xA7,
    SSD_CMD_FUNCTIONSEL = 0xAB,
    SSD_CMD_DISPLAYOFF = 0xAE,
    SSD_CMD_DISPLAYON = 0xAF,
    SSD_CMD_PRECHARGE = 0xB1,
    SSD_CMD_ENHANCE = 0xB2,
    SSD_CMD_CLOCKDIV = 0xB3,
    SSD_CMD_SETVSL = 0xB4,
    SSD_CMD_SETGPIO = 0xB5,
    SSD_CMD_PRECHARGE2 = 0xB6,
    SSD_CMD_GAMMA_TABLE = 0xB8,
    SSD_CMD_RESET_GAMMA = 0xB9,
    SSD_CMD_PRECHARGEV = 0xBB,
    SSD_CMD_VCOMH = 0xBE,
    SSD_CMD_CONTRASTABC = 0xC1,
    SSD_CMD_CONTRASTMASTER = 0xC7,
    SSD_CMD_MUXRATIO = 0xCA,
    SSD_CMD_COMMANDLOCK = 0xFD
} ssd_commands;

class SSD1351
{
public:
    class Config
    {
    public:
        uint8_t pin_spi_dc;
        uint8_t pin_spi_rst;
        uint8_t pin_spi_sck;
        uint8_t pin_spi_data;
        uint32_t pin_spi_speed;
        spi_inst_t* spi_num;
    };

private:
    uint8_t* oled_dma[2];
    uint8_t current_buffer;
    size_t buffer_size;
    uint16_t width;
    uint16_t height;
    uint8_t spi_dc;
    uint8_t spi_rst;
    uint8_t spi_sck;
    uint8_t spi_data;
    uint32_t spi_speed;
    spi_inst_t* spi_num;

    volatile unsigned int dma_tx;
    dma_channel_config config;

    void write(const uint8_t cmd, const uint8_t* data, size_t len);
    void flush_dma(uint8_t* ptr, size_t len);
    void oled_init();

public:
    SSD1351();
    ~SSD1351();

    void init(int width, int height, const Config& config, bool useDoubleBuffer = false);

    uint8_t* get_buffer();

    void refresh();
    void sync();
    pplib::grafix::Drawable getDrawable();
    void clear(uint16_t color = 0x0000);
};

} // namespace pplib::pico::hw

#endif // PPLIB_PICO_HW__SSD1351_H