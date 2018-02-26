#ifndef __ESP_BLINK_H__
#define __ESP_BLINK_H__

#include "ESPLedInterface.h"


class ESPBlink : public ESPLedInterface {

public:

  ESPBlink()
  :
  ESPLedInterface(2)  // 2 events for blinking
  {

    _addEvent(2000, [this]() {
      this->turnOn();
    });

    _addEvent(300, [this]() {
      this->turnOff();
    });
  }

  /**
   * @brief Sets the interval between the beginning of subsequent blinks.
   * 
   * @param ms The time in milliseconds between blinks
   * @pre ms > 0
   * 
   * @return this
   */
  ESPBlink &interval(unsigned long ms);
  unsigned long interval() const { return _eventChain.getTimeOf(0); }
  
  /**
   * @brief Sets how long the LED stays on for during a blink
   * 
   * @param ms The time in milliseconds
   * @pre 0 < ms < interval()
   * 
   * @return this
   */
  ESPBlink &duration(unsigned long ms);
  unsigned long duration() const { return _eventChain.getTimeOf(1); }





protected:

  void turnOn() {
    _forEachLed( [](ESPLed *led) {
      led->on();
    });
  }

  void turnOff() {
    _forEachLed( [](ESPLed *led) {
      led->off();
    });
  }


};

#endif