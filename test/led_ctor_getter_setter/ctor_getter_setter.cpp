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

void initial_values() {

	TEST_ASSERT_EQUAL_MESSAGE(LOW, digitalRead(PIN1), "Initial pin state low");
	TEST_ASSERT_EQUAL_MESSAGE(LOW, digitalRead(PIN2), "Initial pin state low");

	TEST_ASSERT_FALSE_MESSAGE(led1.isOn(), "Initial isOn() == false");
	TEST_ASSERT_FALSE_MESSAGE(led2.isOn(), "Initial isOn() == false");

	TEST_ASSERT_FALSE_MESSAGE(led1.isActive(), "Initial isActive() == false");
	TEST_ASSERT_FALSE_MESSAGE(led2.isActive(), "Initial isActive() == false");

	TEST_ASSERT_EQUAL_MESSAGE(0, led1.getMinBrightness(),
							  "Initial maxBrightness()");
	TEST_ASSERT_EQUAL_MESSAGE(0, led2.getMinBrightness(),
							  "Initial maxBrightness()");

	TEST_ASSERT_EQUAL_MESSAGE(100, led1.getMaxBrightness(),
							  "Initial minBrightness()");
	TEST_ASSERT_EQUAL_MESSAGE(100, led2.getMaxBrightness(),
							  "Initial minBrightness()");
}

void getPin() {
	TEST_ASSERT_EQUAL_MESSAGE(PIN1, led1.getPin(), "getPin() for led1");
	TEST_ASSERT_EQUAL_MESSAGE(PIN2, led2.getPin(), "getPin() for led2");
}

void getMaxBrightness() {
	TEST_ASSERT_EQUAL_MESSAGE(0, led1.getMaxBrightness(),
							  "getMaxBrightness() for led 1");
	TEST_ASSERT_EQUAL_MESSAGE(0, led2.getMaxBrightness(),
							  "getMaxBrightness() for led 2");
}

void getMinBrightness() {
	TEST_ASSERT_EQUAL_MESSAGE(0, led1.getMinBrightness(),
							  "getMinBrightness() for led 1");
	TEST_ASSERT_EQUAL_MESSAGE(0, led2.getMinBrightness(),
							  "getMinBrightness() for led 2");
}

void highIsOn() {
	TEST_ASSERT_TRUE(led1.highIsOn());
	TEST_ASSERT_FALSE(led2.highIsOn());
}

void setMaxBrightness() {
	int set1 = 50;
	int set2 = 30;
	led1.setMaxBrightness(set1);
	led2.setMaxBrightness(set2);

	TEST_ASSERT_EQUAL_MESSAGE(set1, led1.getMaxBrightness(),
							  "getMaxBrightness() after set for led 1");
	TEST_ASSERT_EQUAL_MESSAGE(set2, led2.getMaxBrightness(),
							  "getMaxBrightness() after set for led 2");

	led1.setMaxBrightness(100);
	led2.setMaxBrightness(100);

	TEST_ASSERT_EQUAL_MESSAGE(100, led1.getMaxBrightness(),
							  "getMaxBrightness() after set for led 1");
	TEST_ASSERT_EQUAL_MESSAGE(100, led2.getMaxBrightness(),
							  "getMaxBrightness() after set for led 2");
}

void setMinBrightness() {
	int set1 = 50;
	int set2 = 30;
	led1.setMinBrightness(set1);
	led2.setMinBrightness(set2);

	TEST_ASSERT_EQUAL_MESSAGE(set1, led1.getMinBrightness(),
							  "getMinBrightness() after set for led 1");
	TEST_ASSERT_EQUAL_MESSAGE(set2, led2.getMinBrightness(),
							  "getMinBrightness() after set for led 2");

	led1.setMaxBrightness(0);
	led2.setMaxBrightness(0);

	TEST_ASSERT_EQUAL_MESSAGE(0, led1.getMinBrightness(),
							  "getMinBrightness() after set for led 1");
	TEST_ASSERT_EQUAL_MESSAGE(0, led2.getMinBrightness(),
							  "getMinBrightness() after set for led 2");
}

void brightnessSetBad() {

	led1.setMaxBrightness(50);
	led1.setMinBrightness(60);

	TEST_ASSERT_EQUAL_MESSAGE(59, led1.getMinBrightness(),
							  "getBrightness after bad set");
	TEST_ASSERT_EQUAL_MESSAGE(60, led2.getMinBrightness(),
							  "getBrightness after bad set");

	led1.setMaxBrightness(50);

	TEST_ASSERT_EQUAL_MESSAGE(49, led1.getMinBrightness(),
							  "getBrightness after bad set");
	TEST_ASSERT_EQUAL_MESSAGE(50, led2.getMinBrightness(),
							  "getBrightness after bad set");
}

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
