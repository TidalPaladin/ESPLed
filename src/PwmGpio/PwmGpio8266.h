/**
 * @author Scott Chase Waggener
 * @date 2/4/18
 * General purpose GPIO adapter to handle PWM. Especially helpful for ESP32
 * where the PWM GPIO has extra variables like channel and frequency
 *
 */

#ifdef ESP8266
#ifndef __PWM_GPIO_H__
#define __PWM_GPIO_H__

#include <stdint.h>
#include "pwm.h"

#ifndef PWMRANGE
#define PWMRANGE 1023
#endif

typedef uint8_t gpio_num_t;

/* Not the number of GPIOs, but the largest GPIOXX number */
#ifdef ESP32
#define __ESP_MAX_GPIO_NUM__ 16
extern "C" {
extern uint32_t pwm_range;
extern uint32_t pwm_freq;
void pwm_stop_pin(uint8_t pin);
}
#else
#include <Arduino.h>
#define __ESP_MAX_GPIO_NUM__ 39
#endif

#ifdef ESP32
#define __ESP_MAX_GPIO_CHANNEL__ 15
// #define __ESP_MAX_GPIO_FREQ__
#endif

class ESPPwmGpio {

  public:
	/**
	 * @brief Constructor for a given pin
	 *
	 * @param pin       The GPIO pin number to attach this object to
	 *                  0 <= pin <= (max GPIO number)
	 *
	 * @param channel   The ESP32 PWM channel to attach the object to
	 *                  0 <= channel <= 15
	 *
	 * @param freq_hz   The PWM frequency in Hz
	 *                  freq_hz = a reasonable PWM frequency for the ESP32
	 *
	 * post: GPIO 'pin' set to OUTPUT mode, GPIO 'pin' in LOW state
	 */
	ESPPwmGpio(gpio_num_t pin, uint8_t channel = 0, uint16_t freq_hz = 1000)
		: _pin(pin), _channel(channel), _frequency(freq_hz) {

		initialize();
	}

	/**
	 * @brief Ensures GPIO is in a good state before destroying the object
	 *
	 * post: PWM stopped, GPIO in LOW state
	 */
	~ESPPwmGpio();

	/**
	 * @brief Initializes the GPIO in the manner appropriate to the
	 * microcontroller and framework in use
	 *
	 * post: GPIO in OUTPUT mode, GPIO in LOW state
	 *
	 */
	void initialize();

	/**
	 * @brief Helper function to write duty cycles between ESP32 and ESP8266
	 *
	 * @param analog_value The PWM value to write to the led
	 *
	 * post:    GPIO generating a PWM signal,
	 *          signal frequency = pwmFrequency, signal duty = analog_value /
	 * pwmRange()
	 *
	 */
	void analogWrite(volatile uint16_t analog_value);

	/**
	 * @brief Gets the GPIO to which this LED is attached
	 *
	 * @return The GPIO, 0 <= pin() <= (max GPIO number)
	 */
	gpio_num_t pin() const;

	/**
	 * @brief Gets whether the written duty cycle is the inverse of what is
	 * supplied
	 *
	 * @return value of onWhenHigh
	 */
	ESPPwmGpio onWhenHigh(bool value);

	bool onWhenHigh() const;

	/**
	 * @brief Gets the global PWM range / frequency
	 *
	 * @detials The if statement MUST be there, otherwise exception 29.
	 * No idea why
	 *
	 */
	static uint32_t pwmRange();
	static uint32_t pwmFrequency();

#ifdef ESP32

	/**
	 * @brief Gets which of the 16 PWM channels is in use on
	 * ESP32
	 *
	 *
	 * @return The channel, 0 <= channel() <= 15
	 */
	uint8_t channel() const;

	/**
	 * @brief Gets the frequency of the PWM signal in Hz on
	 * ESP32
	 *
	 *
	 * @return The frequency in Hz, frequency() > 0
	 */
	unsigned long frequency() const;

#endif

  private:
	// inline uint16_t correctAnalogValue(uint16_t analog_value) const;

	bool _onWhenHigh : 1;
	gpio_num_t _pin;
};

#endif
#endif