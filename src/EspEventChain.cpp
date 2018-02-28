#include "ESPEventChain.h"

EspEventChain::EspEventChain(size_t num_events)
{
	_events.reserve(num_events);
}

unsigned long EspEvent::runEvent() const {
	if(!_callback) return 0;

	const unsigned long START = millis();
	(_callback)();
	return millis() - START;
}

EspEvent &EspEvent::setTime(unsigned long ms) {
	_time_ms = ms;
	return *this;
}

EspEvent &EspEvent::setCallback(callback_t &callback) {
	_callback = callback;
	return *this;
}

unsigned long EspEventChain::totalTime() const {
	return totalTimeBefore( numEvents()-1 ) + getTimeOf(numEvents()-1);  
}

unsigned long EspEventChain::totalTimeBefore(size_t index) const {

	if( index >= numEvents() ) 
		panic();
	else if(index == 0)
		return 0;

	unsigned long total = 0;
	
	for(int pos = 0; pos < index; pos++){
		total += getTimeOf(pos);
	}
	return total;
} 

unsigned long EspEventChain::getTimeOf(size_t pos) const {
	if( pos >= numEvents() ) panic();

	return _events.at(pos).getTime();
}

int EspEventChain::getPositionFromHandle(const char* handle) const {
	citerator_t target_pos = getIteratorFromHandle(handle);
	if( target_pos != _events.cend() ) 
		return std::distance(_events.cbegin(), target_pos);
	else
		return -1;
}

EspEventChain::citerator_t EspEventChain::getIteratorFromHandle(const char* handle) const {
	citerator_t result = _events.cend();

	if(handle == nullptr || !strcmp(handle, "null"))
		return result;

	for(auto it = _events.cbegin(); it != _events.cend(); it++) {
		if( !strcmp(handle, it->getHandle())) 
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

	// Stop if we cant find another valid event
	if( !validCurrentEvent() )
		stop();
}

unsigned long EspEventChain::scheduleNextEvent(unsigned long offset_ms) {
	advanceToNextCallable();
	unsigned long delay = _currentEvent->getTime();


	// Handle time=0 events that should be run immediately, no scheduling
	while(delay == 0) {
		_currentEvent->runEvent();
		advanceToNextCallable();
		delay = _currentEvent->getTime();
	}


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