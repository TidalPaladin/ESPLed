#include "ESPBlink.h"
#include "ESPLedInterface.h"
#include "ESPLed.h"

const uint8_t ESPBlink::INTERVAL_INDEX = 0;
const uint8_t ESPBlink::DURATION_INDEX = 1; 



ESPBlink &ESPBlink::setInterval(unsigned long ms) {
	if(ms == 0) panic();

	_changeTimeOf(INTERVAL_INDEX, ms);
	return *this;   
}

ESPBlink &ESPBlink::setDuration(unsigned long ms) {
	if(ms == 0) panic();

	_changeTimeOf(DURATION_INDEX, ms);
	return *this;
}

unsigned long ESPBlink::getInterval() const { return _eventChain.getTimeOf(INTERVAL_INDEX); }
unsigned long ESPBlink::getDuration() const { return _eventChain.getTimeOf(DURATION_INDEX); }


void ESPBlink::construct(unsigned long interval_ms, unsigned long duration_ms) {

	/* 
		ESPEvents hold time relative to the preceeding event. 
		So turning on, added at position 0, holds the off time.
		Turning off, added at position 1, holds the on time
	*/
	
	_addEventEveryLed(interval_ms, [](ESPLed *led) {
		led->on();
	});

	_addEventEveryLed(duration_ms, [](ESPLed *led) {
		led->off();
	});

}

