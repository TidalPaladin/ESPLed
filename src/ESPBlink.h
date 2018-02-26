#ifndef __ESP_BLINK_H__
#define __ESP_BLINK_H__

#include "ESPLedInterface.h"


class ESPBlink : public ESPLedInterface {

public:

  /**
   * @brief Default constructor
   * 
   * post: duration() == 300, interval() == 2000, isStarted() == false
   * 
   */
  ESPBlink()
  :
  ESPLedInterface(2)  // 2 events for blinking
  {
    ESPBlink(2000, 300);
  }

  /**
   * @brief Constructor with custom duration and interval
   * 
   * @param interval  The time in milliseconds between blinks
   *                  0 < interval
   * 
   * @param duration  The time in milliseconds for the LED to stay on when it blinks
   *                  0 < duration
   * 
   * post: duration() == duration, interval() == interval, isStarted() == false
   */
  ESPBlink(unsigned long interval_ms, unsigned long duration_ms)
  :
  ESPLedInterface(2)  // 2 events for blinking
  {

    _addEvent(interval_ms, [](EspLed &led) {
      led.on();
    });

    _addEvent(duration_ms, [](EspLed &led) {
      led.off();
    });
    
  }

  /**
   * @brief Sets the interval between the beginning of subsequent blinks.
   * 
   * @param ms  The time in milliseconds between blinks
   *            0 < ms
   * 
   * @return this
   */
  ESPBlink &interval(unsigned long ms);
  unsigned long interval() const { return _eventChain.getTimeOf(0); }
  
  /**
   * @brief Sets how long the LED stays on for during a blink
   * 
   * @param ms  The time in milliseconds
   *            0 < ms
   * 
   * @return this
   */
  ESPBlink &duration(unsigned long ms);
  unsigned long duration() const { return _eventChain.getTimeOf(1); }


protected:

  /**
   * @brief Turns on each ESPLed attached to this object
   * 
   * post: isOn() == true for every attached LED
   */
  void turnOn() {
    _forEachLed( [](ESPLed *led) {
      led->on();
    });
  }

  /**
   * @brief Turns off each ESPLed attached to this object
   * 
   * post: isOn() == false for every attached LED
   */
  void turnOff() {
    _forEachLed( [](ESPLed *led) {
      led->off();
    });
  }

};

#endif