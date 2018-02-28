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
#else
bool ledTest1() { return true; }
#endif


#ifdef __ESP_EVENT_CHAIN_H__


void eventChainTickTimeHelper(TestHelper &test, unsigned long t1, unsigned long t2) {

	const uint32_t DELAY_MARGIN_ERR = 2;
	unsigned long wait = 3*(t1+t2);
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
	delay( wait + DELAY_MARGIN_ERR );
	chain.stop();

	Serial.println("Checking expected ticks");
	unsigned long expected_ticks = 2 * wait / (t1+t2) + 1;
	test.printResult(expected_ticks, count); 

}

bool eventChainTest4() {
	TestHelper test("EspEventChain", "constructors");

	const uint8_t fixed_size = 10;
	std::function<void()> f = []() {

	};
	unsigned long t1 = 20, t2 = 30;

	EspEventChain chain1;
	EspEventChain chain2(100);
	EspEventChain chain3( EspEvent(t1, f), EspEvent(t2, f) );

	test.printResult(0, chain1.numEvents());
	test.printResult(0, chain2.numEvents());
	test.printResult(2, chain3.numEvents());

	test.printResult(t1, chain3.getTimeOf(0));
	test.printResult(t2, chain3.getTimeOf(1));

	test.printResult();
}


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

	chain.addEvent(e1);
	test.printResult(3, chain.numEvents());
	test.printResult(2*t1+t2, chain.totalTime());
	test.printResult(t1, chain.getTimeOf(2));

	return test.printResult();

}


bool eventChainTest2() {
	TestHelper test("EspEventChain", "Ticking properly");
	unsigned long t1 = 100, t2 = 75, wait = (t1 + t2)*3;
	eventChainTickTimeHelper(test, t1, t2);
	test.printResult();
}

bool eventChainTest3() {
	TestHelper test("ESPEventChain()","events stored by copy, not reference");
	
	bool ticked = false;
	unsigned long wait = 30;

	EspEvent *e1 = new EspEvent(30, [&]() {
		ticked = true;
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

bool eventChainTest5() {
	TestHelper test("ESPEventChain()","time=0 events");
	unsigned long t1 = 0, t2 = 420;
	eventChainTickTimeHelper(test, t1, t2);
	test.printResult();
}

bool eventChainTest6() {
	TestHelper test("ESPEventChain()","speed test");
	unsigned long t1 = 0, t2 = 10;
	eventChainTickTimeHelper(test, t1, t2);
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

	const unsigned long default_duration = 300;
	const unsigned long default_interval = 2000;
	 

	Serial.println("Checking default blink timings");
	test.printResult( default_interval, blink.interval() );
	test.printResult( default_duration, blink.duration() );

	Serial.println("Checking attached led count before start()");
	test.printResult(0, blink.attachedLedCount());

	Serial.println("Basic blink test");
	led.start();

	Serial.println("Checking attached led count after start()");
	test.printResult(1, blink.attachedLedCount());

	Serial.println("Checking blink.isStarted()");
	test.printResult(true, blink.isStarted());
	test.printResult(true, led.isActive());

	delay(4000);
	
	Serial.println("Test changing parameters on the fly");
	const int NEW_INTERVAL = 100;
	const int NEW_DURATION = 50;
	blink.interval(NEW_INTERVAL);
	blink.duration(NEW_DURATION);
	test.printResult( NEW_INTERVAL, blink.interval() );
	test.printResult( NEW_DURATION, blink.duration() );

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

#endif
#else
bool blinkTest1() { return true; }

#endif




#ifdef __ESP_PULSE_H__

bool pulseTest1() {
	TestHelper test("ESPPulse()", "constructors");

	ESPLed led(2, HIGH);
	ESPPulse pulse;
	led.setMode(pulse);

	const unsigned long default_refresh_rate_hz = 60;
	const unsigned long default_period_ms = 2000;
	 

	Serial.println("Checking default pulse timings");
	test.printResult( default_refresh_rate_hz, pulse.getRefreshRate() );
	test.printResult( default_period_ms, pulse.getPeriod() );

	Serial.println("Checking attached led count before start()");
	test.printResult(0, pulse.attachedLedCount());

	Serial.println("Basic pulse test");
	led.start();

	Serial.println("Checking attached led count after start()");
	test.printResult(1, pulse.attachedLedCount());

	Serial.println("Checking pulse.isStarted()");
	test.printResult(true, pulse.isStarted());

	delay(4000);
	
	Serial.println("Test changing parameters on the fly");
	const int NEW_REFRESH_RATE = 150;
	const int NEW_PERIOD = 300;
	pulse.setRefreshRate(NEW_REFRESH_RATE);
	pulse.setPeriod(NEW_PERIOD);
	test.printResult( NEW_REFRESH_RATE, pulse.getRefreshRate() );
	test.printResult( NEW_PERIOD, pulse.getPeriod() );
	Serial.println("Should be pulsing fast now!");

	delay(4000);
	pulse.stopAll();

	test.printResult(false, pulse.isStarted());
	test.printResult();
}
#else
bool pulseTest1() { return true; }
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

	ledBasicTest1();

	eventChainTest4();
	eventChainTest1();
	eventChainTest2();
	eventChainTest3();
	eventChainTest5();
	eventChainTest6();


	blinkTest1();
	blinkTest2();
pulseTest1();


	TestHelper::end();
}


void loop() {
	delay(10);

}