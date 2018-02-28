/**
 * @author Scott Chase Waggener tidal@utexas.edu
 * @date 2/2/18
 * 
 * @description
 * Wraps maximum and minimum brightnesses in a single data structure
 * 
 */

#ifndef __ESP_BRIGHTNESS_RANGE_H__
#define __ESP_BRIGHTNESS_RANGE_H__

#include <Arduino.h>
#include <assert.h>
#include "PwmGpio.h"

#define __ESP_BRIGHTNESS_RANGE_MAX_IN__     100
#define __ESP_BRIGHTNESS_RANGE_MAX_OUT__    PWMRANGE
#define __ESP_BRIGHTNESS_RANGE_GAMMA__      2.8


class ESPLedBrightness {

	public:

		/**
		 * @brief Default constuctor
		 * 
		 * post: Brightness range is set to 0 - 100% 
		 * 
		 */
		ESPLedBrightness()
		:
		ESPLedBrightness(0, 100)
		{

		}

		/**
		 * @brief Constructor with brightness settings
		 * 
		 * @param   min_percent The minimum brightness as a percentage
		 *          0 <= min_percent < max_percent
		 * @param   max_percent THe maximum brightness as a percentage
		 *          min_percent < max_percent <= 100
		 * 
		 */
		ESPLedBrightness(uint8_t min_percent, uint8_t max_percent)
		:
		_minBrightnessPercent(min_percent),
		_maxBrightnessPercent(max_percent)
		{
			assert(min_percent < max_percent && max_percent <= 100);
		}

		/**
		 * @brief Sets the maximum brightness as a percentage
		 * 
		 * @param percent   The brightness as a percent
		 *                  0 <= percent < 100
		 * 
		 * post: maxBrightnessPercent() == percent, minBrightnessPercent() < maxBrightnessPercent()
		 * 
		 * @return this
		 */
		ESPLedBrightness &setMaxBrightnessPercent(uint8_t percent);
		uint8_t getMaxBrightnessPercent() const { return _maxBrightnessPercent; }

		/**
		 * @brief Sets the minimum brightness as a percentage
		 * 
		 * @param percent   The brightness as a percent
		 *                  0 <= percent < 100
		 * 
		 * post: minBrightnessPercent() == percent, minBrightnessPercent() < maxBrightnessPercent()
		 * 
		 * @return this
		 */
		ESPLedBrightness &setMinBrightnessPercent(uint8_t percent);
		uint8_t getMinBrightnessPercent() const { return _minBrightnessPercent; }


		/**
		 * @brief   Maps a brightness from a percentage to a PWM value. The returned
		 *          percentage is compensated for the antilog nature of LED brightness
		 * 
		 * @details     Conversion is done using a 100 element lookup table for
		 *              speed
		 * 
		 * @param percent   The brightness as a percent
		 *                  0 <= percent <= 100
		 * 
		 * @return The PWM value
		 */
		static uint16_t percentToAnalog(uint8_t percent);


		/**
		 * @brief   Maps a brightness from a percentage to a PWM value. The returned
		 *          percentage is compensated for the antilog nature of LED brightness
		 * 
		 * @details     Conversion is done using a 100 element lookup table for
		 *              speed
		 * 
		 * @param percent   The brightness as a percent
		 *                  0 <= percent <= 100
		 * 
		 * @return The PWM value
		 */
		static constexpr uint16_t percentToAnalogExact(const uint8_t percent);

	private:

		// Bit field of 7 since brightness <= 100
		uint8_t _minBrightnessPercent : 7;
		uint8_t _maxBrightnessPercent : 7;

		static const uint16_t _brightnessLut[101];

};


#endif