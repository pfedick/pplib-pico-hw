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

#ifndef PPLIB_PICO_HW_ROTARY_ENCODER_H
#define PPLIB_PICO_HW_ROTARY_ENCODER_H

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <atomic>

#include "pico/types.h"

namespace pplib::pico::hw
{

class RotaryEncoder
{
public:
    enum Event : uint8_t
    {
        EVENT_NONE = 0,
        EVENT_CW,
        EVENT_CCW,
        EVENT_BUTTON_DOWN,
        EVENT_BUTTON_UP
    };

private:
    uint8_t clk_pin, dt_pin, switch_pin;
    std::atomic<int> encoder_position{0};
    std::atomic<bool> button_pressed{false};
    std::atomic<uint32_t> last_button_time{0};
    std::atomic<uint8_t> r_state{0};
    bool is_initialized;

    static const uint8_t QUEUE_SIZE = 32;
    std::atomic<Event> event_queue[QUEUE_SIZE];
    std::atomic<uint8_t> queue_head = 0;
    std::atomic<uint8_t> queue_tail = 0;

    void push_event(Event event);

public:
    RotaryEncoder();
    ~RotaryEncoder();
    void init(uint8_t clk_pin, uint8_t dt_pin, uint8_t switch_pin);
    void handle_interrupt(uint gpio, uint32_t events);

    int position() const
    {
        return encoder_position;
    }
    bool buttonPressed() const
    {
        return button_pressed;
    }
    void setPosition(int pos);

    bool hasEvent() const
    {
        return queue_head != queue_tail;
    }
    Event popEvent();
    void check_button(); // Muss regelmäßig aufgerufen werden, um den Tasterzustand zu aktualisieren (Debounce)
};

} // namespace pplib::pico::hw

#endif // PPLIB_PICO_HW_ROTARY_ENCODER_H