#ifndef __ESP_COLOR_LED_H__
#define __ESP_COLOR_LED_H__

#include "ESPLed.h"
#include "ESPColor.h"

/**
 * 
 * Should this be inheritance or composition?
 * 
 */


class ESPBiColorLed {

public:

#ifdef ESP32

	ESPLed(gpio_num_t pin, uint8_t channel = 0, uint16_t freq_hz = 1000, uint8_t off_state = LOW);

#else

	/**
	 * @brief Constructor for ESP8266
	 * 
	 * @param pin1              The GPIO to use for the first color
	 *                          0 <= pin1 <= (8266 / 32 max GPIO number)
     * 
     * @param pin2              The GPIO to use for the second color
     *                          0 <= pin2 <= (max gpio), pin2 != pin1
	 * 
	 * @param resting_state   The GPIO state when the ESPLed is off
	 *                        resting_state == LOW || resting_state == HIGH
	 * 
	 */
	ESPBiColorLed(gpio_num_t pin1, gpio_num_t pin2, uint8_t off_state = LOW)
	:
	leds[0]( ESPLed(pin1, off_state) ),
	leds[1]( ESPLed(pin2, off_state) )
	{

	}


#endif

	const uint8_t[2] getPin() { return {leds[0].getPin(), leds[1].getPin()}; }

    ESPBiColorLed &setColor(ESPBiColorLed bicolor);

};


private:

	ESPLed leds[2];

#endif