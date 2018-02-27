#include "TestHelper.h"
#include "ESPLed.h"
#include "ESPEventChain.h"



#ifdef __PWM_GPIO_H__

    bool pwmRangeTest1() {

        TestHelper test("pwmRange()", "basic update checks");

        uint32_t inputs[] = {256, 512, 1023, 255};
        for(uint32_t i : inputs) {

            analogWriteRange(i);
            const uint32_t ACTUAL = ESPPwmGpio::pwmRange();
            const uint32_t EXPECTED = i;
            test.printResult(EXPECTED, ACTUAL);

        }

        return test.printResult();
    }

#else

    bool pwmRangeTest() {
        return true;
    }

#endif









#ifdef __ESP_BRIGHTNESS_RANGE_H__

bool mapToAnalogTests() {
    TestHelper test("mapToAnalog()", "fixed values");
   
    const uint16_t ANALOG_RANGE = 1023;
    const uint8_t PERCENT_STEPS = 101;
    analogWriteRange(ANALOG_RANGE);

    int inputs[] = {0, 25, 50, 75, 100};
    for(int i : inputs) {

        const uint16_t expected = ANALOG_RANGE * ( log(i) / log(PERCENT_STEPS) );
        const uint16_t actual = ESPLedBrightness::percentToAnalog(i);

        test.printResult(expected, actual);
    }

    return test.printResult();
}

#else

    bool mapToAnalogTests() { return true; }

#endif












#ifdef __PWM_GPIO_H__
bool gpioInitTest() {

    TestHelper test("ESPPwmGpio()", "constructor test");
    const uint8_t PIN = 2;


    ESPPwmGpio gpio(2);
    const bool STARTED_LOW = digitalRead(PIN) == LOW ? true : false;

    digitalWrite(PIN, HIGH);
    const bool SET_PINMODE = digitalRead(PIN) == HIGH ? true : false;

    const uint8_t GET_PIN = gpio.pin();

    Serial.printf("Pin in LOW state upon init:\n");
    test.printResult(true, STARTED_LOW);

    Serial.printf("Pin in output mode upon init:\n");
    test.printResult(true, SET_PINMODE);

    Serial.printf("Pin getter test:");
    test.printResult(PIN, gpio.pin());
    
    return test.printResult();

}
#else
    bool gpioInitTest() { return true; }
#endif













#ifdef __ESP_BRIGHTNESS_RANGE_H__
bool brightnessTests() {
    TestHelper test("ESPLedBrightness","various fixed tests");
   
    ESPLedBrightness brightness;

    uint8_t test_values[][2] = {
        {50, 75},
        {0, 100},
        {10, 30},
        {80, 96},
        {0,1},
    };
    const uint8_t NUM_VALUES = sizeof(test_values) / sizeof(test_values[0]);

    for(int test_case = 0; test_case < NUM_VALUES; test_case++) {
        const uint8_t MIN = test_values[test_case][0];
        const uint8_t MAX = test_values[test_case][1];
        brightness.maxBrightnessPercent(MAX);
        brightness.minBrightnessPercent(MIN);
        test.printResult(MIN, brightness.minBrightnessPercent());
        test.printResult(MAX, brightness.maxBrightnessPercent());
    }

    return test.printResult();
}
#else
    bool brightnessTests() { return true; }
#endif







// #ifdef __ESPLED_H__
// bool ledTest1() {
    
//     bool all_passed = true;
//     const uint8_t PIN = 2;
//     Serial.printf("ESPLed tests - MAKE SURE GPIO%i is connected to A0!\n", PIN);
//     analogWriteRange(1023);

//     ESPLed reg(PIN);

//     const uint8_t GET_PIN = reg.pin();
//     Serial.printf("Pin getter test: Expected %i, actual %i\n", PIN, GET_PIN);
//     if(PIN != GET_PIN){
//         PRINT_FAIL;
//         all_passed = false;
//     }

//     uint8_t analog_values[] = {0, 100, 50};
//     const size_t TESTS = sizeof(analog_values) / sizeof(analog_values[0]);
//     Serial.println("Analog write value tests:");
//     for(int i = 0; i < TESTS; i++) {    
//         reg.on(analog_values[i]);
//         const uint16_t EXPECTED = ESPLedBrightness::percentToAnalog(analog_values[i]);
//         const uint16_t ACTUAL = 0;
        
//         Serial.printf("Expected %i, actual %i\n", EXPECTED, ACTUAL);
//         if( abs(EXPECTED - ACTUAL) > 20 ) {
//             PRINT_FAIL;
//             all_passed = false;
//         }
//     }

//     reg.off();
//     Serial.printf("isOn() test - Expected false, actual %s\n", reg.isOn() ? "true" : "false");
//     if( reg.isOn() != false ) {
//         PRINT_FAIL;
//         all_passed = false;
//     }

//     reg.on();
//     Serial.printf("isOn() test - Expected true, actual %s\n", reg.isOn() ? "true" : "false");
//     if( reg.isOn() != true ) {
//         PRINT_FAIL;
//         all_passed = false;
//     }





    
//     return all_passed;

// }
// #else
// bool ledTest1() { return true; }
// #endif


#ifdef __ESP_EVENT_CHAIN_H__


bool eventChainTest1() {
    TestHelper test("EspEventChain","getters");

    unsigned long t1 = 200;
    unsigned long t2 = 75;

    EspEvent e1(t1, [&]() {
    });
    EspEvent e2(t2, [&]() {
    });
    EspEventChain chain(e1, e2);

    test.printResult(2, (int)chain.numEvents());
    test.printResult(t1+t2, chain.totalTime());
    test.printResult(t1, chain.totalTimeBefore(1));

    return test.printResult();

}


bool eventChainTest2() {
    TestHelper test("EspEventChain", "Ticking properly");
    
    unsigned long t1 = 100, t2 = 75, wait = (t1 + t2)*3;
    unsigned long count = 0;
    
    unsigned long lastEvent;
    unsigned long elapsedTime;


    EspEvent e1(t1, [&]() {
        unsigned long current_time = millis();
        unsigned long elapsed_time = current_time - lastEvent;
        test.printResultRange(count != 0 ? t1 : 0,  elapsed_time, 2);
        count++;
        lastEvent = current_time;
    });

    EspEvent e2(t2, [&]() {
        unsigned long current_time = millis();
        unsigned long elapsed_time = current_time - lastEvent;
        test.printResultRange(t2, elapsed_time, 2);
        count++;
        lastEvent = current_time;
    });
    EspEventChain chain(e1, e2);

    Serial.printf("Tick1: %i, Tick2: %i\n", t1, t2);


    lastEvent = millis();
    chain.start();
    delay( wait + 10 );
    chain.stop();

    unsigned long expected_ticks = 2 * wait / (t1+t2) + 1;
    test.printResult(expected_ticks, count); 
    test.printResult();
}

bool eventChainTest3() {
    TestHelper test("ESPEventChain()","events stored by copy, not reference");
    
    bool ticked = false;
    unsigned long wait = 30;
    EspEvent *e1 = new EspEvent(30, []() {
        bool ticked = true;
        Serial.println("Tick from deleted object, copy was successful!");
    });

    EspEventChain chain(*e1);
    delete e1;
    chain.start();
    delay(2*wait);

    Serial.println("If we made it this far without an exception, copy was made");
    test.printResult(true, ticked);
    test.printResult();

}
#else
bool eventChainTest1() { return true; }
bool eventChainTest2() { return true; }
bool eventChainTest3() { return true; }
#endif




#ifdef __ESPLED_H__
#ifdef __ESP_EVENT_CHAIN_H__

bool blinkTest1() {
    TestHelper test("ESPBlink()", "visual blink test");

    ESPLed led(2, HIGH);
    ESPBlink blink;
    led.setMode(blink);

    Serial.println("Basic blink test");
    led.start();
    delay(8000);
    
    Serial.println("Test changing parameters on the fly");
    blink.interval(100);
    blink.duration(50);

    delay(8000);
    blink.stopAll();

}

#endif
#else
bool blinkTest1() { return true; }

#endif











void setup() {
    Serial.begin(115200);
    Serial.println("\n\n");
    Serial.println("Beginning tests\n");
    delay(1000);

    pwmRangeTest1();
    mapToAnalogTests();
    gpioInitTest();
    // ledTest1();
    brightnessTests();
    eventChainTest1();
    eventChainTest2();
    eventChainTest3();

    blinkTest1();

    TestHelper::end();
}


void loop() {
    delay(10);

}