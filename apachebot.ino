#include <TM1637Display.h> //import library for 7-segment diaplsy
#include <Arduino.h>

//Initializing pins on Arduino Pro Mini
int forwardPin = 4;  //forward side of dead-man rocker switch
int backwardPin = 5; //backward side of dead-man rocker switch
int PWM_out = 6; //PWM out to motor controller
float val;   //temp variable for potentiometer value
float mappedVal;  //temp calculated variable for potentiometer value

float linear; //float with calculated linear value of log pot

const int numReadings = 10;

int readings[numReadings];      // the readings from the analog input
int readIndex = 0;              // the index of the current reading
int total = 0;                  // the running total
int average = 0;


float motorPulse = 1500; //original starting motor pulse
float middleSpace = 1500; //middle motor pulse (no movement)
float incrementSpeed = 5; //how fast to increment motor speed by
float decrementSpeed = 20; //how fast to decrement motor speed by

#define CLK 2  //connector pins for I2C for 7-seg display
#define DIO 3

float max = 0; //min PWM pulse val
float min = 2000; //max PWM pulse val

TM1637Display display(CLK, DIO); //init 7-seg display object to pins

uint8_t data[] = { 0xff, 0xff, 0xff, 0xff }; //hex vals for all segments on, holds current val to be shown

void setup() {
  pinMode(forwardPin, INPUT);
  pinMode(backwardPin, INPUT); //Init the pins as in or out
  pinMode(PWM_out, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(9600);
  for (int thisReading = 0; thisReading < numReadings; thisReading++) { //initialize whole buffer for log pot readings to 0 (part of moving average filter)
    readings[thisReading] = 0;
  }
  uint8_t blank[] = { 0x00, 0x00, 0x00, 0x00 }; //hex vals for all segments off
  display.setSegments(blank); //Set display to blank
}
void loop() {
  uint8_t blank[] = { 0x00, 0x00, 0x00, 0x00 }; //hex vals for all segments off
  display.setBrightness(0x0f); //set brightness to max

  while (not digitalRead(forwardPin) && not digitalRead(backwardPin)) { //while move button is not on
    smoothDialVal();
    digitalWrite(LED_BUILTIN, LOW);
    delay(20);
    if (motorPulse > middleSpace) {
      motorPulse -= decrementSpeed;
    }                                      //slow down cart from either direction back to 1500
    if (motorPulse < middleSpace) {
      motorPulse += decrementSpeed;
    }
    sendMSpulse(motorPulse);             //send pulse to motor
    //Serial.println(motorPulse);
    writeToDisplay();
  }

  while (digitalRead(forwardPin)) //while forward is pressed on switch
  {
    if (motorPulse < (middleSpace + calculatePulseOffset())) { //ramp cart speed up
      motorPulse += incrementSpeed;
    }
    sendMSpulse(motorPulse);    //send pulse to motor
    digitalWrite(LED_BUILTIN, HIGH);
    //Serial.println(motorPulse);
    writeToDisplay();
  }

  while (digitalRead(backwardPin))  //while backward is pressed on switch
  {
    if (motorPulse > (middleSpace - calculatePulseOffset())) {  //ramp cart speed up
      motorPulse -= incrementSpeed;
    }
    sendMSpulse(motorPulse);      //send pulse to motor
    digitalWrite(LED_BUILTIN, HIGH);
    //Serial.println(motorPulse);
    writeToDisplay();
  }
}
bool rockerForward()
{
  return bool(digitalRead(forwardPin)); //unneeded but this does a nice bool instead of casting in the "if" statement
}
bool rockerBackward()
{
  return bool(digitalRead(backwardPin)); //unneeded but this does a nice bool instead of casting in the "if" statement
}

float calculatePulseOffset() {
  //                 min  max 1mph 2mph
  return map(average, 120, 240, 0, 500); //map values from log pot to equivelant pulse difference from 1500 (ie: max speed is 240 on dial so 500 on mapped value so 1500+500=2000 which is max PWM pulse)
}

void smoothDialVal() {
  val = 1 + analogRead(A0);          // performs a moving average filter on log pot readings (PLZ no touchy it works now and I wanna die)
  linear = log(val) * 1000;
  mappedVal = map(linear, 3000, 6000, 0.000, 180.000);

  total = total - readings[readIndex];
  readings[readIndex] = mappedVal;
  total = total + readings[readIndex];
  readIndex = readIndex + 1;
  if (readIndex >= numReadings) {
    readIndex = 0;
  }
  average = total / numReadings;
}

void writeToDisplay() {
  int percentage = map((log(analogRead(A0))*1000), 5140, 6930, 0, 100);
  //data[0] = display.encodeDigit(0);
  //data[1] = display.encodeDigit(int(percent / 100));
  //data[2] = display.encodeDigit(int(percent / 10));
  //data[3] = display.encodeDigit(int(percent % 10));
  display.showNumberDec(percentage, false);
  Serial.println(log(analogRead(A0))*1000);
  //display.setSegments(data);
}

void sendMSpulse(int pulseLength) { //send PWM pulse to motor
  digitalWrite(PWM_out, HIGH);
  delayMicroseconds(pulseLength);
  digitalWrite(PWM_out, LOW);
  delay(20);
}
