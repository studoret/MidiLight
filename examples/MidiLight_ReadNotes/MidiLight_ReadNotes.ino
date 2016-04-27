/**
 * MidiLight_ReadNotes - Small example of use of MidiLight library
 *
 * @file    MidiLight_ReadNotes.ino
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

#include <MidiLight.h>

#define LED_PORT         (PORTB) // B (digital pin 8 to 13)
#define LED_13           (13)    // LED pin on Arduino Uno
#define SWITCH_LED_13()  (LED_PORT ^= 1 << 5) // Faster than using digitalWrite

static void
handleNoteOn(
        byte channel,
        byte pitch,
        byte velocity)
{
    SWITCH_LED_13();
}

static void
handleNoteOff(
        byte channel,
        byte pitch,
        byte velocity)
{
    SWITCH_LED_13();
}

/****************************************/

void
setup()
{
    pinMode(LED_13, OUTPUT);

    midiLight_setup();

    midiLight_setCallbacks(
            handleNoteOn,
            handleNoteOff);

}

void
loop()
{
    midiLight_read();
}
