# ESPLed
ESPLed makes it easy to control Leds on the ESP8226 and ESP32 for use as indicator lights.

## Key Features
  * **Brightness Compensation** - 
  Led power is set using a percentage from 0 to 100. This value is mapped to a 10 bit PWM value, and adjustments are made for the antilog way in which brightness is perceived by the human eye. 

  * **Active Modes** - 
  Preset modes include blinking and pulsing with user adjustable timings. Active modes are implemented using a strategy pattern, encapsulating led behaviors separate from physical characteristics like GPIO or maximum brightness.

  * **User Definable Active Modes** - 
  Users can easily create new active modes, allowing for great customization of behavior.

  * **HIGH vs LOW Leds** - 
  Specify whether the Led is turned on by a logic `HIGH` or `LOW` and the library will automatically adapt `on()` an `off()` functions to compensate

## TODO List

  * Create a derived class to handle bi and tri color LEDS
  * Create a class derived from ESPBlink that will allow for blinking of error codes


## API

```c++
ESPLed(gpio_num_t pin, uint8_t off_state = LOW)
```

Constructs an ESPLed on GPIO `pin` such that the led will be off when the GPIO is in state `off_state`

```c++
gpio_num_t pin() const
```

Returns the GPIO being used for the `ESPLed` object

```c++
ESPLed &maxBrightness(uint8_t percent)
uint8_t maxBrightness() // Getter
```

Sets the maximum brightness of the led as a percentage from 1-100. Full brightness corresponds to a duty cycle of 100%.

```c++
ESPLed &minBrightness(uint8_t percent)
uint8_t minBrightness() // Getter
```
Sets the minimum brightness of the led as a percentage. This is the brightness level that will be used when the led is turned off.

```c++
ESPLed &setMode(ESPLedInterface &strategy)
```

Assigns the `ESPLed` a behavior described by the strategy pattern `strategy`. 

```c++
ESPLed &start()
ESPLed &stop()
```

Starts or stops the currently assigned behavior. When `stop()` is called the `ESPLed` will retain its state at the time of stopping.

```c++
bool isOn()
```

Returns true if the `ESPLed` is on, and false otherwise. The on / off state is tracked internally, so at present it will have inaccurate behavior when the minimum brightness is not zero.


