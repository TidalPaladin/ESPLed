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

    /**
     * @brief Constructor for a given pin
     * 
     * @param pin           The GPIO pin number to attach this object to
     *                      0 <= pin <= (max GPIO number)
     * 
     * @param onWhenHigh    true if analogWrite(pwmRange()) should set the GPIO to HIGH
     *                      false if -- should set the GPIO to LOW
     * 
     * @note    Set onWhenHigh to false to invert the duty cycle. Ex: a duty of 30% with
     *          onWhenHigh == true becomes a duty of 100-30 = 70%
     * 
     * post: GPIO 'pin' set to OUTPUT mode, GPIO 'pin' in LOW state
     */
    ESPPwmGpio(gpio_num_t pin, bool onWhenHigh = true)
    :
    _pin(pin),
    _onWhenHigh(onWhenHigh)
    {
        assert(pin <= __ESP_MAX_GPIO_NUM__);
        initialize();
    }


#else

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
    :
    _pin(pin),
    _channel(channel),
    _frequency(freq_hz)
    {
        
        assert(pin <= __ESP_MAX_GPIO_NUM__);
        initialize();
    }


#endif

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
     *          signal frequency = pwmFrequency, signal duty = analog_value / pwmRange()
     * 
     */
    void analogWrite(uint16_t analog_value);



    /**
     * @brief Gets the GPIO to which this LED is attached
     * 
     * @return The GPIO
     */
    gpio_num_t pin() const { return _pin; }

    /**
     * @brief Gets whether the written duty cycle is the inverse of what is supplied
     * 
     * @return value of onWhenHigh
     */
    ESPPwmGpio onWhenHigh(bool value) { _onWhenhigh = value; return *this; }
    bool onWhenHigh() const { return _onWhenHigh; }


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
     * @return The channel, 0 <= channel() <= 15
     */
    uint8_t channel() const { return _CHANNEL_32; }


    /**
     * @brief Gets the frequency of the PWM signal in Hz on
     * ESP32 
     * 
     * 
     * @return The frequency in Hz, frequency() > 0
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