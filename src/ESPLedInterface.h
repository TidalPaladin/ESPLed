/**
 * ESPLedInterface.h
 * Scott Chase Waggener
 * 1/23/18
 * 
 * ESPLedInterface provides a strategy pattern interface for ESPLed. You can
 * create custom strategies for ESPLed by creating classes derived from 
 * ESPLedInterface. Overload virtual function _loop() to carry out whatever
 * periodic action you want for your strategy. See ESPBlink.h and ESPPulse.h
 * for examples
 * 
 */


#ifndef __ESPLED_INTERFACE_H__
#define __ESPLED_INTERFACE_H__

#include "ESPLed.h"
#include "ESPEventChain.h"
#include <vector>
#include <algorithm>
#include <Arduino.h>


class ESPLedInterface {

protected:

	typedef std::vector<ESPLed*> led_container_t;
	typedef std::function<void(typename led_container_t::value_type)> foreach_t;

public:

	/**
	 * @brief Empty constructor
	 * 
	 * TODO can this go?
	 */
	ESPLedInterface(size_t num_events) 
	:
	_eventChain(num_events)
	{ 
		
	}

	/**
	 * @brief Perfect forwarding constructor to EspEventChain
	 * 
	 * @param args  Constructor arguments for EspEventChain
	 * 
	 */
	template<typename... Args>
	ESPLedInterface(Args... args)
	: 
	_eventChain(args...)
	{

	}
 
	/**
	 * @brief Destructor. Makes sure that periodic actions are stopped
	 * cleanly before destroying object.
	 * 
	 * post: All attached LEDs stopped at the brightness they had at time of call
	 * 
	 */
	~ESPLedInterface() {
		stopAll();
	}


	/**
	 * @brief Gets how many ESPLed objects are attached to this strategy
	 * 
	 * @return  The number of attached ESPLed objects
	 *          0 <= attachedLedCount()
	 */
	size_t getAttachedLedCount() const;

	/**
	 * @brief Start the periodic action for this strategy
	 * 
	 * @details Creates a scheduled periodic call to _loop()
	 * 
	 * @param led   The ESPLed object
	 * 
	 * post: led.isStarted() == true, led acting
	 * 
	 */
	void start(ESPLed &led);

	/**
	 * @brief Stop acting on a given led by removing it from the interface
	 * 
	 * 
	 * @param led   The ESPLed to stop acting on and remove from interface
	 * 
	 * 
	 * post:  led.isStarted() == false, led stopped at brightness it had at time of call
	 *        led removed from this interface and its last known state forgotten. If led was
	 *        not attached to begin with, nothing happens
	 * 
	 */
	void stop(ESPLed &led);


	/**
	 * @brief Stop all actions
	 * 
	 * post:  stop() called for every led attached to the interface
	 *        No leds attached to the interface
	 * 
	 */
	void stopAll();


	/**
	 * @brief Pauses all Leds, they still continue to be attached to this interface
	 * 
	 */
	void pauseAll();
	void resumeAll();


	/**
	 * @brief Detects whether the interface is running
	 * 
	 * @return
	 *   - true if running
	 *   - false otherwise
	 */
	bool isStarted() const;

public:

	/**
	 * @brief Converts a frequency in Hz to a period in milliseconds
	 * 
	 * @param hz The frequency in hertz
	 * 
	 * @return The period of the wave in milliseconds
	 */
	template <typename T>
	static constexpr T hzToMilliseconds(T hz) { 
		return hz ? 1000 / (double)hz : 0; 
	}

	/**
	 * @brief Converts a period in milliseconds to a frequency in Hz
	 * 
	 * @param ms The period of the wave in milliseconds
	 * 
	 * @return The frequency of the wave in hertz
	 */
	template <typename T>
	static constexpr T millisecondsToHz(T ms) { 
		return hzToMilliseconds(ms);  // The equations are identical
	}


protected:

	/**
	 * @brief Adds an event to this interface by forwarding args to EspEventChain. The given
	 * callback accepts an ESPLed pointer and will be called on every attached Led
	 * 
	 * @note  Mostly used in the constructor to build the event chain that describes
	 *        the behavior of the strategy
	 * 
	 * @param ms_relative_to_previous The time in milliseconds for the event to run relative
	 *                                to the event that preceeds it in the event chain
	 * 
	 * @param for_each  The callback to run for every attached led per tick
	 * 
	 * post: events.numEvents() = old numEvents() + 1
	 * 
	 */
	size_t _addEventEveryLed(unsigned long ms_relative_to_previous, std::function<void(ESPLed *)> for_each);


	/**
	 * @brief Adds an event to this interface by forwarding args to EspEventChain. The given
	 * callback accepts and returns nothing, and will be called once per tick. See above method
	 * for something that will be called on every LED when a tick occurrs
	 * 
	 * @note  Mostly used in the constructor to build the event chain that describes
	 *        the behavior of the strategy
	 * 
	 * @param ms_relative_to_previous The time in milliseconds for the event to run relative
	 *                                to the event that preceeds it in the event chain
	 * 
	 * @param run_once  The callback to run once per tick
	 * 
	 * post: events.numEvents() = old numEvents() + 1
	 * 
	 * @return The number of events in the chain, _events.numEvents()
	 */
	size_t _addEvent(unsigned long ms_relative_to_previous, EspEventChain::callback_t run_once);



	/**
	 * @brief Forwards arguments to EspEventChain::changeTimeOf()
	 * 
	 * @param args  The arguments to be forwarded
	 * 
	 */
	template<typename... Args>
	void _changeTimeOf(Args... args) {
		_eventChain.changeTimeOf(args...);
	}




	/**
	 * @brief Checks an ESPLed pointer parameter for null pointer
	 * 
	 * @param led The pointer to check
	 */
	static inline bool _checkLedPointer(ESPLed *const led);

 
protected:

	led_container_t _leds;
	EspEventChain _eventChain;

};



#endif
