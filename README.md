# Lapland Memory Orb - Arduino Code
Arduino code for the memory orb made at the [University of Lapland](https://www.ulapland.fi/EN) and [Lapland University of Applied Sciences](https://www.lapinamk.fi/en) as part of the [Lapland Robotics](https://laplandrobotics.com) and [Digital Access to Sámi Heritage Archives](https://digisamiarchives.com) projects.

Knowing this code can help debugging or customization, but it is not needed to use the memory orb, a plugin handling all the input/ouput was made for Unity.

## Arduino board
The memory orb uses an [Arduino Nano RP2040 Connect](https://docs.arduino.cc/hardware/nano-rp2040-connect).

## Protocol
The data is transferred over Wi-Fi between the memory orb and the host device (e.g., a mixed reality headset) following a simple protocol:
```
<component code>:<component number>:<value>
```
The components send data only when their state changes (e.g., a button is pressed or released, the knob of a rotary encoder is rotated).

### Component code
Component code is a single character defining the type of the component:
* `B` for the buttons (momentary switches)
* `R` for the rotary encoders
* `P` for the potentiometers
* `I` for the inertial measurement unit
* `V` for the vibrating motors (as `output` from the memory orb)

### Component number
Component number is an integer:
* `0-9` for the buttons (`B`), 0-4 for the left side and 5-9 for the right side. The numbers of the button are affected following the disposition of the fingers when the hands' palms are facing down. `0` is affected to button dedicated to the little finger of the left hand, while `9` to the button dedicated to the little finger of the right hand.
* `0-1` for the rotary encoders (`R`), and vibrating motors (`V`), with `0` for the left side and `1` for the right side.
* `0-1` for the potentiometers (`P`), with `0` for the slide and `1` for the circular.
* `0-1` for the inertial measurement unit (`I`), with `0` for the gyroscope and `1` for the accelerometer.

### Value
Value change depending on the component.
* `0-1` for the buttons (`B`), with `0` meaning the button is pressed, and `1` the button is released.
* `0-1` for the rotary encoders (`R`), with `0` meaning the rotation is counter-clockwise and `1` clockwise.
* `0-1024` (theoretically) for the potentiometers (`P`). It is slightly less for the slide and even less for the circular since the rotation is limited to 180°.
* `0-1` for the vibrating motors (`V`), with `1` for activated and `0` for inactivated.
* `<x>:<y>:<z>` for the inertial measurement unit (`I`), with `x`, `y`, `z` as float.

## Misc
### Debounce
The momentary switches includes a `50 milliseconds debouncing delay`, it includes the one integrated with the rotary encoders.