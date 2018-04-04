#include "PwmGpio.h"

void ESPPwmGpioBase::constructDefault(gpio_num_t pin) {
	ledc_channel_config_t ledConfig;
	ledConfig.gpio_num = pin;
	ledConfig.channel = findNextDefaultChannel();
	ledConfig.speed_mode = LEDC_LOW_SPEED_MODE;
	ledConfig.duty = 0;
	ledConfig.timer_sel = LEDC_TIMER_0;

	ledc_timer_config_t timerConfig;
	timerConfig.speed_mode = LEDC_LOW_SPEED_MODE;
	timerConfig.freq_hz = 1000;
	timerConfig.duty_resolution = LEDC_TIMER_10_BIT;
	timerConfig.timer_num = LEDC_TIMER_0;

	this->ledConfig = ledConfig;
	this->timerConfig = timerConfig;
}