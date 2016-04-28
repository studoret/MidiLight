# MidiLight
Light Midi library for Arduino

## Features
-    Implement the Midi Running Status algorithm from http://www.blitter.com/~russtopia/MIDI/~jglatt/tech/midispec/run.htm
-    Use hardware serial pins of Arduino uno
-    Allow to listen and sends Note On/Off Event messages
-    The Midilight_Looper example allows to record and play three loops
-    Tested with Yamaha digital keyboard PSP-E323

## Todo
-    Manage input channels (at this time only channel 0 is taken in account)
-    Manage Program Change messages
-    Improve the Midilight_Looper example with program change feature

## Schematic


        R1 = R2 = R3 = R5 = 220ohm
        R4 = 1Kohm


        Tx  ------------------------------------R1----- Midi_out_5
        GND -------+----------------------------------- Midi_out_gnd
                   |
        +5  -+-----]----------------------------R2----- Midi_out_4
             |     |
             |     |        _____
             |     +-----.5|     |4.
             |     |       |  6  |
        Rx -----+--]-----.6|  N  |3.-------+----------- Midi_in_5
             |  |  |       |  1  |         |
             |  R3 +-R4--.7|  3  |2.--+   _|_
             |  |          |  6  |    |  _\_/_ 1N4148
             +--+--------.8|____.|1.  |    |
                                      +----+----R5----- Midi_in_4



