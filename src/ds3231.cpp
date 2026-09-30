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

#include "pico/stdlib.h"
#include "stdlib.h"
#include <string.h>
#include "stdio.h"
#include "pico/types.h"
#include "pico/time.h"
#include "hardware/i2c.h"
#include "pico/util/datetime.h"
// #include "pico/aon_timer.h"
#include "pico/aon_timer.h"
#include "pplib/pico/hw/ds3231.h"

namespace pplib::pico::hw
{

// convert BCD to number
static inline uint8_t bcd2num(uint8_t bcd)
{
    return ((bcd / 16) * 10) + (bcd % 16);
}

// convert number to BCD
static inline uint8_t num2bcd(uint8_t num)
{
    return ((num / 10) * 16) + (num % 10);
}

DS3231::DS3231()
{
    i2c_port = i2c0;
    i2c_scl = 0;
    i2c_sda = 0;
    addr = 0x68;
    // i2c_write_blocking(I2C_PORT, addr, buf, 2, false);
}

DS3231::DS3231(i2c_inst_t* i2c_port, int i2c_scl, int i2c_sda, int addr)
{
    this->i2c_port = i2c_port;
    this->i2c_scl = i2c_scl;
    this->i2c_sda = i2c_sda;
    this->addr = addr;
    init(i2c_port, i2c_scl, i2c_sda, addr);
}

void DS3231::init(i2c_inst_t* i2c_port, int i2c_scl, int i2c_sda, int addr)
{
    this->i2c_port = i2c_port;
    this->i2c_scl = i2c_scl;
    this->i2c_sda = i2c_sda;
    this->addr = addr;

    i2c_init(i2c_port, 40000);
    gpio_set_function(i2c_sda, GPIO_FUNC_I2C);
    gpio_set_function(i2c_scl, GPIO_FUNC_I2C);
    gpio_pull_up(i2c_sda);
    gpio_pull_up(i2c_scl);
}

void DS3231::setTime(const struct tm& time)
{
    uint8_t buf[2];
    struct tm old_time;
    getTime(old_time);
    // set second
    if (old_time.tm_sec != time.tm_sec) {
        buf[0] = 0x00;
        buf[1] = num2bcd(time.tm_sec);
        i2c_write_blocking(i2c_port, addr, buf, 2, false);
    }
    // set minute
    if (old_time.tm_min != time.tm_min) {
        buf[0] = 0x01;
        buf[1] = num2bcd(time.tm_min);
        i2c_write_blocking(i2c_port, addr, buf, 2, false);
    }
    // set hour
    if (old_time.tm_hour != time.tm_hour) {
        buf[0] = 0x02;
        buf[1] = num2bcd(time.tm_hour);
        i2c_write_blocking(i2c_port, addr, buf, 2, false);
    }
    // set weekday
    if (old_time.tm_wday != time.tm_wday) {
        buf[0] = 0x03;
        buf[1] = num2bcd((time.tm_wday == 0 ? 7 : time.tm_wday));
        i2c_write_blocking(i2c_port, addr, buf, 2, false);
    }
    // set day
    if (old_time.tm_mday != time.tm_mday) {
        buf[0] = 0x04;
        buf[1] = num2bcd(time.tm_mday);
        i2c_write_blocking(i2c_port, addr, buf, 2, false);
    }
    // set month
    if (old_time.tm_mon != time.tm_mon) {
        buf[0] = 0x05;
        buf[1] = num2bcd(time.tm_mon + 1);
        i2c_write_blocking(i2c_port, addr, buf, 2, false);
    }
    // set year (adjust for years since 2000)
    if (old_time.tm_year != time.tm_year) {
        buf[0] = 0x06;
        buf[1] = num2bcd(time.tm_year % 100);
        i2c_write_blocking(i2c_port, addr, buf, 2, false);
    }
}

void DS3231::setYear(int16_t year)
{
    uint8_t buf[2];
    buf[0] = 0x06;
    buf[1] = num2bcd(year % 100);
    i2c_write_blocking(i2c_port, addr, buf, 2, false);
}

void DS3231::setMonth(int8_t month)
{
    uint8_t buf[2];
    buf[0] = 0x05;
    buf[1] = num2bcd(month);
    i2c_write_blocking(i2c_port, addr, buf, 2, false);
}

void DS3231::setDay(int8_t day)
{
    uint8_t buf[2];
    buf[0] = 0x04;
    buf[1] = num2bcd(day);
    i2c_write_blocking(i2c_port, addr, buf, 2, false);
}

void DS3231::setWeekday(int8_t dotw)
{
    uint8_t buf[2];
    buf[0] = 0x03;
    buf[1] = num2bcd((dotw == 0 ? 7 : dotw));
    i2c_write_blocking(i2c_port, addr, buf, 2, false);
}

void DS3231::setHour(int8_t hour)
{
    uint8_t buf[2];
    buf[0] = 0x02;
    buf[1] = num2bcd(hour);
    i2c_write_blocking(i2c_port, addr, buf, 2, false);
}

void DS3231::setMinute(int8_t min)
{
    uint8_t buf[2];
    buf[0] = 0x01;
    buf[1] = num2bcd(min);
    i2c_write_blocking(i2c_port, addr, buf, 2, false);
}

void DS3231::setSecond(int8_t sec)
{
    uint8_t buf[2];
    buf[0] = 0x00;
    buf[1] = num2bcd(sec);
    i2c_write_blocking(i2c_port, addr, buf, 2, false);
}

void DS3231::getTime(struct tm& time) const
{
    uint8_t buf[8];
    memset(buf, 0, 8);
    memset(&time, 0, sizeof(struct tm));
    time.tm_isdst = -1; // Not set by DS3231, so mark as unknown

    uint8_t val = 0x00;
    if (1 != i2c_write_blocking(i2c_port, addr, &val, 1, true)) {
        printf("ERROR: i2c_write_blocking failed\n");
    }
    int bytes = i2c_read_blocking(i2c_port, addr, buf, 7, false);
    time.tm_sec = bcd2num(buf[0]);
    time.tm_min = bcd2num(buf[1]);
    time.tm_hour = bcd2num(buf[2]);
    time.tm_wday = bcd2num(buf[3]) % 7; // Convert 1-7 to 0-6
    time.tm_mday = bcd2num(buf[4]);
    time.tm_mon = bcd2num(buf[5]) - 1;
    time.tm_year = bcd2num(buf[6]) + 100;
}

datetime_t DS3231::getTime() const
{
    struct tm time;
    getTime(time);
    datetime_t dt;
    dt.year = time.tm_year + 1900;
    dt.month = time.tm_mon + 1;
    dt.day = time.tm_mday;
    dt.dotw = time.tm_wday;
    dt.hour = time.tm_hour;
    dt.min = time.tm_min;
    dt.sec = time.tm_sec;
    return dt;
}

void DS3231::setTime(const datetime_t& time)
{
    struct tm tm_time;
    tm_time.tm_year = time.year - 1900;
    tm_time.tm_mon = time.month - 1;
    tm_time.tm_mday = time.day;
    tm_time.tm_wday = time.dotw;
    tm_time.tm_hour = time.hour;
    tm_time.tm_min = time.min;
    tm_time.tm_sec = time.sec;
    setTime(tm_time);
}

float DS3231::getTemperature() const
{
    uint8_t val = 0x11;
    if (1 != i2c_write_blocking(i2c_port, addr, &val, 1, true)) {
        printf("ERROR: i2c_write_blocking failed\n");
        return 0.0f;
    }
    uint8_t buf[] = {0, 0};
    if (i2c_read_blocking(i2c_port, addr, buf, 2, false) != 2) {
        printf("ERROR: i2c_read_blocking failed\n");
        return 0.0f;
    }
    int16_t v1 = (buf[0] << 8) | buf[1];
    int16_t v2 = v1 / 64;
    return (float)v2 * 0.25f;
}

void DS3231::syncTimeToSystem()
{
    struct tm time;
    getTime(time);
    aon_timer_start_calendar(&time);
}

} // namespace pplib::pico::hw