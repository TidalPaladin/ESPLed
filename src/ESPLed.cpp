#include "ESPLed.h"


// Antilog percent to [0,1023] lookup table
const uint16_t _brightnessLut[101] PROGMEM = {
  0x000,	0x002,	0x004,	0x007,	0x009,
  0x00b,	0x00e,	0x010,	0x012,	0x015,
  0x017,	0x01a,	0x01c,	0x01f,	0x021,
  0x024,	0x026,	0x029,	0x02c,	0x02e,
  0x031,	0x034,	0x036,	0x039,	0x03c,
  0x03f,	0x042,	0x045,	0x048,	0x04b,
  0x04e,	0x051,	0x054,	0x058,	0x05b,
  0x05e,	0x062,	0x065,	0x069,	0x06c,
  0x070,	0x073,	0x077,	0x07b,	0x07f,
  0x083,	0x087,	0x08b,	0x08f,	0x093,
  0x097,	0x09c,	0x0a0,	0x0a5,	0x0aa,
  0x0ae,	0x0b3,	0x0b8,	0x0bd,	0x0c2,
  0x0c8,	0x0cd,	0x0d3,	0x0d9,	0x0df,
  0x0e5,	0x0eb,	0x0f1,	0x0f8,	0x0ff,
  0x106,	0x10d,	0x115,	0x11c,	0x124,
  0x12d,	0x135,	0x13f,	0x148,	0x152,
  0x15c,	0x167,	0x172,	0x17e,	0x18b,
  0x198,	0x1a7,	0x1b6,	0x1c6,	0x1d8,
  0x1eb,	0x201,	0x218,	0x232,	0x250,
  0x272,	0x29a,	0x2cc,	0x30b,	0x365,
  0x3ff,	
};

// Sine lookup table on [0,pi/2]
#define SINE_STEPS  101
const float _sineLut[SINE_STEPS] PROGMEM = {
  0.0000000,	0.0157073,	0.0314108,	0.0471065,	0.0627905,
  0.0784591,	0.0941083,	0.1097343,	0.1253332,	0.1409012,
  0.1564345,	0.1719291,	0.1873813,	0.2027873,	0.2181432,
  0.2334454,	0.2486899,	0.2638730,	0.2789911,	0.2940403,
  0.3090170,	0.3239174,	0.3387379,	0.3534748,	0.3681246,
  0.3826834,	0.3971479,	0.4115144,	0.4257793,	0.4399392,
  0.4539905,	0.4679298,	0.4817537,	0.4954587,	0.5090414,
  0.5224986,	0.5358268,	0.5490228,	0.5620834,	0.5750053,
  0.5877853,	0.6004202,	0.6129071,	0.6252427,	0.6374240,
  0.6494480,	0.6613119,	0.6730125,	0.6845471,	0.6959128,
  0.7071068,	0.7181263,	0.7289686,	0.7396311,	0.7501111,
  0.7604060,	0.7705132,	0.7804304,	0.7901550,	0.7996847,
  0.8090170,	0.8181497,	0.8270806,	0.8358074,	0.8443279,
  0.8526402,	0.8607420,	0.8686315,	0.8763067,	0.8837656,
  0.8910065,	0.8980276,	0.9048271,	0.9114033,	0.9177546,
  0.9238795,	0.9297765,	0.9354440,	0.9408808,	0.9460854,
  0.9510565,	0.9557930,	0.9602937,	0.9645574,	0.9685832,
  0.9723699,	0.9759168,	0.9792228,	0.9822873,	0.9851093,
  0.9876883,	0.9900237,	0.9921147,	0.9939610,	0.9955620,
  0.9969173,	0.9980267,	0.9988899,	0.9995066,	0.9998766,
  1.0000000	
};

Led::Led() {

}


#ifdef ESP32
Led::Led(uint8_t pin, uint8_t channel, led_style_t style){
  setChannel(channel);
  setPin(pin);
  setStyle(style);
}
#else
Led::Led(uint8_t pin, led_style_t style) {
  setPin(pin);
  setStyle(style);
}
#endif

Led::~Led() {
  stop();
  manual(); // Clean up interface
  setMinBrightness(0);
  off();
}





/*
  Sets the pin to use for this Led
  @params
    Gpio pin to use
  @returns
    void	
*/
Led &Led::setPin(uint8_t pin) {
  _gpio.pin = pin;
  
#ifdef ESP32
  ledcAttachPin(_gpio.pin, _gpio.ledChannel);
#else
  pinMode(getPin(), OUTPUT);
#endif

  off();
  return *this;
}

Led &Led::setStyle(led_style_t style){
  _gpio.style = style;
  return *this;
}


/*
  Functions to set various brightnesses
  @param
    uint8_t percent -> percent from 0-100
  @returns
    uint16_t -> the value that will be sent to analogWrite
*/
Led &Led::setMaxBrightness(uint8_t percent) {
  _brightness.max = percent;
  return *this;
}

Led &Led::setMinBrightness(uint8_t percent){
  _brightness.min = percent;
}

Led &Led::manual(){
  if(_strategy == nullptr) return *this;

  _strategy->stop();
  delete _strategy;
  _strategy = nullptr;
  return *this;
}

#ifdef ESP32
Led &Led::setChannel(uint8_t chan){
  _gpio.ledChannel = chan;
  ledcSetup(_gpio.ledChannel, _gpio.freq, _gpio.resolution);
  return *this;
}

Led &Led::setFrequency(unsigned long hz){
  _gpio.freq = hz;
  return *this;
}
#endif




/*
  Sets the speed for LED pusling
  @params
    Period of a pulse in ms
  @returns
    The pulse step size in radians
*/
Led &Led::setPeriod(unsigned long ms){
  // Remember period is [0,2pi] because wave is offset to always be positive
  _step_rads = TWO_PI / ms * hzToMs( getRefreshRate() );
  return *this;
}

Led &Led::setTheta(float radians){
  if(radians >= TWO_PI) { radians = 0; }
  radians = constrain(radians, 0, TWO_PI);
  _theta_rads = radians;
  return *this;
}

Led &Led::setDeltaTheta(float radians){
  _step_rads = abs(radians);
  return *this;
}

Led &Led::setRefreshRate(unsigned int hz){
  _refreshRate_hz = hz;
  return *this;
}

Led &Led::pulse(){
  if(_strategy != nullptr) {
    _strategy->stop(); 
    delete _strategy; 
  }
  off();
  _strategy = new Pulse(*this);
  return *this;
}









/*
  Sets the speed for LED blinking
  @params
    The time in ms
  @returns
    void	
*/
Led &Led::setInterval(unsigned long ms) {
  _interval_ms = ms;
  return *this;
}

Led &Led::setDuration(unsigned long ms) {
  _duration_ms = ms;
  return *this;
}

Led &Led::blink(){
  if(_strategy != nullptr) { 
    _strategy->stop();
    delete _strategy; 
  }
  off();
  _strategy = new Blink(*this);
  return *this;
}


unsigned long Led::getPeriod() {
  const float stepsPerPeriod = TWO_PI / getDeltaTheta();
  return stepsPerPeriod / getRefreshRate() * 1000;  // seconds to ms
}







/*
  Function to turn the led on or off
  @param
    uint16_t brightness -> How bright the LED will turn on to
  @returns
    void
*/
Led &Led::on(){
  return on( getMaxBrightness() );
}

Led &Led::on(uint8_t percent) {
  _isOn = true;
  percent = constrain(percent, getMinBrightness(), getMaxBrightness());
#ifdef ESP32
  ledcWrite(getChannel(), _mapToAnalog(percent));
#else
  analogWrite( getPin(), _mapToAnalog(percent) );
#endif
  return *this;
}

Led &Led::off() {
  _isOn = false;
#ifdef ESP32
  ledcWrite( getChannel(), _mapToAnalog(getMinBrightness()) );
#else
  analogWrite( getPin(), _mapToAnalog(getMinBrightness()) );
#endif
  return *this;
}

Led &Led::toggle(){
  return toggle(getMaxBrightness());
}

Led &Led::toggle(uint8_t power) { 
  isOn() ? off() : on(power); 
  return *this;
} 

/*
  Function to sync the actions of two Leds
  Does not adjust timings, just makes them start at the same time by calling setMode()
  @params
    Pointer to the Led object to sync to
  @returns
    void	
*/
// void Led::sync(Led &master){
//   this->stop();
//   master.stop();

// }



Led &Led::start() {
  if(_strategy != nullptr){
    _strategy->start();
  }
  return *this;
}

Led &Led::stop() {
  if(_strategy != nullptr){
    _strategy->stop();
  }
  return *this;
}





uint16_t Led::_mapToAnalog(uint8_t percent){
  const uint16_t ret = pgm_read_word(_brightnessLut + percent);
  return (getStyle() == REG) ? ret : PWMRANGE - ret;
}

float Pulse::_mapToSine(float theta){

  int index = 0;
  int sign = 1;
  const int elements = SINE_STEPS - 1;

  /*
    Use a sine lookup table from [0,pi/2] to find sine on
    [0,2pi]
  */

  // Quadrant 3
  if(theta >= 3 * HALF_PI && theta < TWO_PI){
    index = -1 * elements;
    sign = -1;
  }
  // Quadrant 2
  else if(theta > PI) {
    sign = -1;
  }
  // Quadrant 1
  else if(theta > HALF_PI ) {
    index = -1 * elements;
  }

  index = abs(index + int(elements * theta / HALF_PI) % elements);
  float ret = sign * pgm_read_float_near(_sineLut + index);

  return ret; 
}








void Blink::_setupTimer(){
#ifdef ESP32
  
  // Create a new task
  xTaskCreate(
    _tickerWrap,    // Function
    "Led Task",     // Name
    1000,           // Stack size in words
    (void*)this,    // Parameter
    1,              // Task priority
    &_taskHandle    // Task handle
  );

#else
  _handle(); 
#endif
}

void Pulse::_setupTimer(){
  #ifdef ESP32
    
  xTaskCreate(
    _tickerWrap,    // Function
    "Led Task",     // Name
    1000,           // Stack size in words
    (void*)this,    // Parameter
    1,              // Task priority
    &_taskHandle    // Task handle
  );
  
  #else
    _handle();
  #endif
  }


void LedInterface::stop(){
#ifdef ESP32
  Serial.println("Deleting task");
  vTaskDelete(_taskHandle);
#else
  _tick.detach();
#endif
}










/*
  Functions to blink/pulse the Led
  @params
    void
  @returns
    void	
*/

unsigned long Blink::_handle() {
  _led->toggle();
  const unsigned long waitTime = _led->isOn() ? _led->getDuration() : _led->getInterval(); 

#ifndef ESP32
  _tick.once_ms(waitTime, _tickerWrap, (void *)this);
#endif

  return waitTime;
}

void Blink::_tickerWrap(void *ptr){
  Blink *self = (Blink *)ptr;

#ifdef ESP32
  TickType_t xLastWakeTime = xTaskGetTickCount(); // Initialize for delayUntil
  
  while(true){
    const TickType_t xFrequency = self->_handle() / portTICK_PERIOD_MS;
    vTaskDelayUntil(&xLastWakeTime, xFrequency);
  }
#else
  self->_handle();
#endif
}










unsigned long Pulse::_handle() {

  const float sine_old = _mapToSine(_led->getTheta());
  _led->setTheta(_led->getTheta() + _led->getDeltaTheta());
  const float sine = _mapToSine(_led->getTheta());

  
  // Calculate amplitude and offset for a sine wave between max and min
  const uint8_t offset = (_led->getMaxBrightness() + _led->getMinBrightness()) / 2;
  const uint8_t amplitude = _led->getMaxBrightness() - offset;

  if( uint8_t(amplitude * sine_old) != uint8_t(amplitude * sine)) {

    // Increment theta by delta theta
    // Constraint to [0, 2PI) happens in setTheta()
    
  
    // Calculate brightness using a sine wave
    
    const uint8_t outputWave = amplitude * sine + offset;

    // And turn the LED on using the percent brightness
    _led->on(outputWave);
  }

  // Schedule next update
#ifndef ESP32
  _tick.once_ms(hzToMs(_led->getRefreshRate()), _tickerWrap, (void *)this);
#endif
  return _led->getRefreshRate();
}

void Pulse::_tickerWrap(void* ptr){
  Pulse *self = (Pulse *)ptr;
#ifdef ESP32
  TickType_t xLastWakeTime = xTaskGetTickCount();
  
  while(true){
    const TickType_t xFrequency = self->_handle() / portTICK_PERIOD_MS;
    vTaskDelayUntil(&xLastWakeTime, xFrequency);
  }
#else
  self->_handle();
#endif
}


