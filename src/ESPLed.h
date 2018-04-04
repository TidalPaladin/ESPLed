/*
		ESPLedBase.h
		Scott Chase Waggener  tidalpaladin@gmail.com
		10/10/2017

		This library simplifies the use of Leds on the Arduino framework
		Designed on ESP8266

*/

#ifndef __ESPLED_H__
#define __ESPLED_H__

#include <stdint.h>

#include "ESPBrightnessRange/ESPBrightnessRange.h"

class ESPLedBase;
class ESPLedInterface;

/**
 * Provides pin definitions
 */
#define NODEMCU_LED_BUILTIN D0  // NodeMCU led
#define ESP_BUILTIN 2			// The led on ESP12 / ESP32

class ESPLedBase {
   public:
	~ESPLedBase();

	/**
	 * @brief Gets the GPIO that the ESPLedBase is attached to
	 *
	 * @return GPIO number, 0 <= pin() <= (max GPIO num)
	 */
	virtual gpio_num_t getPin() const = 0;

	/**
	 * @brief Sets the maximum LED brightness as a percentage
	 *
	 * @param percent   The brightness as a percentage
	 *                  0 <= percent <= 100
	 *
	 * @return this
	 */
	ESPLedBase& setMaxBrightness(uint8_t percent);
	uint8_t getMaxBrightness() const;

	/**
	 * @brief Sets the minimum LED brightness as a percentage. This
	 * is the brightness that the LED will be set to when off() is called.
	 *
	 * @param percent   The brightness as a percentage
	 *                  0 <= percent <= 100
	 *
	 * @return this
	 *
	 */
	ESPLedBase& setMinBrightness(uint8_t percent);
	uint8_t getMinBrightness() const;

	/**
	 * @brief Assigns a strategy to the ESPLedBase
	 *
	 * @param strategy A reference to a strategy to use
	 *
	 * post: start() will cause the ESPLedBase to behave as instructed by
	 * strategy
	 *
	 * @return this
	 */
	ESPLedBase& setMode(ESPLedInterface& strategy);
	ESPLedInterface* getMode() const { return _strategy; }

	/**
	 * @brief Starts the LED's action for a given mode
	 *
	 * pre: setMode() has been called with a valid object
	 * post: The LED is taking the periodic action instructed by strategy
	 *
	 * @return this
	 */
	ESPLedBase& start();

	/**
	 * @brief Stops the LED's action
	 *
	 * post:  The LED is not taking a periodic action
	 *        LED remains in the on/off state it was in when stop() was called
	 *
	 * @return this
	 */
	ESPLedBase& stop(); 

	/**
	 * @brief Gets whether the LED is operating under some mode
	 *
	 * @return false if start() was called more recently than stop(), true
	 * otherwise
	 */
	bool isActive() const;

	/**
	 * @brief Gets the state of the LED
	 *
	 * @note  If minBrightness() != 0 and off() was called, isOn() == false but
	 * thesetM	 *        LED will be emitting light
	 *
	 * @return
	 *   - true if the LED is on
	 *   - false otherwise
	 */
	bool isOn() const;
	operator bool() const;

	/**
	 * @brief Gets whether the LED is on or off when the GPIO is high
	 *
	 * @return  true if on() will set the LED to HIGH
	 *          false if on() will set the LED to LOW
	 */
	bool highIsOn() const;

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
	virtual ESPLedBase& on(uint8_t percent = 100) = 0;

	/**
	 * @brief Turns the LED off
	 *
	 * post:  The LED is set to a brightness level of minBrightness()
	 *        isOn() == false
	 *
	 * @return this
	 */
	virtual ESPLedBase& off() = 0;

	/**
	 * @brief Toggles the LED between off() and on(percent)
	 *
	 * @param percent   The percent brightness
	 *                  0 <= percent <= 100
	 *
	 * post:  if isOn() == false before call, LED is on to 'percent' brightness,
	 * isOn() == true if isOn() == true before call, LED is set to
	 * minBrightness(), isOn() == false
	 *
	 * @return this
	 */
	ESPLedBase& toggle(uint8_t percent = 100);

   protected:
	bool _highIsOn : 1;
	ESPLedInterface* _strategy = nullptr;
	bool _isOn : 1;
	ESPLedBrightness _brightnessRange;
};

#endif

/* Include files for the strategy pattern */
#ifndef __ESPLED_INTERFACE_H__

#include "Interface/ESPBlink.h"
#include "Interface/ESPLedInterface.h"
#include "Interface/ESPPulse.h"

#endif

/* Include the derived class for appropriate board */
#ifdef ESP32
#include "ESP32/ESP32Led.h"
#else
#include "ESP8266/ESP8266Led.h"
#endif
