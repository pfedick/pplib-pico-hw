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

#ifndef PPLIB_PICO_HW__LIGHTSTRIP_H
#define PPLIB_PICO_HW__LIGHTSTRIP_H

#include <stdint.h>
#include <stddef.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"

#include "pplib/grafix/color.h"
#include <vector>

namespace pplib::pico::hw
{

class LightStrip;

class LightStripSection
{
    friend class LightStrip;

public:
    enum class Direction
    {
        Forward = 0,
        Backward = 1
    };

private:
    size_t start;
    size_t end;
    size_t my_size;
    Direction dir;
    LightStrip* ls;

public:
    LightStripSection();
    LightStripSection(const LightStripSection& other);
    LightStripSection(LightStripSection&& other);
    LightStripSection(LightStrip* ls, size_t start, size_t end, Direction dir = Direction::Forward);

    size_t size() const;
    void clear(const pplib::grafix::Color& color = pplib::grafix::Color(0, 0, 0));
    void putPixel(int p, const pplib::grafix::Color& color);
    pplib::grafix::Color getPixel(int p) const;
    void shift(Direction d, int count = 1, bool rotate = false);
    void write();
    LightStripSection& operator=(const LightStripSection& other);
    LightStripSection& operator=(LightStripSection&& other);
};

class LightStrip
{
private:
    std::vector<pplib::grafix::Color> pixel;
    PIO pio;
    uint sm = 0;
    uint offset = 0;
    uint16_t num;
    bool is_rgbw;
    uint8_t brightness = 255;

    constexpr inline uint32_t toWS2812(const pplib::grafix::Color& c) const
    {
        uint8_t r = (c.red() * brightness) >> 8;
        uint8_t g = (c.green() * brightness) >> 8;
        uint8_t b = (c.blue() * brightness) >> 8;
        return (g << 16) | (r << 8) | b;
    }
    constexpr inline uint32_t toSK6812(const pplib::grafix::Color& c) const
    {
        uint8_t r = (c.red() * brightness) >> 8;
        uint8_t g = (c.green() * brightness) >> 8;
        uint8_t b = (c.blue() * brightness) >> 8;
        // Alpha-Kanal könnte hier als "White"-Kanal genutzt werden, falls gewünscht
        uint8_t w = (c.alpha() * brightness) >> 8;
        return (g << 24) | (r << 16) | (b << 8) | w;
    }

public:
    enum class Direction
    {
        Forward = 0,
        Backward = 1
    };
    LightStrip(int gpio, size_t num_pixel, float frequency = 800000.0f, bool is_rgbw = false);
    ~LightStrip();
    size_t size() const;
    PIO getPIO() const
    {
        return pio;
    }
    uint getSM() const
    {
        return sm;
    }
    uint getOffset() const
    {
        return offset;
    }
    void setSize(int num_pixel);

    void clear(const pplib::grafix::Color& color = pplib::grafix::Color(0, 0, 0));
    void write();

    void setBrightness(uint8_t b)
    {
        brightness = b;
    }
    uint8_t getBrightness() const
    {
        return brightness;
    }

    inline void putPixel(int p, const pplib::grafix::Color& color)
    {

        if (p < num && p >= 0) pixel[p] = color;
    }

    inline pplib::grafix::Color getPixel(int p) const
    {

        if (p < num && p >= 0) return pixel[p];
        return pplib::grafix::Color();
    }

    void shift(Direction d, int count = 1, bool rotate = false);

    LightStripSection getSection(size_t start, size_t end, LightStripSection::Direction dir = LightStripSection::Direction::Forward);

    void playIntro();
};

} // namespace pplib::pico::hw

#endif // PPLIB_PICO_HW_LIGHTSTRIP_H