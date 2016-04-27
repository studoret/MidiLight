/**
 * @file    MidiLight.cpp
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

#include "Arduino.h"
#include <MidiLight.h>

#define MIDI_STATUS_SYS_EX_START    (0xF0)
#define MIDI_STATUS_SYS_EX_STOP     (0xF0)
#define MIDI_STATUS_NOTE_OFF        (0x80)
#define MIDI_STATUS_NOTE_ON         (0x90)

#define MIDI_STATE_STATUS_WAITING   (0)
#define MIDI_STATE_NOTE_WAITING     (1)
#define MIDI_STATE_VELOCITY_WAITING (2)

#define MIDI_MAX_NOTE               (128)
#define MIDI_MAX_VELOCITY           (128)

#define MIDI_BAUDRATE               (31250)

static Stream * serial = NULL;
static byte incomingByte = 0;
static byte statusBuffer = 0;
static int  noteDown = LOW;
static byte note = 0;
static byte state = MIDI_STATE_STATUS_WAITING;  // state machine variable
static byte channel = 0;
static t_midiLight_cb cbNoteOn = NULL;
static t_midiLight_cb cbNoteOff = NULL;

void
midiLight_setup()
{
    serial = &Serial;
    Serial.begin(MIDI_BAUDRATE);
}

void
midiLight_setCallbacks(
        t_midiLight_cb cbOn,
        t_midiLight_cb cbOff)
{
    cbNoteOn  = cbOn;
    cbNoteOff = cbOff;
}

void
midiLight_read()
{
    if (serial->available() > 0) {

        incomingByte = serial->read();

        if ((incomingByte >= MIDI_STATUS_NOTE_OFF) && (incomingByte < MIDI_STATUS_SYS_EX_START)) {
            // TODO manage channel

            // Buffer stores the status when a Voice Category Status is received.
            statusBuffer = incomingByte;
            if (incomingByte == MIDI_STATUS_NOTE_ON) {
                noteDown = HIGH;
                state = MIDI_STATE_NOTE_WAITING;
            } else if (incomingByte == MIDI_STATUS_NOTE_OFF) {
                noteDown = LOW;
                state = MIDI_STATE_NOTE_WAITING;
            }
        } else if ((incomingByte >= MIDI_STATUS_SYS_EX_START) && (incomingByte <= MIDI_STATUS_SYS_EX_STOP)){
            // Buffer is cleared when a System Common Category Status is received.
            statusBuffer = 0;
        }

        if ((statusBuffer != 0) && (incomingByte < MIDI_STATUS_NOTE_OFF)) {
            // Any data bytes are ignored when the buffer is 0.

            if (state == MIDI_STATE_STATUS_WAITING) {
                // Runing status
                if (statusBuffer == MIDI_STATUS_NOTE_ON) {
                    noteDown = HIGH;
                    state = MIDI_STATE_NOTE_WAITING;
                } else if (statusBuffer == MIDI_STATUS_NOTE_OFF) {
                    noteDown = LOW;
                    state = MIDI_STATE_NOTE_WAITING;
                }
            }

            switch (state){

                case MIDI_STATE_NOTE_WAITING:
                    if(incomingByte < MIDI_MAX_NOTE) {
                        note = incomingByte;
                        state = MIDI_STATE_VELOCITY_WAITING;
                    }
                    else{
                        state = MIDI_STATE_STATUS_WAITING;  // reset state machine as this should be a note number
                    }
                    break;

                case MIDI_STATE_VELOCITY_WAITING:
                    // get the velocity
                    if(incomingByte < MIDI_MAX_VELOCITY) {
                        // if velocity = 0 on a 'Note ON' command, treat it as a note off
                        if (incomingByte == 0) {
                            noteDown = LOW;
                        }
                        if (noteDown == LOW) {
                            cbNoteOff(channel, note, incomingByte);
                        } else {
                            cbNoteOn(channel, note, incomingByte);
                        }
                    }
                    state = MIDI_STATE_STATUS_WAITING;  // reset state machine to start
                    break;
            }
        }
    }

out:
    return;
}

void
midiLight_writeNoteOn(
        byte channel,
        byte pitch,
        int velocity)
{
    serial->write(MIDI_STATUS_NOTE_ON | channel);
    serial->write(pitch);
    serial->write(velocity);
}

void
midiLight_writeNoteOff(
        byte channel,
        byte pitch,
        int velocity)
{
    serial->write(MIDI_STATUS_NOTE_OFF | channel);
    serial->write(pitch);
    serial->write(velocity);
}
