#ifdef UNIT_TEST
#include <Arduino.h>
#include "ESPLed.h"
#include "unity.h"

const int PIN1 = 2;
const int PIN2 = 3;
#ifndef ESP32
ESPLed led1(PIN1);
ESPLed led2(PIN2, HIGH);
#else
ESPLed led1(PIN1, 0);
ESPLed led2(PIN2, 1, HIGH);
#endif



void setup() {
	delay(2000);
	UNITY_BEGIN();
	RUN_TEST(initial_values);
	RUN_TEST(getPin);
	RUN_TEST(getMinBrightness);
	RUN_TEST(getMaxBrightness);
	RUN_TEST(highIsOn);
	RUN_TEST(setMaxBrightness);
	RUN_TEST(setMinBrightness);
	RUN_TEST(brightnessSetBad);
	RUN_TEST(toggle);
	RUN_TEST(on);

	UNITY_END();
}

void loop() {}

#endif
