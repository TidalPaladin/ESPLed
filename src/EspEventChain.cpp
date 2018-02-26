#include "ESPEventChain.h"

EspEventChain::EspEventChain(size_t num_events)
:
_events(num_events) // Reserve vector space
{

}

unsigned long EspEvent::runEvent() const {
    if(!_callback) return 0;

    const unsigned long START = millis();
    (_callback)();
    return millis() - START;
}

EspEvent &EspEvent::setTime(unsigned long ms) {
    if( ms == 0) panic();
    Serial.println("Changed time REAL");
    _time_ms = ms;
    return *this;
}

EspEvent &EspEvent::setCallback(callback_t &callback) {
    _callback = callback;
    return *this;
}

unsigned long EspEventChain::totalTime() const {
    return totalTimeBefore( numEvents() );  
}

unsigned long EspEventChain::totalTimeBefore(size_t index) const {
    if( index == 0 || index >= numEvents() ) panic();

    index--;
    unsigned long total = 0;
    
    while(index >= 0) {
        total += getTimeOf(index);
    }
    return total;
} 

unsigned long EspEventChain::getTimeOf(size_t pos) const {
    if( pos >= numEvents() ) panic();

    return _events.at(pos).getTime();
}

int EspEventChain::getPositionFromHandle(const char* handle) {
    citerator_t target_pos = getIteratorFromHandle(handle);
    if( target_pos != container.cend() ) 
        return std::distance(container.cbegin(), target_pos);
    else
        return -1;
}

citerator_t EspEventChain::getIteratorFromHandle(const char* handle) const {
    citerator_t result = container.cend();

    if(handle == nullptr || !strcmp(handle, "null"))
        return result;

    for(auto it = container.cBegin(); it != container.cEnd(); it++) {
        if( !strcmp(handle, event.getHandle())) 
            result = it;
    }
    return result;
}

void EspEventChain::start() {
    reset();
    handleTick();
    _started = true;
}

void EspEventChain::stop() {

#ifdef ESP32

    vTaskDelete(_taskHandle);

#else

    _tick.detach();

#endif

    _started = false;
    
}

void EspEventChain::sHandleTick(void *ptr) {
    EspEventChain *pChain = (EspEventChain*)ptr;
    if(pChain == nullptr) panic();

    pChain->handleTick();



}

void EspEventChain::handleTick() {
    unsigned long time_for_callback = _currentEvent->runEvent();
    unsigned long delay = scheduleNextEvent(time_for_callback);

#ifdef ESP32
    preventTaskEnd(delay);
#endif
}


void EspEventChain::advanceToNextCallable() {

    const citerator_t INITIAL_POS = _currentEvent++;
    if( atEndOfChain() ) { reset(); }

    while( !validCurrentEvent() && !atEndOfChain() ) {
        _currentEvent++;
    }

    if( !validCurrentEvent() )
        stop();
}

unsigned long EspEventChain::scheduleNextEvent(unsigned long offset_ms) {
    advanceToNextCallable();
    unsigned long delay = _currentEvent->getTime();

    delay -= ( offset_ms <= delay ? offset_ms : delay); 

#ifdef ESP32
    
    xTaskCreate(
        sHandleTick,    // Function
        "EventChain",     // Name
        1000,           // Stack size in words
        (void*)this,    // Parameter
        1,              // Task priority
        &_taskHandle    // Task handle
    );
  
#else

   _tick.once_ms(delay, sHandleTick, (void*)this);

#endif   

    return delay;
}


#ifdef ESP32
void EspEventChain::preventTaskEnd(unsigned long howLong_ms) {

    TickType_t xLastWakeTime = xTaskGetTickCount(); // Initialize for delayUntil

    while(true){
        const TickType_t xFrequency = howLong_ms / portTICK_PERIOD_MS;
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }

}
#endif





void EspEventChain::changeTimeOf(size_t pos, unsigned long ms) {
    if( pos >= numEvents() ) panic();
    _events.at(pos).setTime(ms);
}