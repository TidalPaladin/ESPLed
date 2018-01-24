#ifndef __ESP_COLOR_LED_H__
#define __ESP_COLOR_LED_H__

#include "ESPLed.h"

class ESPColorLed : public ESPLed {

public:

    ESPColorLed(gpio_num_t red, gpio_num_t blue);
    
    ESPColorLed &on(uint8_t brightness1, uint8_t brightness2);

private:

    




};



#endif