#ifndef __ESP_PULSE_H__
#define __ESP_PULSE_H__

#define SINE_STEPS  101

#ifndef __LED_INTERFACE_H__
#include "ESPLedInterface.h"
#endif

class ESPPulse : public ESPLedInterface {

public:

    ESPPulse(ESPLed *led)
    :
    ESPLedInterface(led, 100)
    {
        
    }

    /**
     * @brief Sets the time for one pulse period,
     * min -> max -> min
     * 
     * @param ms The time in milliseconds
     * 
     * @return this
     */
    ESPPulse &period(unsigned long ms);
    unsigned long period() const;

    /**
     * @brief Sets the rate at which changes in pulse brightness
     * will be updated. This shouldn't need to be faster than what
     * the human eye can perceive
     * 
     * @param hz The rate in hertz
     * 
     * @return this
     */
    ESPPulse &refreshRate(unsigned int hz);
    unsigned long rRefreshRate() const { return _refreshRate_hz; }

protected:

    // Handle blinking, returns the time until the next action
    virtual void _loop();

private:

    /**
     * @brief Computes the sine of an angle using a lookup
     * table with values from 0 to PI/2
     * 
     * @param theta The angle in radians
     * 
     * @return sin(theta)
     */
    float _sin(float theta);

private:

    unsigned int _refreshRate_hz = 60;   // Refresh rate for pulse mode
    float _theta_rads = PI;              // Current value of theta
    float _step_rads = PI;               // Incremental change of theta

    static const float _sineLut[SINE_STEPS];

};

#endif