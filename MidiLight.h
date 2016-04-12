/**
 * MidiLight - Light Midi library for Arduino
 *
 * @file   MidiLight.h
 * @author S.Tudoret
 * @date   29/02/16
 *
 * Copyright 2016 S. Tudoret.
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
midiLight_setCallbacks(
        t_midiLight_cb cbOn,
        t_midiLight_cb cbOff);

#endif /* __MIDILIGTH_H_ */
