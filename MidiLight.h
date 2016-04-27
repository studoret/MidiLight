/**
 * MidiLight - Light Midi library for Arduino
 *
 * @file    MidiLight.h
 * @author  S.Tudoret
 * @date    29/02/16
 * @license MIT
 *
 * Copyright (c) 2016 S. Tudoret
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef __MIDILIGTH_H_
#define __MIDILIGTH_H_

/*
 Uncoment the following line to use software serial.
 */
#define SOFTWARE_SERIAL

#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

typedef void (*t_midiLight_cb) (byte channel, byte pitch, byte velocity);

void
midiLight_setup();

void
midiLight_read();

void
midiLight_writeNoteOn(
        byte channel,
        byte pitch,
        int velocity);

void
midiLight_writeNoteOff(
        byte channel,
        byte pitch,
        int velocity);

void
midiLight_setCallbacks(
        t_midiLight_cb cbOn,
        t_midiLight_cb cbOff);

#endif /* __MIDILIGTH_H_ */
