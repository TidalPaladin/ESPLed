/*
	ESPLed.h
	Scott Chase Waggener  tidalpaladin@gmail.com
	10/10/2017

	This library simplifies the use of Leds on the Arduino framework
	Designed on ESP8266

*/

#ifdef ESP32

#ifndef __ESPLED_32_H__
#define __ESPLED_32_H__

#include <array>
#include "../ESPLed.h"

extern "C" {
#ifndef ESPIDF
#include "esp32-hal-ledc.h"
#else
#include "ledc.h"
#endif
}

#define __ESPLED_32_CHANNELS__ 10

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

   private:
	/**
	 * @brief   Determines which channel to attach a Led to when no channel
	 *          specified in constructor
	 *
	 * @return The channel with the fewest number of attached Leds
	 *          0 <= ret <= __ESPLED_32_CHANNELS__
	 */
	static uint8_t findNextDefaultChannel();
};

#endif
#endif