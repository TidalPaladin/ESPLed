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
   * @param pin             The GPIO to use for the ESPLed
   *                        0 <= pin <= (8266 / 32 max GPIO number)
   * 
   * @param resting_state   The GPIO state when the ESPLed is off
   *                        resting_state == LOW || resting_state == HIGH
   * 
   */
  ESPLed(gpio_num_t pin, uint8_t off_state = LOW);

#endif

  ~ESPLed();

  /**
   * @brief Gets the GPIO that the ESPLed is attached to
   * 
   * @return GPIO number, 0 <= pin() <= (max GPIO num)
   */
  gpio_num_t pin() const { return _gpio.pin(); }


  /** 
   * @brief Sets the maximum LED brightness as a percentage
   * 
   * @param percent   The brightness as a percentage
   *                  0 <= percent <= 100
   * 
   * @return this
   */
  ESPLed &maxBrightness(uint8_t percent); 
  uint8_t maxBrightness() const { return _brightnessRange.maxBrightnessPercent(); }

  /**
   * @brief Sets the minimum LED brightness as a percentage. This
   * is the brightness that the LED will be set to when off() is called.
   * 
   * @param percent   The brightness as a percentage
   *                  0 <= percent <= 100
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
   * post: start() will cause the ESPLed to behave as instructed by strategy
   * 
   * @return this
   */
  ESPLed &setMode(ESPLedInterface &strategy);


  /**
   * @brief Starts the LED's action for a given mode
   * 
   * pre: setMode() has been called with a valid object
   * post: The LED is taking the periodic action instructed by strategy
   * 
   * @return this
   */
  ESPLed &start();

  /**
   * @brief Stops the LED's action
   * 
   * post:  The LED is not taking a periodic action
   *        LED remains in the on/off state it was in when stop() was called
   * 
   * @return this
   */
  ESPLed &stop();
  
  /**
   * @brief Gets whether the LED is operating under some mode
   * 
   * @return false if start() was called more recently than stop(), true otherwise
   */
  bool active() const;

  /**
   * @brief Gets the state of the LED
   * 
   * @note  If minBrightness() != 0 and off() was called, isOn() == false but the
   *        LED will be emitting light
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
   * @return  true if on() will set the LED to HIGH
   *          false if on() will set the LED to LOW
   */
  bool highIsOn() const { return _highIsOn; }

  /**
   * @brief Turns the LED on to a given brightness
   * 
   * @param percent   The brigness to turn on to
   *                  0 <= percent <= 100
   * 
   * post: The LED is on at 'percent' brightness, isOn() == true
   * 
   * @return this
   */
  ESPLed &on(uint8_t percent = 100);

  /**
   * @brief Turns the LED off
   * 
   * post:  The LED is set to a brightness level of minBrightness()
   *        isOn() == false
   * 
   * @return this
   */
  ESPLed &off();

  /**
   * @brief Toggles the LED between off() and on(percent)
   * 
   * @param percent   The percent brightness
   *                  0 <= percent <= 100
   * 
   * post:  if isOn() == false before call, LED is on to 'percent' brightness, isOn() == true
   *        if isOn() == true before call, LED is set to minBrightness(), isOn() == false
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