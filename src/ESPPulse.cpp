#include "ESPPulse.h"
#include "ESPLedInterface.h"
#include "ESPLed.h"


ESPPulse &ESPPulse::setPeriod(unsigned long ms){
  if(ms == 0)
    panic();

  _step_rads = getDeltaThetaFromPeriod(ms);
  return *this;
}

unsigned long ESPPulse::getPeriod() const {
  return getPeriodFromDeltaTheta(_step_rads);
}


ESPPulse &ESPPulse::setRefreshRate(unsigned int hz){
  if(hz == 0)
    panic();
  else if( _eventChain.numEvents() < 1)
    Serial.println("Panic: tried to set refresh rate but the event chain was too short");

  const unsigned long ms = hzToMilliseconds(hz);
  _eventChain.changeTimeOf(__ESP_PULSE_REFRESH_RATE_INDEX__, ms);
  return *this;
}

unsigned long ESPPulse::getRefreshRate() const { 
  if( _eventChain.numEvents() < 1)
    Serial.println("Panic: tried to get refresh rate but the event chain was too short");

  const uint32_t MS_BETWEEN_REFRESHES = _eventChain.getTimeOf(__ESP_PULSE_REFRESH_RATE_INDEX__); 
  return hzToMilliseconds(MS_BETWEEN_REFRESHES);
}


void ESPPulse::setPulseToPeak() {
  _theta_rads = PI / 2;
}


void ESPPulse::setPulseToTrough() {
  _theta_rads = 3 * PI / 2;
}










float ESPPulse::getDeltaThetaFromPeriod(unsigned long period_ms) const {
  if(period_ms == 0)
    panic();
  
  // Remember period is [0,2pi] because wave is offset to always be positive
  return TWO_PI / period_ms * hzToMilliseconds( getRefreshRate() );
}

unsigned long ESPPulse::getPeriodFromDeltaTheta(float delta_theta_rads) const {
  if(delta_theta_rads == 0)
    panic();

  return round(TWO_PI / _step_rads * hzToMilliseconds( getRefreshRate() ));
}


void ESPPulse::construct(unsigned long refresh_rate_hz, unsigned long period_ms) {
  if(refresh_rate_hz == 0 || period_ms == 0)
    panic();


  /*
    Event 1 = update theta, t = hzToMilliseconds(refreshRate)
    Event 2 = forEachLed write brightness, t = 0
  */

  const uint32_t FIRST_EVENT_TIME_MS = hzToMilliseconds(refresh_rate_hz);
  const uint8_t SECOND_EVENT_TIME_MS = 0; // Loop through all Leds immediately after advancing theta

  _addEvent(FIRST_EVENT_TIME_MS, [this]() {
    this->_currentSine = calculateNewSineValue();
  });

  _addEventEveryLed(SECOND_EVENT_TIME_MS, [this](ESPLed *led) {
    const uint8_t BRIGHTNESS = calculateNewBrightness(*led);
    led->on(BRIGHTNESS);
  });

  // This must come after events are added
  _step_rads = getDeltaThetaFromPeriod(period_ms);
}

uint8_t ESPPulse::calculateNewBrightness(const ESPLed &led) const {

  /* Calculate A and O for f(x) = A * sin(x) + O */
  const uint8_t OFFSET = (led.maxBrightness() + led.minBrightness()) / 2;
  const uint8_t AMPLITUDE = led.maxBrightness() - OFFSET;

  /* Find the value of f(x) = A * sin(x) + O */
  const uint8_t PERCENT_BRIGHTNESS = AMPLITUDE * _currentSine + OFFSET;

  Serial.printf("Theta: %f, Offset: %i, Ampitude %i, Sine %f, Percent %i\n", _theta_rads, OFFSET, AMPLITUDE, _currentSine, PERCENT_BRIGHTNESS);
  return PERCENT_BRIGHTNESS;

}

float ESPPulse::calculateNewSineValue() {

  /* Increment theta by the step size */
  _theta_rads += _step_rads;

  /* Make sure theta stays bound by 0, 2pi */
  if(_theta_rads > TWO_PI) {
    _theta_rads = 0;
  }

  /* Calculate the sine of the new theta */
  const float NEW_SINE = _sin(_theta_rads);
  return NEW_SINE;
}

float ESPPulse::_sin(float theta){

    /* Will sine be positive or negative */
    const int8_t sign = (theta > PI) ? -1 : 1;

    /* Is theta in quadrant 2 or 4 */

    //const bool QUAD_2_4 = (theta > HALF_PI) ^ (theta > 3 * HALF_PI / 2);
    const bool QUAD_2_4 = 
      (theta > HALF_PI && theta < PI)
      || (theta > 3/2 * HALF_PI && theta < TWO_PI);

    
    /* How far from beginning or end of table to read */
    const uint8_t OFFSET = (SINE_STEPS - 1) * theta / HALF_PI;

    /* Find correct index in the table */
    /* Read from reverse if in quadrant 2 or 4 */
    const uint8_t INDEX = QUAD_2_4 ? SINE_STEPS - OFFSET : OFFSET;
    
    return sign * pgm_read_float_near(_sineLut + INDEX);

}
