#include "ESPBlink.h"
#ifdef __ESP_LED_H__

ESPBlink &ESPBlink::interval(unsigned long ms) {
  _interval_ms = ms;
  return *this;   
}

ESPBlink &ESPBlink::duration(unsigned long ms) {
  _duration_ms = ms;
  return *this;
}

void ESPBlink::_loop() {
  _led->on();
  // _t.once_ms(_led.blinkDuration(), [this](){
  //   this->_led->off();
  // });


 #ifdef ESP32
  TickType_t xLastWakeTime = xTaskGetTickCount(); // Initialize for delayUntil
  
  while(true){
    const TickType_t xFrequency = self->_handle() / portTICK_PERIOD_MS;
    vTaskDelayUntil(&xLastWakeTime, xFrequency);
  }
#endif


}




#endif