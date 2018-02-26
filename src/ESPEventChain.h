/**
 * @file EspEventChain.h
 * @author Scott Chase Waggener tidal@utexas.edu
 * @date 2/2/18
 * 
 * @description
 * Tracks a collection of timings that can be used with <Ticker.h> or another
 * scheduling class to carry out events separated by irregular intervals
 * 
 *
 * 
 */ 

#ifndef __ESP_EVENT_CHAIN_H__
#define __ESP_EVENT_CHAIN_H__

#include <Arduino.h>
#include <vector>
#include <functional>
#include <algorithm>


#ifndef ESP32
#include <Ticker.h>
#endif



class EspEvent {

    public:

        typedef std::function<void()> callback_t;


        EspEvent() : _time_ms(0) { }

        /**
         * @brief Constructor
         * 
         * @param relative_time_ms The ti
         */
        EspEvent(unsigned long relative_time_ms, callback_t event)
        :
        _time_ms(relative_time_ms),
        _callback(event)
        {
            
        }

        /**
         * @brief Tests whether the event stores a callable function
         * 
         * @return true if a callable function is contained, false otherwise
         */
        operator bool() const { return (bool)_callback; }

        /**
         * @brief Sets the time for this event relative to the event that will preceed it in
         * the EspEventChain
         * 
         * @param ms The time in milliseconds, 0 < ms
         * 
         * @return this
         */
        EspEvent &setTime(unsigned long ms);


        /**
         * @brief Sets the time for this event relative to the event that will preceed it in
         * the EspEventChain
         * 
         * @param ms The time in milliseconds, 0 < ms
         * 
         * @return this
         */
        EspEvent &setCallback(callback_t &callback);

        /**
         * @brief Gets the time property for this Event
         * 
         * @return Time in milliseconds, 0 < newTime_ms
         */
        unsigned long getTime() const { return _time_ms; }

        /**
         * @brief Runs the callback for this event
         * 
         * @return The time in milliseconds that the callback took to run
         */
        unsigned long runEvent() const;

    private:

        unsigned long _time_ms;
        callback_t _callback;
        
};

class EspEventChain {

    public:
        
        /**
         * @brief Default constructor, nothing gets initialized
         * 
         */
        EspEventChain() { }

        /**
         * @brief Reserved space constructor, makes room for "num_events" events
         * 
         * @param num_events The expected number of events, 0 <= num_events
         * 
         */
        EspEventChain(size_t num_events);
        

        /**
         * @brief Populate constructor, puts a variable number of event objects into the event chain
         * 
         * @param ...events Comma separated EspEvent objects to put into the chain
         * 
         */
        template<typename... Args>
        EspEventChain(EspEvent e1, Args... events) 
        :
        _events({e1, events...})
        {

        }

        ~EspEventChain() {
            stop();
        }


        


        /**
         * @brief Add an event to the end of the chain
         * 
         * @details Constructs an event at the end of the chain via perfect forwarding of args
         * 
         * @tparam args Constructor arguments for EspEvent
         * 
         * @return numEvents() - 1, ie the position of the new event in the chain
         */
        template<typename... Args>
        size_t addEvent(Args... args) {
            const bool WAS_RUNNING = running();
            stop();
            _events.emplace_back(args...);
            reset();
            if(WAS_RUNNING) start();
            return numEvents();
        }

        /**
         * @brief Add an event to the end of the chain
         * 
         * @param event The EspEvent object to add
         * 
         * @return numEvents() - 1, ie the position of the new event in the chain
         */
        size_t addEvent(EspEvent event) {
            const bool WAS_RUNNING = running();
            stop();
            _events.push_back(event);
            reset();
            if(WAS_RUNNING) start();
            return numEvents();
        }



        /**
         * @brief Gets the number of events in the chain
         * 
         * @return 0 <= numEvents()
         */
        size_t numEvents() const { return _events.size(); }


        /**
         * @brief Change the time associated with the EspEvent at a given index
         * 
         * @param pos The position of the event to alter, 0 <= pos < numEvents()
         * @param newTime_ms The new time in milliseconds
         * 
         */
        void changeTimeOf(size_t pos, unsigned long newTime_ms);


        /**
         * @brief Gets the time for the "pos" event
         * 
         * @param pos The index, 0 <= pos < numEvents()
         * 
         * @return _events.at(pos).getTime()
         */
        unsigned long getTimeOf(size_t pos) const;


        /**
         * @brief Starts the event chain
         * 
         * @post First event called, second event scheduled
         * 
         */
        void start();


        /**
         * @brief Stops the event chain
         * 
         */
        void stop();

        /**
         * @brief Gets whether the event chain is running
         * 
         * @return true if the chain is running, false otherwise
         */
        bool running() const { return _started; }


        /**
         * @brief Gets the time required for the entire event chain to complete. Does not
         * account for the time taken by the callbacks
         * 
         * @return The time in milliseconds, 0 < time
         */
        unsigned long totalTime() const;

        /**
         * @brief Gets the time it will take for the first "index" events to run
         * 
         * @param index The event to sum before, 0 <= index <= numEvents()
         *
         * @return The time in milliseconds, 0 <= time
         */
        unsigned long totalTimeBefore(size_t index) const;

    private:

        typedef std::vector<EspEvent> container_t;
        typedef container_t::const_iterator citerator_t;
        typedef container_t::iterator iterator_t;


        unsigned long _timeBetweenRepeats_ms = 0;
        container_t _events;

        
        citerator_t _currentEvent;
        
        #ifdef ESP32
            TaskHandle_t _taskHandle = NULL;
        #else
            Ticker _tick;
        #endif


        bool _started : 1;

    private:

        


        /**
         * @brief Resets the current event iterator to the beginning of the event chain
         * 
         */
        void reset() { _currentEvent = _events.cbegin(); }


        /**
         * @brief Checks whether the current event is valid
         * 
         * @return true if valid, false otherwise
         */
        bool validCurrentEvent() const { return !atEndOfChain() && *_currentEvent; }

        /**
         * @brief Checks whether we have reached the end of the event chain
         * 
         * @return bool
         */
        bool atEndOfChain() const { return _currentEvent == _events.cend(); }

        /**
         * @brief Advances the current event to the next event in the chain that is callable
         * 
         */
        void advanceToNextCallable();

        /**
         * @brief Handles each tick. Use this until we get std::function for ticker
         * 
         * @param ptr An EspEventChain object pointer cast to void*
         */
        static void sHandleTick(void *ptr);


        /**
         * @brief Member function called from handleTick that triggers the correct event
         * 
         */
        void handleTick();


        /**
         * @brief Schedules the next tick
         * 
         * @param offset_ms A time in milliseconds to deduct from the delay between the next event
         */
        unsigned long scheduleNextEvent(unsigned long offset_ms);


    #ifdef ESP32


        void preventTaskEnd(unsigned long howLong_ms);

    #endif
};

/**
 *  Blink - {3000, 300} = Repeat starts 3000ms after the call to the last element
 *  event1( on(), 3000)
 * event2( off(), 300);
 * 
 * on() <--- 300 ---> off() <--- 3000 ---> on()
 * 
 * call event 1 on start
 * wait
 */


#endif