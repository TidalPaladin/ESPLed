/**
 * 
 * 
 *  Debug helper for ESPLed
 * 
 */
#ifndef __ESP_LED_DEBUG__
#define __ESP_LED_DEBUG__

#include <Arduino.h>


#define __ESP_LED_DEBUG_TAG__	"*EspLed"

#ifndef __ESP_LED_DEBUG_SRC__
#define __ESP_LED_DEBUG_SRC__	Serial
#endif

 

class ESPLedDebug {

friend class ESPLed;
friend class ESPLedInterface;

private:



    		/**
		 * @brief Checks if a position is a valid index of the events container
		 * 
		 * post: if event_num is invalid, an error is printed and panic() is called
		 * 
		 */
		inline void checkValidEventNum(uint32_t event_num, const char *file, int line, const char* function) const {
			if(event_num >= _events.size() || event_num < 0) {
				printErr(file, line, function, "Invalid event number - %i", event_num);
				panic();
			}
		}

		/**
		 * @brief Checks if a time is valid
		 * 
		 * post: if time_ms is invalid, an error is printed and panic() is called
		 * 
		 */
		inline void checkValidTime(uint32_t time_ms, const char *file, int line, const char* function) const {
			if( time_ms < 0 ) {
				printErr(file, line, function, "Invalid time - %i", time_ms);
				panic();
			}
		}

		/**
		 * @brief Checks if a pointer is not null
		 * 
		 * post: if ptr is null, an error is printed and panic() is called
		 * 
		 */
		template <typename T>
		static inline void checkValidPtr(T ptr, const char *file, int line, const char* function) {
			static_assert(sizeof(T) <= sizeof(uint32_t*), "target must be a native pointer");
			if(ptr == nullptr) {
				printErr(file, line, function, "Tried to operate on a null pointer");
				panic();
			}
		}


		template <typename... Args>
		static inline void printErr(const char* file, int line, const char* function, const char* format, Args... args) {
			String msgStr = String("%s - %s:%i %s() - ");
			msgStr += String(format);
			__ESP_EVENT_CHAIN_DEBUG_SRC__.printf(msgStr.c_str(),
				__ESP_EVENT_CHAIN_DEBUG_TAG__,
				file,
				line,
				function,
				format,
				args...
			);
			__ESP_EVENT_CHAIN_DEBUG_SRC__.println();
		}





};









#endif