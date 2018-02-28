
#ifndef __ESP_TRI_COLOR_LED__
#define __ESP_TRI_COLOR_LED__

#include "ESPBiColorLed.h"

class ESPTriColorLed : public ESPBiColorLed {

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
	ESPTriColorLed(gpio_num_t pin1, gpio_num_t pin2, gpio_num_t pin3, uint8_t off_state = LOW)
	:
    {

    }

#endif

    virtual ESPColorLed &setColor(ESPBiColorLed bicolor);

private:

    ESPPwmGpio _gpio[2];


};

#endif