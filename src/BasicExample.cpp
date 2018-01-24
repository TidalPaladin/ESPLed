/*
    This demonstrates the core functions of ESPLed.
    This project is still under development

    Scott Chase Waggener
    tidalpaladin@gmail.com
    10/11/17

*/



#include <Arduino.h>
#include "ESPLed.h"

/*
Create a LED on pin ESP_BUILTIN
ESP_BUILTIN is off when the GPIO is HIGH, so we set the resting state to HIGH
By default, the resting state is LOW
*/
ESPLed led(ESP_BUILTIN, HIGH);

/*
Create strategy objects for blinking and pulsing. Strategy objects allow you to
describe behaviors for an ESPLed, like blinking and pulsing and how those actions
should be timed. You can then switch between these behaviors as demonstrated later on
*/
ESPBlink blink;
ESPBlink blinkFast;
ESPPulse pulse;

/*
// For ESP32 an additional Led channel parameter is required
// Behavior is currently undefined if you use the same channel on multiple pins
Led led(LED_BUILTIN, 0, INVERTED);
*/


void setup(){
    Serial.begin(115200);

    
    /* ESPLed can be controlled manually without the use of a strategy */
    Serial.println("Demo of on/off");
    led.on();                           // Turn on to the current max brightness
    delay(3000);
    led.off();
    delay(1000);
    led.on(30);                         // Turn on to 30% brightness

    /* You can set limits on max and min brightness */
    Serial.println("Demo of max brightness");
    led.maxBrightness(50);              // Don't get brighter than 50% max power
    led.on();                           // Now LED turns on to 50% brightness
    delay(2000);                        // Max brightness also applies to strategies
    led.maxBrightness(100);

    /* Demo of the ESPPulse strategy */
    Serial.println("Demo of pulse");
    pulse.period(1000);                 // 1000ms for a full pulse to complete
    pulse.refreshRate(50);              // Set the refresh rate to 50 Hz (default = 60 Hz)   
    led.setMode(pulse);                 // Tell ESPLed to use the behaviors from ESPPulse instance pulse
    led.start();                        // Tell ESPLed to start acting on the behaviors from ESPPulse instance pulse
    delay(5000);   

    /* Demo of ESPBlink strategy */
    Serial.println("Demo of blink");
    blink.duration(300);                // Blink on for 300 ms
    blink.interval(2000);               // Wait 2000 ms between blinks
    led.setMode(blink).start();        // Set mode to blink and start blinking
    delay(5000);

    /* You can have multiple instances of a strategy and switch between them */
    blinkFast.duration(300);
    blinkFast.interval(300);
    led.setMode(blinkFast).start();
    delay(5000);

    // If you change strategy while a strategy is running, ESPLed returns to the stopped state
    led.setMode(blink);
    led.start();
    delay(5000);

    /* You can manually override a strategy by calling stop() and start() */
    Serial.println("Demo of manual override");
    led.stop();                                 // Stop the strategy
    led.on();                                   // Start doing our manual actions
    delay(2000);
    led.toggle();                              
    delay(1000);
    led.start();                                // Restart the last used strategy
    delay(5000);

    // Other features
    Serial.println("Demo of other features");
    led.minBrightness(10);                      // The Led's off state will now be 10% brightness
    led.isOn();                                 // Returns true if Led is on

}

void loop(){}