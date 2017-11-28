#include <Arduino.h>

namespace std {

    void __throw_bad_alloc() __attribute__ ((weak));
    void __throw_bad_function_call() __attribute__ ((weak));

    void __throw_bad_alloc() {
        Serial.println("Out of memory");
        while (true) {
            // Forever
        }
    }

    void __throw_bad_function_call() {
        Serial.println("Bad function call");
        while (true) {
            // Forever
        }
    }

}
