#include "ESPBlink.h"
#include "ESPLedInterface.h"
#include "ESPLed.h"

const uint8_t ESPBlink::INTERVAL_INDEX = 0;
const uint8_t ESPBlink::DURATION_INDEX = 1; 



ESPBlink &ESPBlink::interval(unsigned long ms) {
  if(ms == 0) panic();

  _changeTimeOf(INTERVAL_INDEX, ms);
  return *this;   
}

ESPBlink &ESPBlink::duration(unsigned long ms) {
  if(ms == 0) panic();

  _changeTimeOf(DURATION_INDEX, ms);
  return *this;
}


void ESPBlink::construct(unsigned long interval_ms, unsigned long duration_ms) {

  /* 
    ESPEvents hold time relative to the preceeding event. 
    So turning on, added at position 0, holds the off time.
    Turning off, added at position 1, holds the on time
  */
  
  _addEvent(interval_ms, [](ESPLed *led) {
    led->on();
  });

  _addEvent(duration_ms, [](ESPLed *led) {
    led->off();
  });

  }

