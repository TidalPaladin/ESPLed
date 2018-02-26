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
#include <assert.h>


/*
  Ticking handled using RTOS tasks for ESP32
*/
#ifndef ESP32
#include <Ticker.h>
#endif

#include "PwmGpio.h"
#include "ESPBrightnessRange.h"

class ESPLed;
class ESPLedInterface;



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
   * @brief Gets the GPIO of the ESPLed
   * 
   * @return GPIO number
   */
  gpio_num_t pin() const { return _gpio.pin(); }


  /** 
   * @brief Sets the maximum LED brightness as a percentage
   * 
   * @param percent The percentage from 0-100
   * 
   * @return this
   */
  ESPLed &maxBrightness(uint8_t percent); 
  uint8_t maxBrightness() const { return _brightnessRange.maxBrightnessPercent(); }

  /**
   * @brief Sets the minimum LED brightness as a percentage. This
   * is the brightness that the LED will be set to when off() is called.
   * 
   * @param percent The percentage from 0-100
   * 
   * @return this 
   */
  ESPLed &minBrightness(uint8_t percent);
  uint8_t minBrightness() const { return _brightnessRange.minBrightnessPercent(); }



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
   * @brief Gets whether the LED is operating under some mode
   * 
   */
  bool active() const;

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
   * @brief Gets whether the LED is on or off when the GPIO is high
   * 
   * @return true if LED is on when GPIO is high, false otherwise
   */
  bool highIsOn() const { return _highIsOn; }

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

  

protected:


  ESPPwmGpio _gpio;
  bool _highIsOn : 1;
  ESPLedBrightness _brightnessRange;

  ESPLedInterface *_strategy = nullptr;
  bool _isOn : 1;

};

#endif



/* Include files for the strategy pattern */
#ifndef __ESPLED_INTERFACE_H__

#include "ESPLedInterface.h"
#include "ESPPulse.h"
#include "ESPBlink.h"

#endif