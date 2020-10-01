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

![Evan Kale Diagram](https://lh3.googleusercontent.com/1hgUaX3I_uYzKhcgUNY2mM5KGEcL0MHRCm8SHuZo4xuUPgJT_Ej8dfmNFtBdaV65DvsoweFaM1sY6U36EAIQ827_NODmlGqfmQdqUEhtWdTJTByBTT0qjBD9JSDy541aW3_fyL7s7uKXjfGZTfJGjyrNPQ8KKbyB91DSITbRN1LnVyq45LjW71YwQk9AwATysGJvSYWyF9mlot9ArQPreYIoD9jEzbAH7fQK0MzPmBs5Wf1zCcNXrnSdpbtktiMDf3ul62uEp02xghXXBCZbZNxkq7H7ftIb9QTNsJHlXsrlSop4u56QmlsWxYXNuak1m6U_mOc25AF7CeV-WGWaMcwzEbcURdmWLmmKXQZcOg6iWkCA3g8nwZVS-JqZgnoxvokeeUkQAD7yuXYxG4uMf1lXP4ju9ms5ipfzCAdapicFugtga8v4b1UHGyObO_5fhVzG170nMyS8TQQKwXx69duiAmkJc2LK1G81q_cEhS7mxU70X0yMZQalO8D6yP1kYTKn8WBiP23lo9rl9dUzuxTDq3tMsabonRoeFkhC2OA4ODNeZZ6L4JARZLDnCyMEHUGJZrui3IOCSMFvVEDaYFeRpRMCCIVaOJf-3hcffuyHuCUG0Md47_jAREEwAjWth6yTgUCKrrq8sTKTY73XQ9la5vgEf_Za2XsT_aEOepKohy0auELlV19GhY18iOs=w1920-h1080-no?authuser=0)
