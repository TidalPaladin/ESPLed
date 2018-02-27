#include "ESPLedInterface.h"



void ESPLedInterface::start(ESPLed &led) {

    /* Add a reference to the new ESPLed to our list */
    _leds.push_back(&led);

    /* If we arent ticking, start */
    if( !isStarted()) { 
        _eventChain.start();
    }

}

void ESPLedInterface::stop(ESPLed &led){

    /* Find the ESPLed that wants to be stopped */
    auto i = std::find(_leds.begin(), _leds.end(), &led);
    if( i != _leds.cend() ) {
        _leds.erase(i);      // Remove it from the list
    }

    /* If the list is now empty, we can stop ticking */
    if( _leds.empty() ) { _eventChain.stop(); }

}

void ESPLedInterface::stopAll() {
    _eventChain.stop();
    _leds.clear();
}

void ESPLedInterface::pauseAll() {
    if( isStarted() )
        _eventChain.stop();
}

void ESPLedInterface::resumeAll() {
    if( !isStarted() )
        _eventChain.start();
}




size_t ESPLedInterface::_addEventEveryLed(unsigned long time_ms, std::function<void(ESPLed *)> f) {

    EspEventChain::callback_t forEach = [&, this]() {
        for(auto led : this->_leds) {
            if(led != nullptr) {
                f(led); 
            }
        }
    };
    
    return _addEvent(time_ms, forEach);
}

size_t ESPLedInterface::_addEvent(unsigned long time_ms, EspEventChain::callback_t function) {
    EspEvent newEvent(time_ms, function);

    // Add the function that will act on all LEDS to the event chain
    return _eventChain.addEvent(newEvent);
}



bool ESPLedInterface::_checkLedPointer(ESPLed *const led) {

    if(led != nullptr) return true;

    return false;

}







