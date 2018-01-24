#include "ESPLedInterface.h"


void ESPLedInterface::start(ESPLed &led) {

    _leds.push_back(led);

    /* If we arent ticking, start */
    if( !_started) { 
        _startTicking();
    }

}

void ESPLedInterface::stop(ESPLed &led){

    /* Find the ESPLed that wants to be stopped */
    auto i = std::find(_leds.begin(), _leds.end(), led);
    if( i != _leds.cend() ) {
        _leds.erase(i);      // Remove it from the list
    }

    /* If the list is now empty, we can stop ticking */
    if( _leds.empty() ) { _stopTicking(); }

}

void ESPLedInterface::stopAll() {
    _leds.clear();
    _stopTicking();
}



void ESPLedInterface::_startTicking() {

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

    /*  Use this once Ticker can use lambda functions

    _tick.attach_ms(_tickInterval, [this](){
        this->_loop();
    });
    
    */
    _tick.attach_ms(_tickInterval_ms, _sISR, (void*)this);
    
#endif

    _started = true;

}

void ESPLedInterface::_stopTicking() {

#ifdef ESP32
    vTaskDelete(_taskHandle);
#else
    _tick.detach();
#endif

    _started = false;

}



#ifdef ESP32

void ESPLedInterface::_preventTaskEnd() {

    TickType_t xLastWakeTime = xTaskGetTickCount(); // Initialize for delayUntil

    while(true){
        const TickType_t xFrequency = _tickInterval_ms / portTICK_PERIOD_MS;
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}

#endif


