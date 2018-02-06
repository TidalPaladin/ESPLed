//#include "ESPLed.h"
#include "ESPEventChain.h"

#define FAIL_TXT    "***** FAILED *****"
#define PRINT_FAIL  Serial.println(FAIL_TXT)

#ifdef __PWM_GPIO_H__
bool pwmRangeTest() {
    Serial.println("pwmRange() test 1");
    bool all_passed = true;

    uint32_t inputs[] = {256, 512, 1023, 255};
    for(uint32_t i : inputs) {

        analogWriteRange(i);
        const uint32_t actual = ESPPwmGpio::pwmRange();

        Serial.printf("Expected %i, Actual %i \n", i, actual);
        if( i != actual ) {
            PRINT_FAIL;
            all_passed = false;
        }
    }

    return all_passed;
}
#else
bool pwmRangeTest() {
    return true;
}
#endif

#ifdef __ESP_BRIGHTNESS_RANGE_H__
bool mapToAnalogTests() {
    Serial.println("mapToAnalog() test 1 - fixed values");
    bool all_passed = true;

    const uint16_t ANALOG_RANGE = 1023;
    const uint8_t PERCENT_STEPS = 101;
    analogWriteRange(ANALOG_RANGE);

    int inputs[] = {0, 25, 50, 75, 100};
    for(int i : inputs) {

        const uint16_t expected = ANALOG_RANGE * ( log(i) / log(PERCENT_STEPS) );
        const uint16_t actual = ESPLedBrightness::percentToAnalog(i);

        Serial.printf("Expected %i, Actual %i\n", expected, actual);
        if( abs(expected - actual) > 20 )
            PRINT_FAIL;
    }

    return all_passed;
}
#else
bool mapToAnalogTests() { return true; }
#endif

#ifdef __PWM_GPIO_H__
bool gpioInitTest() {
    Serial.println("ESPPwmGpio test 1 - constructor");
    bool all_passed = true;
    const uint8_t PIN = 2;


    ESPPwmGpio gpio(2);
    const bool STARTED_LOW = digitalRead(PIN) == LOW ? true : false;

    digitalWrite(PIN, HIGH);
    const bool SET_PINMODE = digitalRead(PIN) == HIGH ? true : false;

    const uint8_t GET_PIN = gpio.pin();

    Serial.printf("Pin in LOW state upon init: Expected %s, actual %s\n", "true", STARTED_LOW ? "true" : "false");
    Serial.printf("Pin in output mode upon init: Expected %s, actual %s\n", "true", SET_PINMODE ? "true" : "false");
    Serial.printf("Pin getter test: Expected %i, actual %i\n", PIN, GET_PIN);
    
    if(!STARTED_LOW || !SET_PINMODE || PIN != GET_PIN){
        PRINT_FAIL;
        all_passed = false;
    }

    return all_passed;
}
#else
bool gpioInitTest() { return true; }
#endif

#ifdef __ESP_BRIGHTNESS_RANGE_H__
bool brightnessTests() {
    Serial.println("ESPLedBrightness tests");
    bool all_passed = true;
    ESPLedBrightness brightness;

    uint8_t constructor_test_low = 50, constructor_test_high = 75;
    ESPLedBrightness brightness2(50, 75);

     Serial.printf("Brightness constructor : Expected %i->%i, actual %i->%i\n", 
        constructor_test_low, 
        constructor_test_high,
        brightness2.minBrightnessPercent(),
        brightness2.maxBrightnessPercent()
    );
    if(constructor_test_low != brightness2.minBrightnessPercent() || constructor_test_high != brightness2.maxBrightnessPercent() ){
        PRINT_FAIL;
        all_passed = false;
    }


    Serial.printf("Default Brightness constructor : Expected %i->%i, actual %i->%i\n", 
        0, 
        100,
        brightness.minBrightnessPercent(),
        brightness.maxBrightnessPercent()
    );
    if(0 != brightness.minBrightnessPercent() || 100 != brightness.maxBrightnessPercent() ){
        PRINT_FAIL;
        all_passed = false;
    }

    uint8_t b1 = 25, b2 = 75;
    brightness2.maxBrightnessPercent(b1);
    brightness2.maxBrightnessPercent(b2);
    Serial.printf("Set brightness : Expected %i->%i, actual %i->%i\n", 
        b1, 
        b2,
        brightness2.minBrightnessPercent(),
        brightness2.maxBrightnessPercent()
    );
     if(b1 != brightness2.minBrightnessPercent() || b2 != brightness2.maxBrightnessPercent() ){
        PRINT_FAIL;
        all_passed = false;
    }

    return all_passed;
}
#else
bool brightnessTests() { return true; }
#endif

#ifdef __ESPLED_H__
bool ledTest1() {
    
    bool all_passed = true;
    const uint8_t PIN = 2;
    Serial.printf("ESPLed tests - MAKE SURE GPIO%i is connected to A0!\n", PIN);
    analogWriteRange(1023);

    ESPLed reg(PIN);

    const uint8_t GET_PIN = reg.pin();
    Serial.printf("Pin getter test: Expected %i, actual %i\n", PIN, GET_PIN);
    if(PIN != GET_PIN){
        PRINT_FAIL;
        all_passed = false;
    }

    uint8_t analog_values[] = {0, 100, 50};
    const size_t TESTS = sizeof(analog_values) / sizeof(analog_values[0]);
    Serial.println("Analog write value tests:");
    for(int i = 0; i < TESTS; i++) {    
        reg.on(analog_values[i]);
        const uint16_t EXPECTED = ESPLedBrightness::percentToAnalog(analog_values[i]);
        const uint16_t ACTUAL = 0;
        
        Serial.printf("Expected %i, actual %i\n", EXPECTED, ACTUAL);
        if( abs(EXPECTED - ACTUAL) > 20 ) {
            PRINT_FAIL;
            all_passed = false;
        }
    }

    reg.off();
    Serial.printf("isOn() test - Expected false, actual %s\n", reg.isOn() ? "true" : "false");
    if( reg.isOn() != false ) {
        PRINT_FAIL;
        all_passed = false;
    }

    reg.on();
    Serial.printf("isOn() test - Expected true, actual %s\n", reg.isOn() ? "true" : "false");
    if( reg.isOn() != true ) {
        PRINT_FAIL;
        all_passed = false;
    }





    
    return all_passed;

}
#else
bool ledTest1() { return true; }
#endif


#ifdef __ESP_EVENT_CHAIN_H__
bool eventChainTest1() {
    unsigned long t1 = 1000, t2 = 500, wait = (t1 + t2)*3;
    size_t count = 0;
    
    unsigned long currentT;

    EspEvent e1(t1, [&]() {
        Serial.printf("Tick: %i\n", millis() - currentT);
        count++;
    });

    EspEvent e2(t2, [&]() {
        Serial.printf("Tock: %i\n", millis() - currentT);
        count++;
    });

    EspEventChain chain(e1, e2);

    currentT = millis();
    chain.start();
    delay( wait + t1 / 100 );
    chain.stop();
    Serial.println("Called stop");
    Serial.printf("count: %i\n", count);

    delay(t1 + t2);
    return true;
}
#else
bool eventChainTest1() { return true; }
#endif


















void setup() {
    Serial.begin(115200);
    Serial.println("\n\n");
    Serial.println("Beginning tests\n");
    bool all_passed = true;
    pinMode(A0, INPUT);

    delay(1000);

    all_passed &= pwmRangeTest();
    Serial.println("\n");

    all_passed &= mapToAnalogTests();
    Serial.println("\n");

    all_passed &= gpioInitTest();
    Serial.println("\n");

    //all_passed &= ledTest1();

    Serial.println("\n");
    all_passed &= brightnessTests();

    Serial.println("\n");
    all_passed &= eventChainTest1();


    Serial.println("\n\n");
    if(all_passed) {
        Serial.println("All tests passed");
    }
    else {
        Serial.println("***** FAILED ONE OR MORE TESTS *****");
    }

}


void loop() {
    delay(10);

}