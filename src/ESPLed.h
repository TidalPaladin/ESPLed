/*
  ESPLed.h
  Scott Chase Waggener  tidalpaladin@gmail.com
  10/10/2017

  This library simplifies the use of Leds on the Arduino framework
  Designed on ESP8266

*/

#ifndef __ESPLED_H__
#define __ESPLED_H__

#include <Arduino.h>

/*
  Ticking handled using RTOS tasks for ESP32
*/
#ifndef ESP32
#include <Ticker.h>
#endif


class ESPLed;
class ESPLedInterface;


/**
 * Provides access to pwm_range
 * TODO Can these be class members?
 */

extern uint32_t pwm_range;
extern uint32_t pwm_freq;

/**
 * Provides pin definitions 
 */
#define NODEMCU_LED_BUILTIN   D0  // NodeMCU led
#define ESP_BUILTIN           2   // The led on ESP12 / ESP32


typedef uint8_t gpio_num_t;

class ESPLed {
public:

  // Constructors
  ESPLed();

#ifdef ESP32

  ESPLed(gpio_num_t pin, uint8_t channel = 0, uint16_t freq_hz = 1000, uint8_t off_state = LOW);

#else

  /**
   * @brief Constructor for ESP8266
   * 
   * @param pin The GPIO to use for the ESPLed
   * @param resting_state The GPIO state when the ESPLed is off
   * 
   */
  ESPLed(gpio_num_t pin, uint8_t off_state = LOW);

#endif

  ~ESPLed();

  /**
   * @brief Gets the GPIO to which this LED is attached
   * 
   * @return The GPIO
   */
  gpio_num_t pin() const { return _PIN; }


  /**
   * @brief Sets the maximum LED brightness as a percentage
   * 
   * @param percent The percentage from 0-100
   * 
   * @return this
   */
  ESPLed &maxBrightness(uint8_t percent);
  uint8_t maxBrightness() const { return _brightness.max; }

  /**
   * @brief Sets the minimum LED brightness as a percentage. This
   * is the brightness that the LED will be set to when off() is called.
   * 
   * @param percent The percentage from 0-100
   * 
   * @return this
   */
  ESPLed &minBrightness(uint8_t percent);
  uint8_t minBrightness() const { return _brightness.min; }

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

  /**
   * @brief Assigns a strategy to the ESPLed
   * 
   * @param strategy A reference to a strategy to use
   * 
   * @return this
   */
  ESPLed &setMode(ESPLedInterface &strategy);


  /**
   * @brief Starts or stops the LED's action for a given mode
   * 
   * @return this
   */
  ESPLed &start();
  ESPLed &stop();
  

  /**
   * @brief Gets the state of the LED
   * 
   * @return
   *   - true if the LED is on
   *   - false otherwise
   */
  bool isOn() const { return _isOn; }
  operator bool() const { return _isOn; }


  /**
   * @brief Turns the LED on to a given brightness
   * 
   * @param percent The brigness to turn on to
   * 
   * @return this
   */
  ESPLed &on(uint8_t percent = 100);

  /**
   * @brief Turns the LED off
   * 
   * @return this
   */
  ESPLed &off();

  /**
   * @brief Toggles the LED to the specified brightness
   * 
   * @param percent The percent brightness from 1-100
   * 
   * @return this
   */
  ESPLed &toggle(uint8_t percent = 100);


  static uint32_t pwmRange() { return pwm_range; }
  static uint32_t pwmFreq() { return pwm_freq; }


protected:

  /**
   * @brief Maps a brightness from a percentage to a PWM value. 
   * 
   * @details Includes compensation for the antilog nature of brightness.
   * Conversion is done using a lookup table.
   * 
   * @param percent The brightness from 0-100
   * 
   * @return The PWM value
   */
  uint16_t _mapToAnalog(uint8_t percent);


  /**
   * @brief Initializes the GPIO in the manner appropriate to the
   * microcontroller and framework in use
   * 
   * @pin The GPIO to initialize
   * 
   */
  static void _initPin(gpio_num_t pin);

protected:

  const gpio_num_t _PIN;
  const uint8_t _RESTING : 1;

#ifdef ESP32
  const uint16_t _FREQUENCY_32 = 5000;
  const uint8_t _CHANNEL_32 : 4;  // 16 channels
  const uint8_t resolution : 4;  // Use 10 bit resolution to match default for ESP8266
#endif

  static const uint16_t _brightnessLut[101];

  struct {
    uint8_t max : 7; // The analogWrite() maximum
    uint8_t min : 7;
  } _brightness;

  ESPLedInterface *_strategy;
  bool _isOn : 1;

};

#endif



/* Include files for the strategy pattern */
#ifndef __ESPLED_INTERFACE_H__

#include "ESPLedInterface.h"
#include "ESPPulse.h"
#include "ESPBlink.h"

#endif