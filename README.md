# Lapland Memory Orb - Arduino Code
**v0.8** _(network and vibrating motors added)_

Arduino code for the memory orb made at the [University of Lapland](https://www.ulapland.fi/EN) and [Lapland University of Applied Sciences](https://www.lapinamk.fi/en) as part of the [Lapland Robotics](https://laplandrobotics.com) and [Digital Access to Sámi Heritage Archives](https://digisamiarchives.com) projects.

Knowing this code can help debugging or customization, but it is not needed to use the memory orb, a plugin handling all the input/ouput was made for Unity.

## Arduino board
The memory orb uses an [Arduino Nano RP2040 Connect](https://docs.arduino.cc/hardware/nano-rp2040-connect).

## Network
The data is transferred over Wi-Fi between the memory orb and the host device (e.g., a mixed reality headset).
Currently (unfortunately), the network `SSID`, `password` and `IP address` are hardcoded into the `credentials.h` file.

## Protocol
The data is transferred over Wi-Fi between the memory orb and the host device (e.g., a mixed reality headset) following a simple protocol:
```
<component code>:<component number>:<value>;
```

Examples:

```
B:7:0;
B:11:1;
R:1:1;
P:0:87;

V:1:1;
```

The components send data `only` when their state changes (e.g., a button is pressed or released, the knob of a rotary encoder is rotated).

### Component code
Component code is a single character defining the type of the component:
* `B` for the buttons (momentary switches)
* `R` for the rotary encoders
* `P` for the potentiometers
* `I` for the inertial measurement unit
* `V` for the vibrating motors (as `output` from the memory orb)

### Component number
Component number is an integer:
* `0-11` for the buttons (`B`), `0-9` for the individual buttons, `0-4` for the left side and `5-9` for the right side. The numbers of the button are affected following the disposition of the fingers when the hands' palms are facing down. `0` is affected to button dedicated to the little finger of the left hand, while `9` to the button dedicated to the little finger of the right hand. `10` and `11` are reserved for the switches integrated to the rotary encoders, respectively left and right.
* `0-1` for the rotary encoders (`R`), and vibrating motors (`V`), with `0` for the left side and `1` for the right side.
* `0-1` for the potentiometers (`P`), with `0` for the slide and `1` for the circular.
* `0-1` for the inertial measurement unit (`I`), with `0` for the gyroscope and `1` for the accelerometer.

### Value
Value change depending on the component.
* `0-1` for the buttons (`B`), with `0` meaning the button is pressed, and `1` the button is released.
* `0-1` for the rotary encoders (`R`), with `0` meaning the rotation is counter-clockwise and `1` clockwise.
* `0-99` (theoretically) for the potentiometers (`P`). It is slightly less for the circular since the rotation is limited to 180°. Warning, the circular potentiometer is linear, while the slide potentiometer is logarithmic.
* `0-1` for the vibrating motors (`V`), with `1` for activating and `0` for deactivating.
* `<x>:<y>:<z>` for the inertial measurement unit (`I`), with `x`, `y`, `z` as float.

## Misc
### Vibration limit
The vibrating motors are automatically turn off after `5 seconds` but longer uninterrupted periods are still possible without changing the Arduino code, simply by sending multiple activating commands (e.g., every 4s) from the host device.

### Debounce
The momentary switches (including those integrated within the rotary encoders) have `50 milliseconds debouncing delay`.

## License
```
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

   http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
```
A permissive license whose main conditions require preservation of copyright and license notices. Contributors provide an express grant of patent rights. Licensed works, modifications, and larger works may be distributed under different terms and without source code.

## TODO
Inertial measure unit remain to be implemented.