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

#include <cstddef>
#include <malloc.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "pplib/pico/hw/st7789.h"
namespace pplib::pico::hw
{
typedef enum write_type
{
    WRITE_COMMAND = 0,
    WRITE_DATA = 1
} write_type;

typedef enum st7789_commands
{
    // System Function Commands
    CMD_NOP = 0x00,       // No Operation
    CMD_SWRESET = 0x01,   // Software Reset
    CMD_RDDID = 0x04,     // Read Display ID
    CMD_RDDST = 0x09,     // Read Display Status
    CMD_RDDPM = 0x0A,     // Read Display Power Mode
    CMD_RDDMADCTL = 0x0B, // Read Display MADCTL
    CMD_RDDCOLMOD = 0x0C, // Read Display Pixel Format
    CMD_RDDIM = 0x0D,     // Read Display Image Mode
    CMD_RDDSM = 0x0E,     // Read Display Signal Mode
    CMD_RDDSDR = 0x0F,    // Read Display Self-Diagnostic Result

    // Sleep Mode Commands
    CMD_SLPIN = 0x10,  // Sleep In (enter sleep mode)
    CMD_SLPOUT = 0x11, // Sleep Out (exit sleep mode)
    CMD_PTLON = 0x12,  // Partial Display Mode On
    CMD_NORON = 0x13,  // Normal Display Mode On

    // Display Inversion Commands
    CMD_INVOFF = 0x20, // Display Inversion Off
    CMD_INVON = 0x21,  // Display Inversion On

    // Display On/Off Commands
    CMD_DISPOFF = 0x28, // Display Off
    CMD_DISPON = 0x29,  // Display On

    // Address Setting Commands
    CMD_CASET = 0x2A, // Column Address Set
    CMD_RASET = 0x2B, // Row Address Set
    CMD_RAMWR = 0x2C, // Memory Write
    CMD_RAMRD = 0x2E, // Memory Read

    // Partial Area Commands
    CMD_PTLAR = 0x30,   // Partial Area (define partial display area)
    CMD_VSCRDEF = 0x33, // Vertical Scrolling Definition
    CMD_TEOFF = 0x34,   // Tearing Effect Line OFF
    CMD_TEON = 0x35,    // Tearing Effect Line ON

    // Memory Access Control
    CMD_MADCTL = 0x36,   // Memory Data Access Control (rotation/mirroring)
    CMD_VSCRSADD = 0x37, // Vertical Scrolling Start Address
    CMD_IDMOFF = 0x38,   // Idle Mode Off
    CMD_IDMON = 0x39,    // Idle Mode On

    // Pixel Format
    CMD_COLMOD = 0x3A, // Interface Pixel Format (color depth: 12/16/18 bit)
    CMD_RAMWRC = 0x3C, // Memory Write Continue
    CMD_RAMRDC = 0x3E, // Memory Read Continue

    // Tearing Effect
    CMD_TESCAN = 0x44,   // Set Tear Scanline
    CMD_RDTESCAN = 0x45, // Get Scanline

    // Brightness Control
    CMD_WRDISBV = 0x51,  // Write Display Brightness
    CMD_RDDISBV = 0x52,  // Read Display Brightness Value
    CMD_WRCTRLD = 0x53,  // Write CTRL Display
    CMD_RDCTRLD = 0x54,  // Read CTRL Value Display
    CMD_WRCACE = 0x55,   // Write Content Adaptive Brightness Control and Color Enhancement
    CMD_RDCABC = 0x56,   // Read Content Adaptive Brightness Control
    CMD_WRCABCMB = 0x5E, // Write CABC Minimum Brightness
    CMD_RDCABCMB = 0x5F, // Read CABC Minimum Brightness

    // RGB Interface Control
    CMD_RAMCTRL = 0xB0, // RAM Control
    CMD_RGBCTRL = 0xB1, // RGB Interface Control
    CMD_PORCTRL = 0xB2, // Porch Setting (front/back porch control)
    CMD_FRCTRL1 = 0xB3, // Frame Rate Control 1 (in partial mode/idle colors)

    // Gate Control
    CMD_GCTRL = 0xB7, // Gate Control
    CMD_GTADJ = 0xB8, // Gate On Timing Adjustment
    CMD_DGMEN = 0xBA, // Digital Gamma Enable

    // VCOM Setting
    CMD_VCOMS = 0xBB, // VCOM Setting

    // Power Control
    CMD_LCMCTRL = 0xC0,  // LCM Control
    CMD_IDSET = 0xC1,    // ID Code Setting
    CMD_VDVVRHEN = 0xC2, // VDV and VRH Command Enable
    CMD_VRHS = 0xC3,     // VRH Set (voltage setting)
    CMD_VDVS = 0xC4,     // VDV Set (voltage setting)
    CMD_VCMOFSET = 0xC5, // VCOM Offset Set
    CMD_FRCTRL2 = 0xC6,  // Frame Rate Control in Normal Mode
    CMD_CABCCTRL = 0xC7, // CABC Control
    CMD_REGSEL1 = 0xC8,  // Register Value Selection 1
    CMD_REGSEL2 = 0xCA,  // Register Value Selection 2
    CMD_PWMFRSEL = 0xCC, // PWM Frequency Selection

    // Power Control
    CMD_PWCTRL1 = 0xD0,  // Power Control 1
    CMD_VAPVANEN = 0xD2, // Enable VAP/VAN signal output
    CMD_RDID1 = 0xDA,    // Read ID1
    CMD_RDID2 = 0xDB,    // Read ID2
    CMD_RDID3 = 0xDC,    // Read ID3

    // Gamma Control
    CMD_PVGAMCTRL = 0xE0, // Positive Voltage Gamma Control
    CMD_NVGAMCTRL = 0xE1, // Negative Voltage Gamma Control
    CMD_DGMLUTR = 0xE2,   // Digital Gamma Look-up Table for Red
    CMD_DGMLUTB = 0xE3,   // Digital Gamma Look-up Table for Blue

    // Other Commands
    CMD_GATECTRL = 0xE4, // Gate Control
    CMD_SPI2EN = 0xE7,   // SPI2 Enable
    CMD_PWCTRL2 = 0xE8,  // Power Control 2
    CMD_EQCTRL = 0xE9,   // Equalize time control
    CMD_PROMCTRL = 0xEC, // Program Mode Control
    CMD_PROMEN = 0xFA,   // Program Mode Enable
    CMD_NVMSET = 0xFC,   // NVM Setting
    CMD_PROMACT = 0xFE   // Program action

} st7789_commands;

ST7789::ST7789()
{
    my_width = 0;
    my_height = 0;
    oled_dma[0] = nullptr;
    oled_dma[1] = nullptr;
    current_buffer = 0;
    spi_dc = -1;
    spi_cs = -1;
    spi_blk = -1;
    spi_num = nullptr;
    dma_tx = 0;
    orientation = Orientation::Landscape;
}

ST7789::~ST7789()
{
    free(oled_dma[0]);
    free(oled_dma[1]);
}

void ST7789::init(uint16_t width, uint16_t height, const Config& config, bool useDoubleBuffer)
{
    free(oled_dma[0]);
    free(oled_dma[1]);

    this->my_width = width;
    this->my_height = height;
    size_t buffer_size = get_buffer_size();
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
    spi_cs = config.pin_spi_cs;
    spi_blk = config.pin_spi_blk;
    spi_num = config.spi_num;
    spi_mode = config.spi_mode;
    init_tft(config);
    init_pwm();
}

void ST7789::init_pwm()
{
    if (spi_blk != UNUSED_PIN) {
        gpio_set_function(spi_blk, GPIO_FUNC_PWM);
        uint slice_num = pwm_gpio_to_slice_num(spi_blk);
        pwm_config cfg = pwm_get_default_config();
        pwm_config_set_clkdiv(&cfg, 4.0f); // Teiler für weichere PWM
        pwm_init(slice_num, &cfg, true);
        pwm_set_wrap(slice_num, 255); // 0-255 Range
    }
}

void ST7789::write(const uint8_t cmd, const uint8_t* data, size_t len)
{
    gpio_put(spi_cs, 0); // CS aktivieren (LOW)
    gpio_put(spi_dc, WRITE_COMMAND);
    spi_write_blocking(spi_num, &cmd, 1);
    if (len) {
        gpio_put(spi_dc, WRITE_DATA);
        spi_write_blocking(spi_num, data, len);
    }
    gpio_put(spi_cs, 1); // CS deaktivieren (HIGH)
}

void ST7789::init_tft(const Config& config)
{
    uint8_t param[14]; // Größeres Array für längere Parameterlisten

    spi_init(spi_num, config.spi_speed);
    switch (spi_mode) {
    case SPIMode::Mode0:
        spi_set_format(spi_num, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
        break;
    case SPIMode::Mode1:
        spi_set_format(spi_num, 8, SPI_CPOL_0, SPI_CPHA_1, SPI_MSB_FIRST);
        break;
    case SPIMode::Mode2:
        spi_set_format(spi_num, 8, SPI_CPOL_1, SPI_CPHA_0, SPI_MSB_FIRST);
        break;
    case SPIMode::Mode3:
        spi_set_format(spi_num, 8, SPI_CPOL_1, SPI_CPHA_1, SPI_MSB_FIRST);
        break;
    }
    gpio_set_function(config.pin_spi_sck, GPIO_FUNC_SPI);
    gpio_set_function(config.pin_spi_data, GPIO_FUNC_SPI);

    gpio_init(spi_cs);
    gpio_set_dir(spi_cs, GPIO_OUT);
    gpio_put(spi_cs, 0); // CS aktiv (LOW)

    // Setup GPIO Pins
    gpio_init(spi_dc);
    gpio_set_dir(spi_dc, GPIO_OUT);
    gpio_put(spi_dc, 1);

    if (config.pin_spi_rst != UNUSED_PIN) {
        gpio_init(config.pin_spi_rst);
        gpio_set_dir(config.pin_spi_rst, GPIO_OUT);

        // Hardware Reset
        gpio_put(config.pin_spi_rst, 1);
        sleep_ms(10);
        gpio_put(config.pin_spi_rst, 0);
        sleep_ms(10);
        gpio_put(config.pin_spi_rst, 1);
        sleep_ms(120); // Warten bis Display bereit ist
    }

    // Software Reset
    write(CMD_SWRESET, NULL, 0);
    sleep_ms(150); // WICHTIG: Nach Software-Reset warten!

    // Sleep Out (Display aufwecken)
    write(CMD_SLPOUT, NULL, 0);
    sleep_ms(120); // WICHTIG: Nach SLPOUT warten!

    // Interface Pixel Format: 16-bit/pixel (RGB565)
    param[0] = 0x55; // 0x55 = 16-bit RGB565
    write(CMD_COLMOD, param, 1);

    // Memory Data Access Control (Display-Orientierung)
    // MY=0, MX=0, MV=0, ML=0, RGB=1, MH=0
    param[0] = 0x60; // 0x00 = Standard-Orientierung
    // Alternative Werte:
    // 0x00 = 0°   (Portrait)
    // 0x60 = 90°  (Landscape)
    // 0xC0 = 180° (Portrait, gespiegelt)
    // 0xA0 = 270° (Landscape, gespiegelt)
    write(CMD_MADCTL, param, 1);

    // Porch Setting (Front und Back Porch)
    param[0] = 0x0C; // Back porch in normal mode
    param[1] = 0x0C; // Front porch in normal mode
    param[2] = 0x00; // Disable separate porch control
    param[3] = 0x33; // Back porch in idle mode
    param[4] = 0x33; // Front porch in idle mode
    write(CMD_PORCTRL, param, 5);

    // Gate Control
    param[0] = 0x35; // Default value
    write(CMD_GCTRL, param, 1);

    // VCOM Setting
    param[0] = 0x19; // VCOM = 0.725V (typischer Wert)
    write(CMD_VCOMS, param, 1);

    // LCM Control
    param[0] = 0x2C;
    write(CMD_LCMCTRL, param, 1);

    // VDV and VRH Command Enable
    param[0] = 0x01; // Enable command 2
    write(CMD_VDVVRHEN, param, 1);

    // VRH Set (GVDD voltage setting)
    param[0] = 0x12; // 4.45V + (vcom+vcom offset+vdv)
    write(CMD_VRHS, param, 1);

    // VDV Set
    param[0] = 0x20; // 0V
    write(CMD_VDVS, param, 1);

    // Frame Rate Control in Normal Mode
    param[0] = 0x0F; // 60 Hz (standard frame rate)
    write(CMD_FRCTRL2, param, 1);

    // Power Control 1
    param[0] = 0xA4; // AVDD=6.8V, AVCL=-4.8V, VDDS=2.3V
    param[1] = 0xA1; // VGH=14.06V, VGL=-9.32V
    write(CMD_PWCTRL1, param, 2);

    // Positive Voltage Gamma Control
    param[0] = 0xD0;
    param[1] = 0x04;
    param[2] = 0x0D;
    param[3] = 0x11;
    param[4] = 0x13;
    param[5] = 0x2B;
    param[6] = 0x3F;
    param[7] = 0x54;
    param[8] = 0x4C;
    param[9] = 0x18;
    param[10] = 0x0D;
    param[11] = 0x0B;
    param[12] = 0x1F;
    param[13] = 0x23;
    write(CMD_PVGAMCTRL, param, 14);

    // Negative Voltage Gamma Control
    param[0] = 0xD0;
    param[1] = 0x04;
    param[2] = 0x0C;
    param[3] = 0x11;
    param[4] = 0x13;
    param[5] = 0x2C;
    param[6] = 0x3F;
    param[7] = 0x44;
    param[8] = 0x51;
    param[9] = 0x2F;
    param[10] = 0x1F;
    param[11] = 0x1F;
    param[12] = 0x20;
    param[13] = 0x23;
    write(CMD_NVGAMCTRL, param, 14);

    // Display Inversion On (oft für bessere Farben nötig)
    write(CMD_INVON, NULL, 0);

    // Normal Display Mode On
    write(CMD_NORON, NULL, 0);
    sleep_ms(10);

    // Display On
    write(CMD_DISPON, NULL, 0);
    sleep_ms(120);

    // Setup DMA transfers
    dma_tx = dma_claim_unused_channel(true);

    dma_channel_config dma_config = dma_channel_get_default_config(dma_tx);
    channel_config_set_transfer_data_size(&dma_config, DMA_SIZE_8);
    channel_config_set_dreq(&dma_config, spi_get_index(spi_num) ? DREQ_SPI1_TX : DREQ_SPI0_TX);
    dma_channel_configure(dma_tx, &dma_config, &spi_get_hw(spi_num)->dr, oled_dma[0], get_buffer_size(), false);
}

void ST7789::flush_dma(uint8_t* ptr, size_t len)
{
    dma_channel_wait_for_finish_blocking(dma_tx);
    gpio_put(spi_cs, 1); // CS deaktivieren nach Transfer
    sleep_us(10);        // Kurze Pause, um sicherzustellen, dass CS korrekt erkannt wird

    uint8_t param[4];

    gpio_put(spi_cs, 0); // CS aktivieren für gesamte Sequenz

    uint16_t caset_max, raset_max;
    uint16_t x_offset = 0, y_offset = 0;

    // ST7789 interner Framebuffer: 240 (Spalten) × 320 (Zeilen)
    const uint16_t ST7789_COLS = 240;
    const uint16_t ST7789_ROWS = 320;

    if (orientation == Orientation::Portrait || orientation == Orientation::InvertedPortrait) {
        caset_max = my_height - 1;
        raset_max = my_width - 1;
        // Portrait: CASET nutzt ST7789_COLS (240), RASET nutzt ST7789_ROWS (320)
        // Y-Offset = (320 - display_höhe) für zentriert oder am Ende
        if (orientation == Orientation::Portrait) {
            x_offset = 0;
            y_offset = 0;
        } else { // InvertedPortrait
            x_offset = 0;
            y_offset = (my_height < ST7789_ROWS) ? (ST7789_ROWS - my_height) : 0;
        }
    } else {
        caset_max = my_width - 1;
        raset_max = my_height - 1;
        // Landscape: Display nutzt Spalten 0-239, Zeilen 0-239
        if (orientation == Orientation::Landscape) {
            x_offset = 0;
            y_offset = 0;
        } else { // InvertedLandscape
            x_offset = (my_width < ST7789_ROWS) ? (ST7789_ROWS - my_width) : 0;
            y_offset = 0;
        }
    }

    // Column Address Set (CASET) - 0 bis 239
    param[0] = x_offset >> 8;
    param[1] = x_offset & 0xFF;
    param[2] = (x_offset + caset_max) >> 8;
    param[3] = (x_offset + caset_max) & 0xFF;
    gpio_put(spi_dc, WRITE_COMMAND);
    spi_write_blocking(spi_num, (const uint8_t[]){CMD_CASET}, 1);
    gpio_put(spi_dc, WRITE_DATA);
    spi_write_blocking(spi_num, param, 4);

    // Row Address Set (RASET) - 0 bis 239
    param[0] = y_offset >> 8;
    param[1] = y_offset & 0xFF;
    param[2] = (y_offset + raset_max) >> 8;
    param[3] = (y_offset + raset_max) & 0xFF;
    gpio_put(spi_dc, WRITE_COMMAND);
    spi_write_blocking(spi_num, (const uint8_t[]){CMD_RASET}, 1);
    gpio_put(spi_dc, WRITE_DATA);
    spi_write_blocking(spi_num, param, 4);

    // Memory Write
    gpio_put(spi_dc, WRITE_COMMAND);
    spi_write_blocking(spi_num, (const uint8_t[]){CMD_RAMWR}, 1);
    gpio_put(spi_dc, WRITE_DATA);

    // DMA Transfer
    dma_channel_transfer_from_buffer_now(dma_tx, ptr, len);
}

void ST7789::refresh()
{
    size_t buffer_size = get_buffer_size();
    if (oled_dma[0] != nullptr && oled_dma[1] != nullptr) {
        if (current_buffer == 0) {
            flush_dma(oled_dma[0], buffer_size);
            // memcpy(oled_dma[1], oled_dma[0], buffer_size);
            current_buffer = 1;
        } else {
            flush_dma(oled_dma[1], buffer_size);
            // memcpy(oled_dma[0], oled_dma[1], buffer_size);
            current_buffer = 0;
        }
    } else {
        flush_dma(oled_dma[0], buffer_size);
        current_buffer = 0;
    }
}

void ST7789::sync()
{
    dma_channel_wait_for_finish_blocking(dma_tx);
}

uint8_t* ST7789::get_buffer() const
{
    return oled_dma[current_buffer];
}

pplib::grafix::Drawable ST7789::getDrawable()
{
    if (orientation == Orientation::Portrait || orientation == Orientation::InvertedPortrait) {
        return pplib::grafix::Drawable(oled_dma[current_buffer], my_height * 2, my_height, my_width, pplib::grafix::RGBFormat::R5G6B5);
    }
    return pplib::grafix::Drawable(oled_dma[current_buffer], my_width * 2, my_width, my_height, pplib::grafix::RGBFormat::R5G6B5);
}

void ST7789::setOrientation(Orientation o)
{
    uint8_t param = 0x00;
    switch (o) {
    case Orientation::Portrait:
        param = 0x00; // MY=0, MX=0, MV=0
        break;
    case Orientation::Landscape:
        param = 0x60; // MY=0, MX=0, MV=1
        break;
    case Orientation::InvertedPortrait:
        param = 0xC0; // MY=1, MX=1, MV=0
        break;
    case Orientation::InvertedLandscape:
        param = 0xA0; // MY=1, MX=1, MV=1
        break;
    }
    write(CMD_MADCTL, &param, 1);
    orientation = o;
}

void ST7789::clear(pplib::grafix::Color color)
{
    uint16_t color565 = ((color.red() & 0xf8) << 8) | ((color.green() & 0xfc) << 3) | ((color.blue() & 0xf8) >> 3);
    if (color565 == 0 || color565 == 0xFFFF) {
        memset(get_buffer(), color565 & 0xff, get_buffer_size());
        return;
    }
    uint16_t nativeColor = (color565 >> 8) | (color565 << 8); // Byte swap for SPI transmission

    uint16_t* buffer = (uint16_t*)get_buffer();

    for (int y = 0; y < my_height; y++) {
        for (int x = 0; x < my_width; x++) {
            buffer[y * my_width + x] = nativeColor;
        }
    }
}

void ST7789::setBrightness(uint8_t brightness)
{
    if (spi_blk == UNUSED_PIN) return; // Kein PWM-Pin konfiguriert, daher keine Helligkeitsregelung möglich
    uint slice_num = pwm_gpio_to_slice_num(spi_blk);
    uint channel = pwm_gpio_to_channel(spi_blk);
    pwm_set_chan_level(slice_num, channel, brightness);
}

void ST7789::setVCom(uint8_t vcom)
{
    // VCOM-Wertebereich ist 0x00 (0.1V) bis 0x3F (1.675V)
    if (vcom > 0x3F) vcom = 0x3F;
    write(CMD_VCOMS, &vcom, 1);
}

} // namespace pplib::pico::hw