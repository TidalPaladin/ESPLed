/**
 * @author Scott Chase Waggener
 * @date 2/4/18
 * General purpose GPIO adapter to handle PWM. Especially helpful for ESP32
 * where the PWM GPIO has extra variables like channel and frequency
 *
 */

#ifndef __PWM_GPIO_32_H__
#define __PWM_GPIO_32_H__

#include <array>
#include "PwmGpio.h"

#define __ESP_MAX_GPIO_NUM__ 16
extern uint32_t pwm_range;
extern uint32_t pwm_freq;

class ESPPwmGpio : public ESPPwmGpioBase {
   private:
	ledc_channel_config_t ledConfig;
	ledc_timer_config_t timerConfig;

	static std::array<uint8_t, LEDC_CHANNEL_MAX> channels;

   public:
	/**
	 * @brief Explicit constructor
	 *
	 * @param pin       The GPIO pin number to attach this object to
	 *                  0 <= pin < GPIO_PIN_COUNT (40)
	 *
	 * @param channel   The ESP32 PWM channel to attach the object to
	 *                  0 <= channel <= LEDC_CHANNEL_MAX (7)
	 *
	 * @param freq_hz   The PWM frequency in Hz
	 *                  freq_hz = a reasonable PWM frequency for the ESP32
	 *
	 * post: GPIO 'pin' set to output mode, GPIO 'pin' in LOW state
	 */
	ESPPwmGpio(gpio_num_t pin,
			   ledc_mode_t speed_mode,
			   ledc_channel_t channel,
			   ledc_timer_bit_t resolution,
			   ledc_timer_t timer_num,
			   uint32_t freq_hz);

	/**
	 * @brief Simplified constructor
	 *
	 * @param pin       The GPIO pin number to attach this object to
	 *                  0 <= pin < GPIO_PIN_COUNT (40)
	 *
	 * @param channel   The ESP32 PWM channel to attach the object to
	 *                  0 <= channel <= LEDC_CHANNEL_MAX (7)
	 *
	 * @param freq_hz   The PWM frequency in Hz
	 *                  freq_hz = a reasonable PWM frequency for the ESP32
	 *
	 * post: GPIO 'pin' set to output mode, GPIO 'pin' in LOW state
	 */
	ESPPwmGpio(gpio_num_t pin);

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
	void write(uint32_t analog_value) override;

	/**
	 * @brief Gets the GPIO to which this LED is attached
	 *
	 * @return The GPIO, 0 <= pin() <= (max GPIO number)
	 */
	void setPin(gpio_num_t pin) override;
	gpio_num_t getPin() const override;

	/**
	 * @brief Gets which of the 16 PWM channels is in use on ESP32
	 *
	 *
	 * @return The channel, 0 <= channel() <= 15
	 */
	void setChannel(ledc_channel_t channel);
	ledc_channel_t getChannel() const;

	// For compatibility with ESP8266
	uint16_t pwmRange() const;
	uint32_t pwmFreq() const;

	/**
	 * @brief Gets the frequency of the PWM signal in Hz on
	 * ESP32
	 *
	 *
	 * @return The frequency in Hz, frequency() > 0
	 */
	void setFrequency(uint32_t freq_hz);
	uint32_t getFrequency() const { return timerConfig.freq_hz; }

	void setSpeedMode(ledc_mode_t speed_mode);
	ledc_mode_t getSpeedMode() const { return timerConfig.speed_mode; }

	void setResolution(ledc_timer_bit_t duty_resolution);
	ledc_timer_bit_t getResolution() const {
		return timerConfig.duty_resolution;
	}

	/**
	 * @brief Sets the duty cycle from 0-100
	 *
	 * @param duty	The duty cycle, 0 <= duty <= 100
	 *
	 */
	void setDuty(uint32_t duty);
	uint32_t getDuty() const;

	void setTimer(ledc_timer_t timer);
	ledc_timer_t getTimer() const { return timerConfig.timer_num; }

	ledc_channel_config_t getLedConfig() const { return ledConfig; }
	ledc_timer_config_t getTimerConfig() const { return timerConfig; }

   private:
	/**
	 * @brief Gets the ledc_channel_t with the fewest number of PwmGpio objects
	 * attached to it
	 *
	 * @return Least occupied ledc_channel_t
	 */
	static ledc_channel_t findNextDefaultChannel();
};

#endif