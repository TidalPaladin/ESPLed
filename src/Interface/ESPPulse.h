#ifndef __ESP_PULSE_H__
#define __ESP_PULSE_H__
#include "ESPLedInterface.h"

#define SINE_STEPS 101
#define __ESP_PULSE_REFRESH_RATE_INDEX__ 0

class ESPPulse : public ESPLedInterface {

  public:
	/**
	 * @brief Consructor with default refresh rate of 100 Hz and default period
	 * of 1000 milliseconds
	 *
	 * post: isStarted() == false, attachedLedCount() == 0
	 *
	 */
	ESPPulse() : ESPLedInterface(1), _currentSine(0) { construct(30, 2000); }

	/**
	 * @brief Consructor with custom refresh rate
	 *
	 * @param refresh_rate_hz   The refresh rate
	 * @param period_ms          The pulse period in ms
	 *
	 * post: isStarted() == false, attachedLedCount() == 0
	 *
	 */
	ESPPulse(unsigned long refresh_rate_hz, unsigned long period_ms)
		: ESPLedInterface(1), _currentSine(0) {
		construct(refresh_rate_hz, period_ms);
	}

	/**
	 * @brief Sets the time taken for the Led to pulse through one full sine
	 * period
	 *
	 * @param ms    The time in milliseconds for the pulse to go from dim to
	 * bright and back to dim. 0 < ms
	 *
	 * @return this
	 */
	ESPPulse &setPeriod(unsigned long ms);
	unsigned long getPeriod() const;

	/**
	 * @brief   Sets the rate at which changes in pulse brightness
	 *          will be updated. This shouldn't need to be faster than what
	 *          the human eye can perceive
	 *
	 * @param hz    The refresh rate in hertz, hz > 0
	 *
	 * @return this
	 */
	ESPPulse &setRefreshRate(unsigned int hz);
	unsigned long getRefreshRate() const;

	/**
	 * @brief Resets theta such that the sine wave will be at maximum amplitude.
	 * Pulseing continues uninterrupted
	 *
	 * post: Pulse intensity set to maximum amplitude, isStarted() after =
	 * isStarted before
	 *
	 */
	void setPulseToPeak();

	/**
	 * @brief Resets theta such that the sine wave will be at minimum amplitude.
	 * Pulseing continues uninterrupted
	 *
	 * post: Pulse intensity set to minimum amplitude, isStarted() after =
	 * isStarted before
	 *
	 */
	void setPulseToTrough();

  private:
	/**
	 * @brief Constructor helper
	 *
	 * @param refresh_rate_hz   The refresh rate in Hz to initialzie to
	 *                          refresh_rate_hz > 0
	 *
	 * @param period_ms         The period in milliseconds to initialize to
	 *                          period_ms > 0
	 *
	 * post: EspEventChain has 1 event added for pulsing
	 *
	 */
	void construct(unsigned long refresh_rate_hz, unsigned long period_ms);

	/**
	 * @brief Converts a pulse period in milliseconds to an angular step size
	 * such that at the current refresh rate the Led will appear to pulse at the
	 * given period
	 *
	 * @param period_ms The pulse period in milliseconds
	 *                  0 < period_ms
	 *
	 * @return The delta theta value appropriate for refreshRate()
	 */
	float getDeltaThetaFromPeriod(unsigned long period_ms) const;

	/**
	 * @brief Converts an angular step size at which the sinusoidal pulse
	 * intensity will advance to the period in milliseconds it takes for a full
	 * sine wave to complete
	 *
	 * @param delta_theta_rads  The step size of theta in radians
	 *                              0 < delta_theta_rads
	 *
	 * @return The period in milliseconds for a full sine wave refreshing at
	 * refreshRate() steps/s
	 */
	unsigned long getPeriodFromDeltaTheta(float delta_theta_rads) const;

	/**
	 * @brief Calculates the new brightness level after stepping forward
	 *
	 * @param led   The ESPLed to calculate from, using its values of min and
	 * max brightness
	 *
	 * @return led.minBrightness() <= val <= led.maxBrightness() appropriate to
	 * where we are on the sine wave
	 */
	uint8_t calculateNewBrightness(ESPLed *led) const;

	/**
	 * @brief Calculates the new value of sin(theta) after taking a step forward
	 *
	 * @param
	 */
	float calculateNewSineValue();

	/**
	 * @brief Computes the sine of an angle using a lookup
	 * table with values from 0 to PI/2
	 *
	 * @param theta The angle in radians
	 *
	 * @return sin(theta)
	 */
	static float _sin(float theta);

	/**
	 * @brief Constrains theta to -pi, pi
	 *
	 */
	static float constrainTheta(float theta);

	/**
	 * @brief Populates a sine lookup table at compile time
	 *
	 */
	// NYI void _createSineLookup();

  private:
	float _theta_rads = PI; // Current value of theta
	float _step_rads;		// Incremental change of theta
	float _currentSine;		// Track sin(theta) separately
};

#endif