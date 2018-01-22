/*
    This demonstrates the core functions of ESPLed.
    This project is still under development

    Scott Chase Waggener
    tidalpaladin@gmail.com
    10/11/17

*/



#include <Arduino.h>
//#include "ESPBlink.h"
#include "ESPLed.h"


// Create a LED on LED_BUILTIN
// Because LED_BUILTIN is ON when the pin is HIGH, we use INVERTED
// Default led_style_t parameter is REG

ESPLed led(ESP_BUILTIN, HIGH);
// Pins ESP_BUILTIN and NODEMCU_BUILTIN are provided by ESPLed.h


// // For ESP32 an additional Led channel parameter is required
// // Behavior is currently undefined if you use the same channel on multiple pins
// Led led(LED_BUILTIN, 0, INVERTED);


void setup(){
    Serial.begin(115200);

    // Manual control of the LED with on() and off()
    // Brightness is set from 0-100 and is compensated using an antilog lookup table
    Serial.println("Demo of on/off");
    led.on();
    delay(3000);
    led.off();
    delay(1000);
    led.on(30);     // Turn on to 30% brightness

    Serial.println("Demo of max brightness");
    led.maxBrightness(50);
    led.on();                   // Now LED turns on to 50% brightness
    delay(2000);


    // Pulsing LED
    // Serial.println("Demo of pulse");
    // led.maxBrightness(100);
    // led.pulsePeriod(1000);        // 1000ms for a full pulse to complete
    // led.pulseRefreshRate(50);      // Set the refresh rate to 50 Hz (default = 60 Hz)
    // led.pulse().start();        // Set mode to pulse and start
    // delay(5000);

    // // Blinking LED
    // Serial.println("Demo of blink");
    // led.blinkDuration(300);       // Blink on for 300 ms
    // led.blinkInterval(2000);      // Wait 2000 ms between blinks
    // led.blink().start();        // Set mode to blink and start blinking
    // delay(5000);

    // Manual override
    Serial.println("Demo of manual override");
    led.stop();                 // Blinking stops, mode is preserved
    led.on();
    delay(2000);
    led.toggle();               // Toggles the LED
    // led.toggle(40);          // Toggle to on at 40% power or to off
    delay(1000);
    led.start();                // Resume blinking
    delay(5000);

    // Other features
    Serial.println("Demo of other features");
    led.minBrightness(10);   // The Led's off state will now be 10% brightness
    led.isOn();                 // Returns true if Led is on

}

void loop(){}