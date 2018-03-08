#include "TestHelper.h"
#include "ESPLed.h"
#include "EspEventChain.h"

void wdtFeed() {
	#ifndef ESP32
		ESP.wdtFeed();
	#endif
}

#ifdef ESP32

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
	#ifndef ESP32
	analogWriteRange(ANALOG_RANGE);
	#endif
	int inputs[] = {0, 25, 50, 75, 100};

	for(int i : inputs) {

		// Polynomial approximation
		const uint16_t expected = ANALOG_RANGE * pow((double)i/100,3);
		const uint16_t actual = ESPLedBrightness::percentToAnalog(i);

		test.printResult(expected, actual);
	}

	for(int i = 0; i <= 100; i++) {
		ESPLedBrightness::percentToAnalog(i);
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

bool gpioAnalogWriteTest() {
	TestHelper test("ESPPwmGpio::analogWrite", "handling bad values");
	const uint8_t PIN = 2;
	analogWriteRange(PWMRANGE);

	ESPPwmGpio gpio(2);
	
	const uint32_t BAD_VALUES[] = {1025, 60000, 1000000000};
	
	for(uint16_t analogVal : BAD_VALUES) {
		gpio.analogWrite(analogVal);
	}
	
	return test.printResult();
}

bool gpioAnalogWriteTest2() {
	TestHelper test("ESPPwmGpio::analogWrite", "handling good values");
	const uint8_t PIN = 2;
	analogWriteRange(PWMRANGE);

	ESPPwmGpio gpio(2);
	
	for(int repeats = 0; repeats < 5; repeats++){
		for(int i = 0; i < PWMRANGE; i++) {
			gpio.analogWrite(i);
			wdtFeed();
		}
		for(int i = PWMRANGE; i >= 0; i--) {
			gpio.analogWrite(i);
			wdtFeed();
		}
	}
	
	return test.printResult();
}

#else
	bool gpioInitTest() { return true; }
	bool gpioAnalogWriteTest() { return true; }
	bool gpioAnalogWriteTest2() { return true; }
#endif













#ifdef __ESP_BRIGHTNESS_RANGE_H__
bool brightnessTests() {
	TestHelper test("ESPLedBrightness","various fixed range tests");
   
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
		brightness.setMaxBrightnessPercent(MAX);
		brightness.setMinBrightnessPercent(MIN);
		test.printResult(MIN, brightness.getMinBrightnessPercent());
		test.printResult(MAX, brightness.getMaxBrightnessPercent());
	}

	return test.printResult();
}

bool brightnessTests2() {
	TestHelper test("ESPLedBrightness","various fixed conversion tests");
   
	ESPLedBrightness brightness;

	uint8_t test_values[] = {0, 1, 20, 50, 75, 90, 99, 100};
	const uint8_t NUM_VALUES = sizeof(test_values) / sizeof(test_values[0]);

	for(int test_case = 0; test_case < NUM_VALUES; test_case++) {
		test.printResultRange(512, ESPLedBrightness::percentToAnalog(test_values[test_case]), 512);
	}

	return test.printResult();
}

bool brightnessTests3() {
	TestHelper test("ESPLedBrightness","percents > 100");
   
	ESPLedBrightness brightness;
	analogWriteRange(PWMRANGE);

	uint32_t test_values[] = {101, 1023};
	const uint8_t NUM_VALUES = sizeof(test_values) / sizeof(test_values[0]);

	for(int test_case = 0; test_case < NUM_VALUES; test_case++) {

		const uint32_t EXPECTED = test_case & B11111111;
		test.printResult(PWMRANGE, ESPLedBrightness::percentToAnalog(test_values[test_case]));
	}

	return test.printResult();
}
#else
	bool brightnessTests() { return true; }
	bool brightnessTests2() { return true; }
	bool brightnessTests3() { return true; }
#endif







#ifdef __ESPLED_H__
bool ledBasicTest1() {

	TestHelper test("ESPLed","various fixed tests");
	
	const uint8_t PIN = 2;
	analogWriteRange(PWMRANGE);
	ESPLed led(PIN, HIGH);

	Serial.println("Checking pin assignment");
	test.printResult(PIN, led.getPin());

	test.printResult(false, led.highIsOn());

	led.on();
	test.printResult(LOW, digitalRead(PIN));

	led.off();
	test.printResult(HIGH, digitalRead(PIN));
	test.printResult(false, led.isOn());

	led.on();
	test.printResult(true, led.isOn());

	led.toggle();
	test.printResult(HIGH, digitalRead(PIN));

	test.printResult(100, led.getMaxBrightness());
	test.printResult(0, led.getMinBrightness());
	test.printResult(false, led.isActive());

	return test.printResult();
}

bool ledBasicTest2() {

	TestHelper test("ESPLed::on()","fast updates to brightness");
	
	const uint8_t PIN = 2;
	analogWriteRange(PWMRANGE);
	ESPLed led(PIN, HIGH);

	for(int repeats = 0; repeats < 6; repeats++) {
		for(int level = 0; level < 110; level++){
			wdtFeed();
			led.on(level);
		}
		for(int level = 100; level >= 0; level--) {
			wdtFeed();
			Serial.println(level); Serial.flush();
			led.on(level);
		}
	}

	return test.printResult();
}
#else
bool ledTest1() { return true; }
bool ledTest2() { return true; }
#endif




#ifdef __ESP_BLINK_H__

bool blinkTest1() {
	TestHelper test("ESPBlink()", "visual blink test");

	ESPLed led(2, HIGH);
	ESPBlink blink;
	led.setMode(blink);

	const unsigned long default_duration = 300;
	const unsigned long default_interval = 2000;
	 

	Serial.println("Checking default blink timings");
	test.printResult( default_interval, blink.getInterval() );
	test.printResult( default_duration, blink.getDuration() );

	Serial.println("Checking attached led count before start()");
	test.printResult(0, blink.getAttachedLedCount());

	Serial.println("Basic blink test");
	led.start();

	Serial.println("Checking attached led count after start()");
	test.printResult(1, blink.getAttachedLedCount());

	Serial.println("Checking blink.isStarted()");
	test.printResult(true, blink.isStarted());
	test.printResult(true, led.isActive());

	delay(4000);
	
	Serial.println("Test changing parameters on the fly");
	const int NEW_INTERVAL = 100;
	const int NEW_DURATION = 50;
	blink.setInterval(NEW_INTERVAL);
	blink.setDuration(NEW_DURATION);
	test.printResult( NEW_INTERVAL, blink.getInterval() );
	test.printResult( NEW_DURATION, blink.getDuration() );

	delay(4000);
	blink.stopAll();

	test.printResult(false, blink.isStarted());
	test.printResult();

}


bool blinkTest2() {
	TestHelper test("ESPBlink()", "test time conversion methods");

	const unsigned long INITIAL_MS = 100;
	const unsigned long EXPECTED_HZ = 10;

	test.printResult( EXPECTED_HZ, ESPLedInterface::millisecondsToHz(INITIAL_MS) );
	test.printResult( INITIAL_MS, ESPLedInterface::hzToMilliseconds(EXPECTED_HZ) );
	test.printResult( 0, ESPLedInterface::hzToMilliseconds(0) );
	test.printResult( 0, ESPLedInterface::millisecondsToHz(0) );

	test.printResult();

}

#else
bool blinkTest1() { return true; }

#endif




#ifdef __ESP_PULSE_H__

bool pulseTest1() {
	TestHelper test("ESPPulse()", "constructors");

	ESPLed led(2, HIGH);
	ESPPulse pulse;
	led.setMode(pulse);

	const unsigned long default_refresh_rate_hz = 30;
	const unsigned long default_period_ms = 2000;
	 

	Serial.println("Checking default pulse timings");
	test.printResult( default_refresh_rate_hz, pulse.getRefreshRate() );
	test.printResult( default_period_ms, pulse.getPeriod() );

	Serial.println("Checking attached led count before start()");
	test.printResult(0, pulse.getAttachedLedCount());

	Serial.println("Basic pulse test");
	led.start();

	Serial.println("Checking attached led count after start()");
	test.printResult(1, pulse.getAttachedLedCount());

	Serial.println("Checking pulse.isStarted()");
	test.printResult(true, pulse.isStarted());

	delay(4000);
	
	Serial.println("Test changing parameters on the fly");
	const int NEW_REFRESH_RATE = 60;
	const int NEW_PERIOD = 1000;
	pulse.setRefreshRate(NEW_REFRESH_RATE);
	pulse.setPeriod(NEW_PERIOD);
	test.printResult( NEW_REFRESH_RATE, pulse.getRefreshRate() );
	test.printResult( NEW_PERIOD, pulse.getPeriod() );
	Serial.println("Should be pulsing fast now!");

	for(uint32_t t0 = millis(); millis() - t0 < 10000; ){
		yield();
	}
	Serial.println("About to stop all");
	pulse.stopAll();

	test.printResult(false, pulse.isStarted());
	test.printResult();
}
#else
bool pulseTest1() { return true; }
#endif


void induceAnalogWriteFail() {
	const uint8_t PIN = 2;

	analogWrite(2, 512);

	pinMode(2, OUTPUT);
	analogWriteRange(1025);
	analogWrite(2, 10);
	analogWrite(2, 0);
	analogWrite(2,0);
	analogWrite(2,0);
	analogWrite(2,0);
	analogWrite(2,0);
	analogWrite(2, 1050);

}0-






void setup() {
	Serial.begin(115200);
	Serial.println("\n\n");
	Serial.println("Beginning tests\n");
	delay(1000);
	//analogWriteFreq(200);

	induceAnalogWriteFail();

	pwmRangeTest1();
	mapToAnalogTests();

	gpioInitTest();
	gpioAnalogWriteTest();
	gpioAnalogWriteTest2();

	brightnessTests();
	brightnessTests2();
	brightnessTests3();

	ledBasicTest1();
	ledBasicTest2();

	ESP.wdtEnable(500);
	// blinkTest1();
	// blinkTest2();
	pulseTest1();


	TestHelper::end();
}


void loop() {
	delay(10);

}