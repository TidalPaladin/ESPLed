#ifndef __LED_INTERFACE_H__
#define __LED_INTERFACE_H__

#include <Arduino.h>
#include <Ticker.h>

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
 
  ~ESPLedInterface() {
    stop();
  }

  /**
   * @brief Start the periodic action for this strategy
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

protected:

  /**
   * @brief Static function so Ticker can call member _loop()
   * 
   */
  static void _sISR(void *obj) {
    ESPLedInterface *ptr = (ESPLedInterface *) obj;
    ptr->_loop();
  }

  /**
   * @brief This is called perodically and handles what needs
   * to be done to the LED
   * 
   */
  virtual void _loop() = 0;  


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

  ESPLed *_led;                       // The LED to be acted on 
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
