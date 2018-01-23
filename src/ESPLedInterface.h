/**
 * ESPLedInterface.h
 * Scott Chase Waggener
 * 1/23/18
 * 
 * ESPLedInterface provides a strategy pattern interface for ESPLed. You can
 * create custom strategies for ESPLed by creating classes derived from 
 * ESPLedInterface. Overload virtual function _loop() to carry out whatever
 * periodic action you want for your strategy. See ESPBlink.h and ESPPulse.h
 * for examples
 * 
 */


#ifndef __LED_INTERFACE_H__
#define __LED_INTERFACE_H__

#include <Arduino.h>
#include <Ticker.h>
#include <vector>

#ifndef __ESP_LED_H__
class ESPLed;
#endif

class ESPLedInterface {

public:


  ESPLedInterface(ESPLed *led, unsigned long tick_interval_ms)
  : 
  _led(led),
  _tickInterval_ms(tick_interval_ms),
  _started(false)
  {

  }
 
  /**
   * @brief Destructor. Makes sure that periodic actions are stopped
   * cleanly before destroying object.
   * 
   */
  ~ESPLedInterface() {
    stop();
  }

  /**
   * @brief Start the periodic action for this strategy
   * 
   * @details Creates a scheduled periodic call to _loop()
   * 
   * 
   */
  virtual void start();

  /**
   * @brief Stop the periodic action for this strategy
   * 
   */
  virtual void stop();


  /**
   * @brief Detects whether the interface is running
   * 
   * @return
   *   - true if running
   *   - false otherwise
   */
  bool isStarted() const { return _started; }

public:

  /**
   * @brief Converts a frequency in Hz to a period in milliseconds
   * 
   * @param hz The frequency in hertz
   * 
   * @return The period of the wave in milliseconds
   */
  template <typename T>
  static inline T hzToMilliseconds(T hz) final { return hz ? 1000 / hz : 0; }

  /**
   * @brief Converts a period in milliseconds to a frequency in Hz
   * 
   * @param ms The period of the wave in milliseconds
   * 
   * @return The frequency of the wave in hertz
   */
  template <typename T>
  static inline T millisecondsToHz(T ms) final { return ms ? 1000 / ms : 0; }


protected:

  /**
   * @brief Static function so Ticker can call member _loop().
   * This can be eliminated once Ticker accepts lambda functions
   * 
   */
  static void _sISR(void *obj) final {
    ESPLedInterface *ptr = (ESPLedInterface *) obj;
    ptr->_loop();
  }

  /**
   * @brief This is called perodically and handles what needs
   * to be done to the LED. Override this for each strategy.
   * 
   */
  virtual void _loop() = 0;  



 
protected:

  ESPLed *_led;                          // The LED to be acted on 
  const unsigned long _tickInterval_ms;  // The interval at which to loop
  bool _started : 1;

#ifdef ESP32
  TaskHandle_t _taskHandle = NULL;
#else
  Ticker _tick;
#endif

};

//#include "ESPLed.h"

#endif
