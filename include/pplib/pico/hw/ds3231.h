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
#ifndef PPLIB_PICO_HW_DS3231_H
#define PPLIB_PICO_HW_DS3231_H

#include "pico/time.h"
#include "pico/stdlib.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include "pico/types.h"
#include "hardware/i2c.h"
#include "hardware/timer.h"
// #include "pico/aon_timer.h"

namespace pplib::pico::hw
{
typedef struct
{
    int16_t year; // 0..4095
    int8_t month; // 1..12 (1 = Januar)
    int8_t day;   // 1..31 (je nach Monat)
    int8_t dotw;  // 0..6  (0 = Sonntag)
    int8_t hour;  // 0..23
    int8_t min;   // 0..59
    int8_t sec;   // 0..59
} datetime_t;

class DS3231
{
private:
    i2c_inst_t* i2c_port;
    int i2c_scl;
    int i2c_sda;
    int addr;

public:
    DS3231();
    DS3231(i2c_inst_t* i2c_port, int i2c_scl, int i2c_sda, int addr = 0x68);
    void init(i2c_inst_t* i2c_port, int i2c_scl, int i2c_sda, int addr = 0x68);

    datetime_t getTime() const;
    void setTime(const datetime_t& time);
    void setTime(const struct tm& time);
    void setYear(int16_t year);
    void setMonth(int8_t month);
    void setDay(int8_t day);
    void setWeekday(int8_t dotw);
    void setHour(int8_t hour);
    void setMinute(int8_t min);
    void setSecond(int8_t sec);

    void getTime(struct tm& time) const;
    void syncTimeToSystem();
    float getTemperature() const;
};

} // namespace pplib::pico::hw

#endif // PPLIB_PICO_HW_DS3231_H