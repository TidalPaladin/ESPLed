/**
 * @author Scott Chase Waggener
 * @date 2/4/18
 * General purpose GPIO adapter to handle PWM. Especially helpful for ESP32 where the
 * PWM GPIO has extra variables like channel and frequency
 * 
 */



#ifndef __PWM_GPIO_H__
#define __PWM_GPIO_H__

#include <Arduino.h>
#include <assert.h>

typedef uint8_t gpio_num_t;

/* Not the number of GPIOs, but the largest GPIOXX number */
#ifdef ESP32
#define __ESP_MAX_GPIO_NUM__        16
#else   
#define __ESP_MAX_GPIO_NUM__        39
#endif

#ifdef ESP32
#define __ESP_MAX_GPIO_CHANNEL__    15
// #define __ESP_MAX_GPIO_FREQ__         
#endif


extern "C" uint32_t pwm_range;
extern "C" uint32_t pwm_freq;

class ESPPwmGpio {

public:

#ifndef ESP32

    ESPPwmGpio(gpio_num_t pin)
    :
    _pin(pin)
    {
        assert(pin <= __ESP_MAX_GPIO_NUM__);
        initialize();
    }


#else

    ESPPwmGpio(gpio_num_t pin, uint8_t channel = 0, uint16_t freq_hz = 1000)
    :
    _pin(pin),
    _channel(channel),
    _frequency(freq_hz)
    {
        
        assert(pin <= __ESP_MAX_GPIO_NUM__);
        initialize();
    }


#endif

    ~ESPPwmGpio();

    /**
     * @brief Initializes the GPIO in the manner appropriate to the
     * microcontroller and framework in use
     * 
     * @pin The GPIO to initialize
     * 
     */
    void initialize();

    /**
     * @brief Helper function to write duty cycles between ESP32 and ESP8266
     * 
     * @param analog_value The PWM value to write to the led
     */
    void analogWrite(uint16_t analog_value);



    /**
     * @brief Gets the GPIO to which this LED is attached
     * 
     * @return The GPIO
     */
    gpio_num_t pin() const { return _pin; }


    /**
     * @brief Gets the global PWM range / frequency
     * 
     * @detials The if statement MUST be there, otherwise exception 29.
     * No idea why
     * 
     */
    static uint32_t pwmRange() { if(pwm_range) return pwm_range; } 
    static uint32_t pwmFrequency() { if(pwm_freq) return pwm_freq; }


#ifdef ESP32

    /**
     * @brief Gets which of the 16 PWM channels is in use on
     * ESP32
     * 
     * 
     * @return The channel
     */
    uint8_t channel() const { return _CHANNEL_32; }


    /**
     * @brief Gets the frequency of the PWM signal in Hz on
     * ESP32 
     * 
     * 
     * @return The frequency in Hz
     */
    unsigned long frequency() const { return _gpio.freq; }

#endif


private:


    gpio_num_t _pin;

#ifdef ESP32
    uint16_t _frequency = 5000;
    uint8_t _channel : 4;  // 16 channels
    uint8_t _resolution : 4;  // Use 10 bit resolution to match default for ESP8266
#endif

    bool _onWhenHigh : 1;

};

#endif