#include "ESPPulse.h"
#include "ESPLedInterface.h"
#include "ESPLed.h"


ESPPulse &ESPPulse::period(unsigned long ms){
  if(ms == 0)
    panic();

  _step_rads = getDeltaThetaFromPeriod(ms);
  return *this;
}

unsigned long ESPPulse::period() const {
  return getPeriodFromDeltaTheta(_step_rads);
}


ESPPulse &ESPPulse::refreshRate(unsigned int hz){
  if(hz == 0)
    panic();
  else if( _eventChain.numEvents() < 1)
    Serial.println("Panic: tried to set refresh rate but the event chain was too short");

  const unsigned long ms = hzToMilliseconds(hz);
  _eventChain.changeTimeOf(__ESP_PULSE_REFRESH_RATE_INDEX__, ms);
  return *this;
}

unsigned long ESPPulse::refreshRate() const { 
  if( _eventChain.numEvents() < 1)
    Serial.println("Panic: tried to get refresh rate but the event chain was too short");

  const uint32_t MS_BETWEEN_REFRESHES = _eventChain.getTimeOf(__ESP_PULSE_REFRESH_RATE_INDEX__); 
  return hzToMilliseconds(MS_BETWEEN_REFRESHES);
}



float ESPPulse::getDeltaThetaFromPeriod(unsigned long period_ms) const {
  if(period_ms == 0)
    panic();
  
  // Remember period is [0,2pi] because wave is offset to always be positive
  return TWO_PI / period_ms * hzToMilliseconds( refreshRate() );
}

unsigned long ESPPulse::getPeriodFromDeltaTheta(float delta_theta_rads) const {
  if(delta_theta_rads == 0)
    panic();

  return round(TWO_PI / _step_rads * hzToMilliseconds( refreshRate() ));
}


float ESPPulse::_sin(float theta){

    /* Will sine be positive or negative */
    const int8_t sign = (theta >= PI) ? -1 : 1;

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
  if(_theta_rads > TWO_PI) {
    _theta_rads = 0;
  }

  /* Calculate the sine of the new theta */
  const float NEW_SINE = _sin(_theta_rads);
  return NEW_SINE;
}



// Sine lookup table on [0,pi/2]
const float ESPPulse::_sineLut[SINE_STEPS] PROGMEM = {
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
