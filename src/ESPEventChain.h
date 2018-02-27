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


/**
 * 
 * Data structure representing one event. Holds data about the callback method to run and the tick delay
 * relative to the preceeding event
 * 
 */
class EspEvent {

    public:

        typedef std::function<void()> callback_t;


        EspEvent() : _time_ms(0) { }

        /**
         * @brief Constructor
         * 
         * @param relative_time_ms      The delay in milliseconds between the preceeding event and this event
         *                              0 < relative_time_ms
         * 
         * @param event                 The void() callback to run when the event is triggered
         *                              event takes no parameters and returns void
         * 
         * @param identifying_handle    A text handle to identify this event as part of the chain
         */
        EspEvent(unsigned long relative_time_ms, callback_t event, const char* identifying_handle = "null")
        :
        _time_ms(relative_time_ms),
        _callback(event),
        _HANDLE(identifying_handle)
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
         * @brief Gets the text handle for this event for the purpose of identification
         * 
         * @return "null" if no handle was set, or identifying_handle assigned at construction
         */
        const char* getHandle() const { return _HANDLE; }

        /**
         * @brief Runs the callback for this event
         * 
         * @return The time in milliseconds that the callback took to run
         */
        unsigned long runEvent() const;

    private:

        const char* _HANDLE;
        unsigned long _time_ms;
        callback_t _callback;
        
};


/**
 * 
 * Holds a collection of EspEvents and coordinates the periodic calls to each event's callback
 * 
 * 
 */
class EspEventChain {

    public:

        typedef std::vector<EspEvent> container_t;
        typedef container_t::const_iterator citerator_t;
        typedef container_t::iterator iterator_t;

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
         * @param pos           The position of the event to alter, 0 <= pos < numEvents()
         * @param newTime_ms    The new time in milliseconds, 0 < newTime_ms
         * 
         */
        void changeTimeOf(size_t pos, unsigned long newTime_ms);


        /**
         * @brief Gets the time for the event at a given position
         * 
         * @param pos The index, 0 <= pos < numEvents()
         * 
         * @return _events.at(pos).getTime()
         */
        unsigned long getTimeOf(size_t pos) const;


        /**
         * @brief Attempts to look up an EspEvent in the chain using the identifying handle of the
         * object
         * 
         * @param handle    The handle to look up, handle != null && handle != "null"
         * 
         * @return  The position of the event with getHandle() == handle in the chain if it exists
         *          -1 if no match was found
         */
        int getPositionFromHandle(const char* handle) const;


        /**
         * @brief Attempts to look up an EspEvent in the chain using the identifying handle of the
         * object
         * 
         * @param handle    The handle to look up, handle != null && handle != "null"
         * 
         * @return  A const iterator of the event closest to begin() with getHandle() == handle if it exists
         *          container.cend() if no event was found with that handle
         */
        citerator_t getIteratorFromHandle(const char* handle) const;

        /**
         * @brief Starts the event chain
         * 
         * post:    reset() called, _currentEvent positioned at the first event,
         *          ticker armed to call first event, running() == true
         * 
         */
        void start();


        /**
         * @brief Stops the event chain
         * 
         * post: Ticker disarmed, running() == false
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
         * @brief Runs the chain once from start to finish
         * 
         */
        // TODO implement this!!!

        /**
         * @brief Gets the time required for the entire event chain to complete. Does not
         * account for the time taken by the callbacks
         * 
         * @return  The time in milliseconds for all events to run, 
         *          equivalent to getTime(0, numEvents() - 1)
         */
        unsigned long totalTime() const;

        /**
         * @brief Gets the time it will take for the first "index" events to run
         * 
         * @param index The event to sum before, 0 < index < numEvents()
         *
         * @return Sum of getTimeOf() for events between 0 and index
         */
        unsigned long totalTimeBefore(size_t index) const;

    private:

        // The container of EspEvents and the corresponding iterators
        

        container_t _events;
        citerator_t _currentEvent;

        unsigned long _timeBetweenRepeats_ms = 0;
        
        // The class that actually handles the periodic calls
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
         * post: _currentEvent is positioned at the first event in _events
         *  
         */
        void reset() { _currentEvent = _events.cbegin(); }


        /**
         * @brief Checks whether the current event contains a callable callback
         * 
         * @return true if valid, false otherwise
         */
        bool validCurrentEvent() const { return !atEndOfChain() && *_currentEvent; }

        /**
         * @brief Checks whether _currentEvent is positioned at the end of _events
         * 
         * @return true if _currentEvent == _events.cend();
         */
        bool atEndOfChain() const { return _currentEvent == _events.cend(); }

        /**
         * @brief Advances the current event to the next event in the chain that is callable
         * 
         * post:    _currentEvent > _currentEventOld if the next validCurrentEvent() follows _currentEventOld in the container
         *          _currentEvent =< _currentEventOld if there is no validCurrentEvent() between _currentEventOld and the end
         *              of the container
         */
        void advanceToNextCallable();

        /**
         * @brief Handles each tick. Use this until we get std::function for ticker
         * 
         * @param ptr   An EspEventChain object pointer cast to void*
         *              ptr != null, ptr instanceof EspEventChain
         * 
         * post: ptr cast to EspEventChain, handleTick called on casted object
         */
        static void sHandleTick(void *ptr);


        /**
         * @brief Member function called from handleTick that triggers the correct event
         * 
         * post:    _currentEvent method called, _currentEvent == next valid event in chain,
         *          ticker armed to call _currentEvent
         * 
         */
        void handleTick();


        /**
         * @brief Schedules the next tick
         * 
         * @param offset_ms A time in milliseconds to deduct from the delay between the next event
         * 
         * post: _currentEvent advanced to next valid event, ticker armed to call _currentEvent
         * 
         * @return The time in milliseconds to wait before _currentEvent will be called
         */
        unsigned long scheduleNextEvent(unsigned long offset_ms);


    #ifdef ESP32

        /**
         * @brief Runs an infinite loop for a given amount of time
         * 
         * @note This is required on ESP32 to prevent scheduled tasks from ending
         * 
         * @param howLong_ms    The time in milliseconds for the loop to run,
         *                      0 < howLong_ms
         * post: infinite loop will run for howLong_ms milliseconds
         */
        void preventTaskEnd(unsigned long howLong_ms);

    #endif
};


#endif