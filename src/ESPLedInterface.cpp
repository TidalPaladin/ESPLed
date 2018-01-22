#include "ESPLedInterface.h"

void ESPLedInterface::start() {
    if( _started ) return;

#ifdef ESP32
    
    xTaskCreate(
        _loop,    // Function
        "Led Task",     // Name
        1000,           // Stack size in words
        (void*)this,    // Parameter
        1,              // Task priority
        &_taskHandle    // Task handle
    );
  
#else

    // _tick.attach_ms(_tickInterval, [this](){
    //     this->_loop();
    // });
    _tick.attach_ms(_tickInterval_ms, _sISR, (void*)this);
    
#endif

}

void ESPLedInterface::stop(){

#ifdef ESP32
  vTaskDelete(_taskHandle);
#else
  _tick.detach();
#endif

}


