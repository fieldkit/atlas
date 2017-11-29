#ifndef LEDS_H_INCLUDED
#define  LEDS_H_INCLUDED

#include <Arduino.h>

class FkLeds {
public:
    void setup() {
        pinMode(13, OUTPUT);
        pinMode(A3, OUTPUT);
        pinMode(A4, OUTPUT);
        pinMode(A5, OUTPUT);
        on();
    }

    void all(bool value) {
        digitalWrite(13, value);
        digitalWrite(A3, value);
        digitalWrite(A4, value);
        digitalWrite(A5, value);
    }

    void on() {
        all(HIGH);
    }

    void off() {
        all(LOW);
    }
};

#endif
