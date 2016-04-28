/**
 * MidiLight_Looper - Midi looper application
 *
 * @file    MidiLight_Looper.ino
 * @author  S.Tudoret
 * @date    20/04/16
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

#define LED_PORT         (PORTB) // B (digital pin 8 to 13)
#define LED_13           (13)    // LED pin on Arduino Uno
#define SWITCH_LED_13()  (LED_PORT ^= 1 << 5) // Faster than using digitalWrite

#define REC_EVT_TYPE_EOT      (0) // end of track
#define REC_EVT_TYPE_NOTE_ON  (1)
#define REC_EVT_TYPE_NOTE_OFF (2)

#define REC_STATE_IDLE        (0x00)
#define REC_STATE_RECORDING   (0x01)
#define REC_STATE_PLAYING     (0x02)

#define IS_RECORDING(idx) (tracks[idx].state & REC_STATE_RECORDING)
#define RECORD(idx)       (tracks[idx].state = REC_STATE_RECORDING)
#define IS_PLAYING(idx)   (tracks[idx].state & REC_STATE_PLAYING)
#define PLAY(idx)         (tracks[idx].state = REC_STATE_PLAYING)

#define REC_EVT_MAX           (100) // Max number of events by record
#define REC_MAX               (3)   // Max number of records

// C1 is the lower note on my keyboard
#define KEY_C1       (36)  // reserved to switch RECORD/PLAYBACK on track 1
#define KEY_C1_SHARP (37)  // reserved to switch RECORD/PLAYBACK on track 2
#define KEY_D1       (38)  // reserved to switch RECORD/PLAYBACK on track 3
#define LAST_RESERVED_KEY KEY_D1 

static unsigned long evtDelay = 0;
static unsigned long currentTime = 0;

typedef struct s_recEvt {
    byte type;
    byte pitch;
    byte velocity;
    byte delay; // unity of 64ms, so 255 means 16,3 seconds
} t_recEvt;

typedef struct s_track{
    t_recEvt events[REC_EVT_MAX + 1];
    unsigned long lastEvtTime; // used to manage delay of events
    byte key; // the key used to switch RECORD/PLAYBACK
    byte playbackChannel;
    byte eIdx;
    byte state;
} t_track;

static t_track tracks[REC_MAX];
static t_recEvt * evtPtr;

static void
handleNoteOn(
        byte channel,
        byte pitch,
        byte velocity)
{
    byte tIdx = 0;
    for (tIdx = 0; tIdx < REC_MAX; tIdx++) {
        if (IS_RECORDING(tIdx) && (tracks[tIdx].eIdx >= REC_EVT_MAX)) {
                // abort the record by simulate 
                pitch = tracks[tIdx].key;
            }
        if (pitch == tracks[tIdx].key) {
            // If is recording then stop recording and trigger a play
            // If is playing playback then stop playing and trigger a record
            if (IS_RECORDING(tIdx)) {
                evtPtr = &tracks[tIdx].events[tracks[tIdx].eIdx];
                PLAY(tIdx);
                SWITCH_LED_13();
                evtPtr->type = REC_EVT_TYPE_EOT;
                if (tracks[tIdx].eIdx == 0) {
                    evtPtr->delay = 0;
                } else {
                    evtPtr->delay =
                            (byte)((currentTime - tracks[tIdx].lastEvtTime) >> 5);
                }
            } else {
                RECORD(tIdx);
                SWITCH_LED_13();
            }
            tracks[tIdx].eIdx = 0;
        } else {
            // if recording then add the event in the event table
            if ((pitch > LAST_RESERVED_KEY) && (IS_RECORDING(tIdx))) {
                evtPtr = &tracks[tIdx].events[tracks[tIdx].eIdx];
                if (tracks[tIdx].eIdx == 0) {
                    evtPtr->delay = 0;
                } else {
                    evtPtr->delay =
                            (byte)((currentTime - tracks[tIdx].lastEvtTime) >> 5);
                }

                tracks[tIdx].lastEvtTime = currentTime;
                evtPtr->type = REC_EVT_TYPE_NOTE_ON;
                evtPtr->pitch = pitch;
                evtPtr->velocity = velocity;

                // Shift the end of track event
                tracks[tIdx].eIdx++;
                evtPtr++;
                evtPtr->type = REC_EVT_TYPE_EOT;
                evtPtr->delay = 0;
            }
        }
    }
}

static void
handleNoteOff(
        byte channel,
        byte pitch,
        byte velocity)
{
    // Do nothing on reserved key.
    // for the rest, if recording then add the event in the event table
    byte tIdx = 0;
    for (tIdx = 0; tIdx < REC_MAX; tIdx++) {
        if ((pitch > LAST_RESERVED_KEY) && IS_RECORDING(tIdx)){
            evtPtr = &tracks[tIdx].events[tracks[tIdx].eIdx];
            if (tracks[tIdx].eIdx == 0) {
                evtPtr->delay = 0;
            } else {
                evtPtr->delay =
                        (byte)((currentTime - tracks[tIdx].lastEvtTime) >> 5);
            }

            tracks[tIdx].lastEvtTime = currentTime;

            evtPtr->type = REC_EVT_TYPE_NOTE_OFF;
            evtPtr->pitch = pitch;
            evtPtr->velocity = velocity;

            // Shift the end of track event
            tracks[tIdx].eIdx++;
            evtPtr++;
            evtPtr->type = REC_EVT_TYPE_EOT;
            evtPtr->delay = 0;
        }
    }
}

void
setup()
{
    byte tIdx = 0;
    pinMode(LED_13, OUTPUT);

    midiLight_setup();

    midiLight_setCallbacks(
            handleNoteOn,
            handleNoteOff);

    // config tracks
    tracks[0].key = KEY_C1;
    tracks[0].playbackChannel = 1;
    tracks[1].key = KEY_C1_SHARP;
    tracks[1].playbackChannel = 2;
    tracks[2].key = KEY_D1;
    tracks[2].playbackChannel = 3;
    for (tIdx = 0; tIdx < REC_MAX; tIdx++) {
        tracks[tIdx].eIdx = 0;
        tracks[tIdx].state = REC_STATE_IDLE;
        tracks[tIdx].events[0].delay = 0;
        tracks[tIdx].events[0].type = REC_EVT_TYPE_EOT;
    }

}


void
loop()
{
    byte tIdx = 0;
    currentTime = millis();
    midiLight_read();

    for (tIdx = 0; tIdx < REC_MAX; tIdx++) {
        if (IS_PLAYING(tIdx)) {
            evtPtr = &tracks[tIdx].events[tracks[tIdx].eIdx];
            // convert the event delay in ms
            if (tracks[tIdx].eIdx > 0) {
                evtDelay = (unsigned long)evtPtr->delay;
                evtDelay <<= 5;
            }

            // check if the current event has to be played
            if ((tracks[tIdx].eIdx == 0) || (currentTime > tracks[tIdx].lastEvtTime + evtDelay)){
                tracks[tIdx].lastEvtTime = currentTime;

                if (evtPtr->type == REC_EVT_TYPE_NOTE_ON) {
                    midiLight_writeNoteOn(
                            tracks[tIdx].playbackChannel,
                            evtPtr->pitch,
                            evtPtr->velocity);
                } else if (evtPtr->type == REC_EVT_TYPE_NOTE_OFF) {
                    midiLight_writeNoteOff(
                            tracks[tIdx].playbackChannel,
                            evtPtr->pitch,
                            evtPtr->velocity);
                }

                // Update the track index
                if (evtPtr->type == REC_EVT_TYPE_EOT) {
                    tracks[tIdx].eIdx = 0;
                } else {
                    tracks[tIdx].eIdx++;
                }
            }

        }
    }

}
