/*
	ESPLed.h
	Scott Chase Waggener  tidalpaladin@gmail.com
	10/10/2017

	This library simplifies the use of Leds on the Arduino framework
	Designed on ESP8266

*/

#ifdef ESP8266

#ifndef __ESPLED_8266_H__
#define __ESPLED_8266_H__

#include "../ESPLed.h"

/**
 * Provides pin definitions
 */
#define NODEMCU_LED_BUILTIN D0  // NodeMCU led
#define ESP_BUILTIN 2			// The led on ESP12 / ESP32

class ESPLed : public ESPLedBase {
   private:
	static std::array<uint8_t, __ESPLED_32_CHANNELS__> leds_per_channel;

   public:
	ESPLed(uint8_t channel, double freq, uint8_t resolution_bits);

	ESPLed();

	/**
	 * @brief Gets the GPIO that the ESPLed is attached to
	 *
	 * @return GPIO number, 0 <= pin() <= (max GPIO num)
	 */
	virtual gpio_num_t getPin() const override;

	/**
	 * @brief Turns the LED on to a given brightness
	 *
	 * @param percent   The brigness to turn on to
	 *                  0 <= percent <= 100
	 *
	 * post: The LED is on at 'percent' brightness, isOn() == true
	 *
	 * @return this
	 */
	virtual ESPLed& on(uint8_t percent = 100) override;

	/**
	 * @brief Turns the LED off
	 *
	 * post:  The LED is set to a brightness level of minBrightness()
	 *        isOn() == false
	 *
	 * @return this
	 */
	virtual ESPLed& off() override;
};

#endif
#endif