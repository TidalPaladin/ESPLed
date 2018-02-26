#include "ESPBlink.h"
#include "ESPLedInterface.h"
#include "ESPLed.h"

ESPBlink &ESPBlink::interval(unsigned long ms) {
  if(ms == 0) panic();

  const size_t INDEX_TURN_OFF = 1;
  _changeTimeOf(INDEX_TURN_OFF, ms);
  return *this;   
}

ESPBlink &ESPBlink::duration(unsigned long ms) {
  if(ms == 0) panic();

  const size_t INDEX_TURN_ON = 0;
  _changeTimeOf(INDEX_TURN_ON, ms);
  return *this;
}


