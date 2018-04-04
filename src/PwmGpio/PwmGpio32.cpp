#include "PwmGpio32.h"

std::array<uint8_t, LEDC_CHANNEL_MAX> ESPPwmGpio::channels;

ESPPwmGpio::ESPPwmGpio(gpio_num_t pin,
					   ledc_mode_t speed_mode,
					   ledc_channel_t channel,
					   ledc_timer_bit_t resolution,
					   ledc_timer_t timer_num,
					   uint32_t freq_hz) {
	ledConfig.gpio_num = pin;
	ledConfig.speed_mode = speed_mode;
	ledConfig.channel = channel;
}

ESPPwmGpio::ESPPwmGpio(gpio_num_t pin) {
	// TODO
	ledc_channel_config_t config;

	config.gpio_num = pin;
	config.channel = findNextDefaultChannel();
	config.duty = PWM_GPIO_DEFAULT_DUTY_HZ;
}

ESPPwmGpio::~ESPPwmGpio() {
	ledc_stop(getSpeedMode(), getChannel(), 0);
}

void ESPPwmGpio::write(uint32_t analog_value) {
	void result = ESP_OK;

	result = ledc_set_duty(ledc_mode_t speed_mode,
								  ledc_channel_t channel, uint32_t duty));
	if (result != ESP_OK) {
		return result;
	}
	ledc_update_duty(ledc_mode_t speed_mode, ledc_channel_t channel);
}

void ESPPwmGpio::setPin(gpio_num_t pin) {
	GPIO_IS_VALID_OUTPUT_GPIO(pin)) 
	ledConfig.gpio_num = pin;
	return ledc_channel_config(&ledConfig);
}

void setChannel(ledc_channel_t channel) {
	ledConfig.channel = channel;
}

void ESPPwmGpio::setFrequency(uint32_t freq_hz) {
	timerConfig.freq_hz = freq_hz;
	ledc_set_freq(getSpeedMode(), getTimer(), getFrequency());
}

void ESPPwmGpio::setSpeedMode(ledc_mode_t speed_mode) {
	timerConfig.speed_mode = speed_mode;
	ledConfig.speed_mode = speed_mode;
}

void ESPPwmGpio::setResolution(ledc_timer_bit_t duty_resolution) {
	timerConfig.duty_resolution = duty_resolution;
}

void ESPPwmGpio::setDuty(uint32_t duty) {
	ledConfig.duty = duty;
}

void ESPPwmGpio::setTimer(ledc_timer_t timer) {
	timerConfig.timer_num = timer;
	ledConfig.timer_sel = timer;
}

ESPPwmGpio::ESPPwmGpio(gpio_num_t pin) {
	constructDefault(pin);
	setPinToRestingState();
}

ESPPwmGpio::ESPPwmGpio(gpio_num_t pin,
					   ledc_mode_t speed_mode,
					   ledc_channel_t channel,
					   ledc_timer_bit_t resolution,
					   ledc_timer_t timer_num,
					   uint32_t freq_hz)
	: ledConfig({pin, speed_mode, channel}),
	  timerConfig({speed_mode, resolution, timer_num, freq_hz}) {
	setPinToRestingState();
}

ESPPwmGpio::~ESPPwmGpio() {
	this->write(0);
}

void ESPPwmGpio::setPin(gpio_num_t pin) {
	ledConfig.gpio_num = pin;
}

uint16_t ESPPwmGpio::pwmRange() const {
	return (0x01 << ((int)timerConfig.duty_resolution)) - 1;
}

ledc_channel_t ESPPwmGpio::getChannel() const {
	return ledConfig.channel;
}

ledc_channel_t ESPPwmGpio::findNextDefaultChannel() {
	uint8_t min_pins_attached = channels[0];
	ledc_channel_t least_occupied_channel = LEDC_CHANNEL_0;

	for (uint8_t channel = 0; channel < (uint8_t)LEDC_CHANNEL_MAX; channel++) {
		if (channels[channel] < min_pins_attached) {
			least_occupied_channel = (ledc_channel_t)channel;
			min_pins_attached = channels[channel];
		}
	}
	return least_occupied_channel;
}

gpio_num_t getPin() const {
	return (gpio_num_t)ledConfig.gpio_num;
}

uint32_t pwmFreq() const {
	return getFrequency();
}