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


#ifndef __ESPLED_INTERFACE_H__
#define __ESPLED_INTERFACE_H__

#include "ESPLed.h"
#include <Ticker.h>
#include <vector>
#include <algorithm>
#include <Arduino.h>


class ESPLedInterface {

public:

  /**
   * @brief Constructor
   * 
   * @param tick_interval_ms The time between calls to _loop() in ms
   * 
   */
  ESPLedInterface(unsigned long tick_interval_ms)
  : 
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
    stopAll();
  }

  /**
   * @brief Gets how many ESPLed objects are attached to this strategy
   * 
   * @return The number of attached ESPLed objects
   */
  size_t attachedLedCount() { return _leds.size(); }

  /**
   * @brief Start the periodic action for this strategy
   * 
   * @details Creates a scheduled periodic call to _loop()
   * 
   * @param led The ESPLed object
   * 
   * 
   */
  void start(ESPLed &led);

  /**
   * @brief Stop the periodic action for this strategy
   * 
   */
  void stop(ESPLed &led);


  /**
   * @brief Stop all actions
   * 
   */
  void stopAll();


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
  static inline T hzToMilliseconds(T hz) { return hz ? 1000 / hz : 0; }

  /**
   * @brief Converts a period in milliseconds to a frequency in Hz
   * 
   * @param ms The period of the wave in milliseconds
   * 
   * @return The frequency of the wave in hertz
   */
  template <typename T>
  static inline T millisecondsToHz(T ms) { return ms ? 1000 / ms : 0; }


protected:

  

  /**
   * @brief Static function so Ticker can call member _loop().
   * This can be eliminated once Ticker accepts lambda functions
   * 
   */
  static void _sISR(void *obj) {
    ESPLedInterface *ptr = (ESPLedInterface *) obj;
    ptr->_loop();
  }


  /**
   * @brief This is called perodically and handles what needs
   * to be done to the LED. Override this for each strategy.
   * 
   */
  virtual void _loop();

  /**
   * @brief This is called from _loop() and applies the periodic
   * action to an individual led.
   * 
   * @details _loop() should call this for each led in 
   * led_container_t
   * 
   */
  static virtual void _handleLed(ESPLed *const led) = 0;




  /**
   * @brief Helper function to start periodic calls to _loop(). Call this
   * when the first element of led_container_t is added.
   * 
   * 
   */
  void _startTicking();


  /**
   * @brief Helper function to stop periodic calls to _loop(). Call this
   * when the last element of led_container_t is removed.
   * 
   * 
   */
  void _stopTicking();


#ifdef ESP32

  /**
   * @brief Stops an ESP32 task from ending
   * 
   */
  void _preventTaskEnd();

#endif


 
protected:

  typedef std::vector<ESPLed*> led_container_t;
  led_container_t _leds;                          // The LED to be acted on 
  
  unsigned long _tickInterval_ms;  // The interval at which to loop
  bool _started : 1;

#ifdef ESP32
  TaskHandle_t _taskHandle = NULL;
#else
  Ticker _tick;
#endif

};



#endif
