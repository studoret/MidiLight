// Preprocessor bug prevent - insert me on top of your arduino-code
// From: http://www.a-control.de/arduino-fehler/?lang=en
#if 1
__asm volatile ("nop");
#endif

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
