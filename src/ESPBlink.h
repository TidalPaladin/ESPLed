#ifndef __ESP_BLINK_H__
#define __ESP_BLINK_H__

#include "ESPLedInterface.h"


class ESPBlink : public ESPLedInterface {

public:

  ESPBlink()
  : 
  ESPLedInterface(3000)
  {
    
  }

  /**
   * @brief Sets the interval between blinks
   * 
   * @param ms The time in ms between blinks
   * 
   * @return this
   */
  ESPBlink &interval(unsigned long ms);
  unsigned long interval() const { return tickInterval(); }
  
  /**
   * @brief Sets how long the LED stays on for during a blink
   * 
   * @param ms The time in milliseconds
   * 
   * @return this
   */
  ESPBlink &duration(unsigned long ms);
  unsigned long duration() const { return _duration_ms;}

protected:
  
  virtual void _handleLed(ESPLed *const led);

  /**
   * @brief Call this from _handleLed() to turn off the led
   * 
   */
  static void _sOff(void *blink);

private:

  //unsigned long _interval_ms = 3000;     // Interval on which to Pulse/blink
  unsigned long _duration_ms = 300;      // How long the led stays lit during a blink

  Ticker _blinkTick;

};

#endif