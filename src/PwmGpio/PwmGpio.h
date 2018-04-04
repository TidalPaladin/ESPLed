/**
 * @author Scott Chase Waggener
 * @date 2/4/18
 * General purpose GPIO adapter to handle PWM. Especially helpful for ESP32
 * where the PWM GPIO has extra variables like channel and frequency
 *
 */

#ifndef __PWM_GPIO_H__
#define __PWM_GPIO_H__

#include <stdint.h>

#ifndef ESP32
typedef uint8_t gpio_num_t;
#else
extern "C" {
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_err.h"
}
#endif

#define PWM_GPIO_DEFAULT_DUTY_HZ 1000

class ESPPwmGpioBase {
   public:
	/**
	 * @brief Constructor that attempts to choose led parameters automatically
	 *
	 * @param pin       The GPIO pin number to attach this object to
	 *                  0 <= pin < GPIO_PIN_COUNT (40)
	 *
	 */
	ESPPwmGpio(gpio_num_t pin) {
		setPin(pin);
		write(0);
	}

	/**
	 * @brief Ensures GPIO is in a good state before destroying the object
	 *
	 * post: PWM stopped, GPIO in LOW state
	 */
	~ESPPwmGpio();

	/**
	 * @brief Helper function to write duty cycles between ESP32 and ESP8266
	 *
	 * @param analog_value The PWM value to write to the led
	 *
	 * post:    GPIO generating a PWM signal,
	 *          signal frequency = pwmFrequency, signal duty = analog_value /
	 * 			pwmRange()
	 *
	 */
	virtual void write(uint32_t analog_value) = 0;

	/**
	 * @brief Gets the GPIO to which this LED is attached
	 *
	 * @return The GPIO, 0 <= pin() <= (max GPIO number)
	 */
	virtual void setPin(gpio_num_t pin) = 0;
	virtual gpio_num_t getPin() const = 0;

	// For compatibility with ESP8266
	virtual uint16_t pwmRange() const = 0;
	virtual uint32_t pwmFreq() const = 0;

	/**
	 * @brief Gets the frequency of the PWM signal in Hz on
	 * ESP32
	 *
	 *
	 * @return The frequency in Hz, frequency() > 0
	 */
	virtual void setFrequency(uint32_t freq_hz) = 0;
	virtual uint32_t getFrequency() const = 0;

   private:
	void constructDefault(gpio_num_t pin);

	void setPinToRestingState();
};
#endif

#ifndef ESP32
#include "PwmGpio8266.h"
#else
#include "PwmGpio32.h"
#endif
