/*
  Led.h
  Scott Chase Waggener  tidalpaladin@gmail.com
  9/16/2017

  This library simplifies the use of Leds on the Arduino framework
  Designed on ESP8266

*/

#ifndef LED_H
#define LED_H

#include <Arduino.h>
#include <Ticker.h>

#define NODEMCU_BUILTIN D0
#define ESP_BULITIN     2

#define hzToMs(hz) (1000/hz)
#define msToHz(ms) (1000/ms)



/*
  IDEAS
  Finish adding RGB led functionality
  Allow sync of Leds with a phase shift (ie destructively interfering waves)
*/

typedef enum LED_PULSE_SPEEDS { SLOW = 0, MEDIUM = 1, FAST = 5 } led_pulse_t;
typedef enum LED_STYLES { REG, INVERTED, RGB } led_style_t;
typedef enum LED_MODES { BLINK, PULSE, MANUAL, SOS } led_mode_t;
typedef enum LED_COLORS {RED, ORANGE, YELLOW, GREEN, BLUE, WHITE} led_colors_t;

class Led;
class LedInterface;
class Pulse;
class Blink;
class ColorLed;

class Led {
public:

  

  // Constructors
  Led();
#ifdef ESP32
  Led(uint8_t pin, uint8_t channel, led_style_t style = REG);
#else
  Led(uint8_t pin, led_style_t style = REG);
#endif
  ~Led();

  

  /*
    Setters General
  */

  // Sets the LED pin
  Led &setPin(uint8_t pin);

  // LED_REG -> HIGH = on
  // LED_INVERTED -> LOW = off
  Led &setStyle(led_style_t style);
 
  // Sets maximum brightness as a percent [0,100]
  Led &setMaxBrightness(uint8_t percent);

  // Sets mainimum brightness as a percent [0,100]
  Led &setMinBrightness(uint8_t percent);

  // Returns the LED to manual control
  Led &manual();

#ifdef ESP32
  // Set which of the 16 PWM channels to use
  Led &setChannel(uint8_t channel);

  // Set the frequency of the PWM signal in Hz
  Led &setFrequency(unsigned long hz);
#endif


  /*
    Setters Pulse
  */

  // Sets the time in ms for one pulse period
  Led &setPeriod(unsigned long ms);
  
  // Sets the theta value used to calculate brightness
  Led &setTheta(float radians);

  // Sets the increment in theta, how fast pulsing happens
  Led &setDeltaTheta(float radians);

  // Sets how many steps theta will take per second
  Led &setRefreshRate(unsigned int hz);

  // Puts the Led in pulse mode
  Led &pulse();




  /*
    Setters Blink
  */

  // Sets the interval between blinks
  Led &setInterval(unsigned long ms);
  
  // Sets how long the LED stays on for during a blink
  Led &setDuration(unsigned long ms);
  
  // Puts the Led in blink mode
  Led &blink();
  
  


  /*
    Getters General
  */

  // Returns the pin in use
  const uint8_t getPin() { return _gpio.pin; }

  // Returns the LED style (LED_INVERTED / LED_REG)
  led_style_t getStyle(){ return _gpio.style; }

  // Returns true if the LED is on
  const bool isOn() { return _isOn; }

  // Returns whether the led is in MANUAL, BLINK, or PULSE mode
  virtual led_mode_t getMode();

  // Returns the maximum brightness as a percent [0,100]
  uint8_t getMaxBrightness() { return _brightness.max; }

  // Returns the brightness corresponding to the off state as a percent [0,100]
  uint8_t getMinBrightness() { return _brightness.min; }

#ifdef ESP32
  // Returns the PWM channel for this Led
  uint8_t getChannel() { return _gpio.ledChannel; }

  // Gets the PWM frequency in Hz
  unsigned long getFrequency() { return _gpio.freq; }
#endif



  /*
    Getters Pulse
  */

  // Uses theta to calculate a period for a single pulse
  unsigned long getPeriod();
  
  // Gets the theta value in radians used to calculate brightness
  float getTheta() { return _theta_rads; }

  // Gets the incremental change in theta in radians
  float getDeltaTheta(){ return _step_rads; };

  // Gets the rate in Hz at which theta is incremented
  unsigned int getRefreshRate() { return _refreshRate_hz; }



  /*
    Getters Blink
  */

  // Gets the interval between blinks in ms
  unsigned long getInterval() { return _interval_ms; }

  // Gets the duration of a blink in ms
  unsigned long getDuration() { return _duration_ms;}




  /*
    Actions
  */

  // Turns the LED on to a given brightness
  Led &on(uint8_t percent);

  // Turns the LED on to max brightness
  Led &on();

  // Turns the LED off
  Led &off();

  // Toggles the LED to the specified power level as a percent [0,100]
  Led &toggle(uint8_t percent);

  // Toggles the LED using max/min brightness
  Led &toggle();


  void sync(Led &master);
  //void _pulse();

  // Action wrappers
  static void wrap(void (Led:: *funcPtr)(void));
  static void toggleWrap(void *ptr);


  virtual Led &start();
  virtual Led &stop();
 

protected:

  struct {
    uint8_t pin;
    led_style_t style = INVERTED;

#ifdef ESP32
    unsigned int freq = 5000;
    uint8_t ledChannel = 0;
    const uint8_t resolution = 10;  // Use 10 bit resolution to match default for ESP8266
 #endif
  } _gpio;


  struct {
    uint8_t max = 100; // The analogWrite() maximum
    uint8_t min = 0;
  } _brightness;

  LedInterface *_strategy;
  bool _isOn;

  // Maps a power level in percent to an analogWrite value
  // Value will be adjusted for Led Style
  uint16_t _mapToAnalog(uint8_t percent);
  
  

  // f(x) = maxAnalog * Exp[x / maxPercent]

  
  /*
    Pulse variables
  */
  unsigned int _refreshRate_hz = 60;   // Refresh rate for pulse mode
  float _theta_rads = PI;
  float _step_rads = PI;

  /*
    Blink variables
  */
  unsigned long _interval_ms = 3000;     // Interval on which to Pulse/blink
  unsigned long _duration_ms = 300;      // How long the led stays lit during a blink

  virtual void _handle() { }
  


private:

};

/*
  Interface to handle scheduled things like pulsing / blinking
*/
class LedInterface {
public:
  LedInterface(){}

  virtual void start() { this->_handle(); }
  virtual void stop() { _tick.detach(); }
  
  virtual led_mode_t getMode() const = 0;

protected:
  virtual void _handle() = 0;
  Ticker _tick;

};



class Blink : public LedInterface {
public:

  Blink(Led &led) : _led(led){ }
  
  led_mode_t getMode() const { return BLINK; }

private:
  
  void _handle();
  static void _tickerWrap(void*);
  Led &_led;

};

class Pulse : public LedInterface {
public:

  Pulse(Led &led) : _led(led){ }
 
  led_mode_t getMode() const { return PULSE; }

private:

  void _handle();
  static void _tickerWrap(void*);
  float _mapToSine(float theta);
  Led &_led;

};


// NYI
class ColorLed : public Led {

public:
  ColorLed &setColor(unsigned int, uint8_t, uint8_t, uint8_t);

};

#endif
