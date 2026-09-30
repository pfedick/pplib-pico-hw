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

#include "pplib/pico/hw/ssd1351.h"

#include <malloc.h>
#include <string.h>

namespace pplib::pico::hw
{

typedef enum write_type
{
    WRITE_COMMAND = 0,
    WRITE_DATA = 1
} write_type;

SSD1351::SSD1351()
{
    width = 0;
    height = 0;
    buffer_size = 0;
    oled_dma[0] = nullptr;
    oled_dma[1] = nullptr;
    current_buffer = 0;
    spi_dc = -1;
    spi_rst = -1;
    spi_sck = -1;
    spi_data = -1;
    spi_speed = 20000000;
    spi_num = nullptr;
    dma_tx = 0;
}

SSD1351::~SSD1351()
{
    free(oled_dma[0]);
    free(oled_dma[1]);
}

void SSD1351::init(int width, int height, const Config& config, bool useDoubleBuffer)
{
    free(oled_dma[0]);
    free(oled_dma[1]);

    this->width = width;
    this->height = height;
    buffer_size = width * height * 2;
    oled_dma[0] = (uint8_t*)malloc(buffer_size);
    if (!oled_dma[0]) return;
    memset(oled_dma[0], 0, buffer_size);
    if (useDoubleBuffer) {
        oled_dma[1] = (uint8_t*)malloc(buffer_size);
        if (!oled_dma[1]) return;
        memset(oled_dma[1], 0, buffer_size);
    }
    current_buffer = 0;

    spi_dc = config.pin_spi_dc;
    spi_rst = config.pin_spi_rst;
    spi_rst = config.pin_spi_rst;
    spi_sck = config.pin_spi_sck;
    spi_data = config.pin_spi_data;
    spi_speed = config.pin_spi_speed;
    spi_num = config.spi_num;
    oled_init();
}

void SSD1351::write(const uint8_t cmd, const uint8_t* data, size_t len)
{
    gpio_put(spi_dc, WRITE_COMMAND);
    spi_write_blocking(spi_num, &cmd, 1);
    if (len) {
        gpio_put(spi_dc, WRITE_DATA);
        spi_write_blocking(spi_num, data, len);
    }
}

void SSD1351::flush_dma(uint8_t* ptr, size_t len)
{
    dma_channel_wait_for_finish_blocking(dma_tx);
    sleep_ms(1);
    write(SSD_CMD_WRITERAM, NULL, 0);
    gpio_put(spi_dc, WRITE_DATA);
    dma_channel_transfer_from_buffer_now(dma_tx, ptr, len);
}

void SSD1351::sync()
{
    dma_channel_wait_for_finish_blocking(dma_tx);
}

uint8_t* SSD1351::get_buffer()
{
    return oled_dma[current_buffer];
}

pplib::grafix::Drawable SSD1351::getDrawable()
{
    return pplib::grafix::Drawable(oled_dma[current_buffer], 256, 128, 128, pplib::grafix::RGBFormat::R5G6B5);
}

void SSD1351::oled_init()
{
    uint8_t param[3];
    // Setup SPI
    spi_init(spi_num, spi_speed);
    spi_set_format(spi_num, 8, SPI_CPOL_1, SPI_CPHA_1, SPI_MSB_FIRST);
    gpio_set_function(spi_sck, GPIO_FUNC_SPI);
    gpio_set_function(spi_data, GPIO_FUNC_SPI);

    // Setup GPIO
    gpio_init(spi_dc);
    gpio_set_dir(spi_dc, GPIO_OUT);
    gpio_put(spi_dc, 1);

    gpio_init(spi_rst);
    gpio_set_dir(spi_rst, GPIO_OUT);

    // Power on reset
    gpio_put(spi_rst, 1);
    sleep_ms(10);
    gpio_put(spi_rst, 0);
    sleep_ms(500);
    gpio_put(spi_rst, 1);
    gpio_put(spi_dc, 0);

    // Init display

    // Unlock driver
    param[0] = 0x12;
    write(SSD_CMD_COMMANDLOCK, param, 1);

    // Unlock commands
    param[0] = 0xB1;
    write(SSD_CMD_COMMANDLOCK, param, 1);

    write(SSD_CMD_DISPLAYOFF, NULL, 0);

    // Max frequency, no divider. Fastest refresh rate.
    // param[0] = 0xF0;
    param[0] = 0xff;
    write(SSD_CMD_CLOCKDIV, param, 1);

    // Effectively number of lines
    param[0] = 0x7F;
    write(SSD_CMD_MUXRATIO, param, 1);

    // Horizontal addressing, unmirrored, C->B->A colours, normal scan, 65K colours
    param[0] = 0x74;
    write(SSD_CMD_SETREMAP, param, 1);

    param[0] = 0x00;
    write(SSD_CMD_STARTLINE, param, 1);

    param[0] = 0x00;
    write(SSD_CMD_DISPLAYOFFSET, param, 1);

    // Disable GPIO
    param[0] = 0x00;
    write(SSD_CMD_SETGPIO, param, 1);

    // 16bit parallel interface
    param[0] = 0x01;
    write(SSD_CMD_FUNCTIONSEL, param, 1);

    write(SSD_CMD_NONINVERT, NULL, 0);

    // R G B contrast
    param[0] = param[1] = param[2] = 0xff;
    write(SSD_CMD_CONTRASTABC, param, 3);

    // Max master contrast
    param[0] = 0x08;
    write(SSD_CMD_CONTRASTMASTER, param, 1);

    param[0] = 0xA4;
    param[1] = 0x00;
    param[2] = 0x00;
    write(SSD_CMD_ENHANCE, param, 3);

    write(SSD_CMD_DISPLAYON, NULL, 0);

    sleep_ms(10);

    // Setup DMA transfers
    dma_tx = dma_claim_unused_channel(true);
    config = dma_channel_get_default_config(dma_tx);
    channel_config_set_transfer_data_size(&config, DMA_SIZE_8);
    channel_config_set_dreq(&config, spi_get_index(spi_num) ? DREQ_SPI1_TX : DREQ_SPI0_TX);
    dma_channel_configure(dma_tx, &config, &spi_get_hw(spi_num)->dr, oled_dma[0], buffer_size, false);
}

void SSD1351::clear(uint16_t color)
{
    uint16_t* buffer = (uint16_t*)get_buffer();
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            buffer[y * width + x] = color;
        }
    }
}

void SSD1351::refresh()
{
    if (oled_dma[0] != nullptr && oled_dma[1] != nullptr) {
        if (current_buffer == 0) {
            flush_dma(oled_dma[0], buffer_size);
            current_buffer = 1;
        } else {
            flush_dma(oled_dma[1], buffer_size);
            current_buffer = 0;
        }
    } else {
        flush_dma(oled_dma[0], buffer_size);
        current_buffer = 0;
    }
}

} // namespace pplib::pico::hw