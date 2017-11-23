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
    }

    void on() {
        digitalWrite(13, HIGH);
        digitalWrite(A3, HIGH);
        digitalWrite(A4, HIGH);
        digitalWrite(A5, HIGH);
    }
};

#endif
