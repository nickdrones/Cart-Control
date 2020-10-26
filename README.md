# Smart Cart Control
This code is for driving a heavy-load cart in a smart manner with fine speed control, a dead-man switch, and a ramp up / ramp down function. An operator will move behind the cart to steer it and control the switch to move the cart forward or backward. A rotary dial is used to allow the operator to select a speed (0-100% power) of the motor while moving forward. Moving the cart in reverse results in a constant speed and does not depend on the dial setting.

## Hardware
Final Prototype:
- Arduino Pro Mini
- Talon SRX
- 10k logarithmic potentiometer
- Single-Pole Triple-Throw momentary rocker switch
- 7 Segment display to print set motor speed (% power)

## Code
The code is all very simple and easy to read, with the exception of the area where we remapped the logarithmic dial to give a linear output. Originally, I planned to use formula to simply remap the values but for unknown reasons it was inconsistent. I finally settled on simply remapping the logarithmic curve into two seperate linear sections which allowed me to more easily dial in the correct power value. The below image is what I used as a reference, taken from a now-deleted video uploaded by Evan Kale on YouTube.

![Evan Kale Diagram](https://raw.githubusercontent.com/nickdrones/Cart-Control/master/vlcsnap-2020-10-01-12h34m34s020.png)
