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
#include "ESPEventChain.h"
#include <vector>
#include <algorithm>
#include <Arduino.h>


class ESPLedInterface {

protected:

  typedef std::vector<ESPLed*> led_container_t;
  typedef std::function<void(typename led_container_t::value_type)> foreach_t;

public:


  ESPLedInterface() { }

  /**
   * @brief Perfect forwarding constructor to EspEventChain
   * 
   */
  template<typename... Args>
  ESPLedInterface(Args... args)
  : 
  _eventChain(args...)
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
   * @brief Stop acting on a given led
   * 
   * @param led The ESPLed to stop acting on
   * 
   */
  void stop(ESPLed &led);


  /**
   * @brief Stop all actions
   * 
   */
  void stopAll();


  /**
   * @brief Pauses all Leds, they still continue to be attached to this interface
   * 
   */
  void pauseAll();
  void resumeAll();


  /**
   * @brief Detects whether the interface is running
   * 
   * @return
   *   - true if running
   *   - false otherwise
   */
  bool isStarted() const { return _eventChain.running(); } 

public:

  /**
   * @brief Converts a frequency in Hz to a period in milliseconds
   * 
   * @param hz The frequency in hertz
   * 
   * @return The period of the wave in milliseconds
   */
  template <typename T>
  static constexpr T hzToMilliseconds(T hz) { 
    return hz ? 1000 / hz : 0; 
  }

  /**
   * @brief Converts a period in milliseconds to a frequency in Hz
   * 
   * @param ms The period of the wave in milliseconds
   * 
   * @return The frequency of the wave in hertz
   */
  template <typename T>
  static constexpr T millisecondsToHz(T ms) { 
    return hzToMilliseconds(ms);  // The equations are identical
  }


protected:

  template<typename... Args>
  size_t _addEvent(Args... args) {
    return _eventChain.addEvent(args...);
  }

  
  template<typename... Args>
  void _changeTimeOf(Args... args) {
    Serial.println("Changed time!");
    _eventChain.changeTimeOf(args...);
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
  virtual void _forEachLed(foreach_t f);


  /**
   * @brief Checks an ESPLed pointer parameter for null pointer
   * 
   * @param led The pointer to check
   */
  static inline bool _checkLedPointer(ESPLed *const led);

 
protected:

  led_container_t _leds;
  EspEventChain _eventChain;

};



#endif
