
#ifndef __ESP_LED_TEST__
#define __ESP_LED_TEST__

#include <Arduino.h>
#include <cmath>
#include "ESPLed.h"

#define __ESP_TEST_PINI__       2
#define __ESP_TEST_PIN__        0

#ifndef PWMRANGE
#define PWMRANGE                1023
#endif


class EspLedTester : private ESPLed {

    public:


        bool percentToAnalogTest(uint8_t percent);

        bool pinTest();

        bool isOnTest();

        bool onTest(uint8_t percent);

        bool maxBrightnessTest(uint8_t percent);

        bool minBrightnessTest(uint8_t percent);

        /**
         * @brief Tests behavior when user attempts to set min brightness higher
         * than max brightness
         * 
         */
        bool badMaxMinBrightnessTest();
    

    private:

        ESPLed led;
        ESPLed ledI;

        EspLedTester()
        :
        led( ESPLed(__ESP_TEST_PIN__) ),
        ledI( ESPLed(__ESP_TEST_PINI__, HIGH) )
        {

        }

        
        constexpr double antilog(unsigned int x) {
            (double) 1023 * ( 1 - log(101, x) );
        }

        template<typename T>
        constexpr double log(unsigned long base, T value) {
            return log(val)/log(base);
        }

};

#endif