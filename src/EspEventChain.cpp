#include "ESPEventChain.h"

unsigned long EspEvent::runEvent() const {
    if(!_callback) return 0;

    const unsigned long START = millis();
    (_callback)();
    return millis() - START;
}

EspEvent &EspEvent::setTime(unsigned long ms) {
    if( ms == 0) panic();
    _time_ms = ms;
    return *this;
}

EspEventChain &EspEventChain::timeBetweenChainRepeats(unsigned long ms) {

    if(ms <= 0) panic();

    _timeBetweenRepeats_ms = ms;
    return *this;
}

EspEventChain &EspEventChain::absoluteTimeBetweenChainRepeats(unsigned long ms) {
    if(ms <= 0) panic();

    _timeBetweenRepeats_ms = ms;
    return *this;
}

unsigned long EspEventChain::totalTime() const {
    return totalTimeBefore( numEvents() );  
}

unsigned long EspEventChain::totalTimeBefore(size_t index) const {

    citerator_t begin = _events.cbegin();
    citerator_t end = _events.cbegin();
    std::advance(end, index);
    
    
    return std::accumulate(
        begin,
        end,
        0,
        [](unsigned long a, EspEvent b){
            return a + b.getTime();
        }
    );

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
    pChain->handleTick();

#ifdef ESP32
    preventTaskEnd();
#endif

}

void EspEventChain::handleTick() {
    _currentEvent->runEvent();
    scheduleNextEvent();
}

void EspEventChain::scheduleNextEvent() {
    _currentEvent++;
    if( !validCurrentEvent() ) {
        reset();
    }

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

   _tick.once_ms(_currentEvent->getTime(), sHandleTick, (void*)this);

#endif   
}




#ifdef ESP32
void EspEventChain::preventTaskEnd() {

    TickType_t xLastWakeTime = xTaskGetTickCount(); // Initialize for delayUntil

    while(true){
        const TickType_t xFrequency = _tickInterval_ms / portTICK_PERIOD_MS;
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }

}
#endif





void EspEventChain::changeTimeOf(size_t pos, unsigned long ms) {
    if( pos >= numEvents() ) panic();
    _events.at(pos).setTime(ms);
}