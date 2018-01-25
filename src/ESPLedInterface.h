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
   * @brief Change the Ticking interval. This is the interval on which calls
   * are made to _handleLed() for each attached ESPLed.
   * 
   * @param ms The new interval in milliseconds
   * 
   * @return this
   */
  ESPLedInterface &tickInterval(unsigned long ms) { _tickInterval_ms = ms; return *this; }
  unsigned long tickInterval() const { return _tickInterval_ms; }

  /**
   * @brief Gets how many ESPLed objects are attached to this strategy
   * 
   * @return The number of attached ESPLed objects
   */
  size_t attachedLedCount() const { return _leds.size(); }

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
   * @brief Calls _handleLed() for each ESPLed in _leds
   * 
   * @details You can override this in derived classes if you
   * need to add behaviors before the call to _handleLed(). Just make
   * sure that you call ESPLedInterface::_loop() at the end of your overload
   * 
   * 
   */
  virtual void _loop();

  /**
   * @brief This is called from _loop() and applies the periodic
   * action to an individual led.
   * 
   * @details _loop() should call this for each led in 
   * led_container_t. Here is where you put the code that
   * describes the Led's action
   * 
   * @param led The led to act on
   */
  virtual void _handleLed(ESPLed *const led) = 0;


  /**
   * @brief Checks an ESPLed pointer parameter for null pointer
   * 
   * @param led The pointer to check
   */
  static inline bool _checkLedPointer(ESPLed *const led);

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
   * @brief Threads on ESP32 must not return. This prevents the thread from
   * returning.
   * 
   * @details Runs an infinite loop until the thread is ready to end
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
