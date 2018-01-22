#ifndef __ESP_BLINK_H__
#define __ESP_BLINK_H__

#ifndef __LED_INTERFACE_H__
#include "ESPLedInterface.h"
#endif

class ESPBlink : public ESPLedInterface {

public:

  ESPBlink(ESPLed *led)
  : 
  ESPLedInterface(led, 100)
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
  unsigned long interval() const { return _interval_ms; }
  
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
  
  virtual void _loop();

private:

  unsigned long _interval_ms = 3000;     // Interval on which to Pulse/blink
  unsigned long _duration_ms = 300;      // How long the led stays lit during a blink

  Ticker _t;

};

#endif