#include <TM1637Display.h>

//#include "src/TM1637/TM1637Display.h"

int forwardPin = 4;
int backwardPin = 5;
int PWM_out = 6;
float val;
float mappedVal;

float linear;

const int numReadings = 30;

int readings[numReadings];      // the readings from the analog input
int readIndex = 0;              // the index of the current reading
int total = 0;                  // the running total
int average = 0;


float motorPulse = 1500;
float middleSpace = 1500;
float incrementSpeed = 5;
float decrementSpeed = 20;

float max = 0;
float min = 2000;

void setup() {
  pinMode(forwardPin, INPUT);
  pinMode(backwardPin, INPUT);
  pinMode(PWM_out, OUTPUT);

  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(9600);
  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readings[thisReading] = 0;
  }
}
void loop() {
  while (not digitalRead(forwardPin) && not digitalRead(backwardPin)) {
    smoothDialVal();
    digitalWrite(LED_BUILTIN, LOW);
    delay(20);
    if (motorPulse > middleSpace) {
      motorPulse -= decrementSpeed;
    }
    if (motorPulse < middleSpace) {
      motorPulse += decrementSpeed;
    }
    sendMSpulse(motorPulse);
    Serial.println(motorPulse);
  }

  while (digitalRead(forwardPin))
  {
    if (motorPulse < (middleSpace + calculatePulseOffset())) {
      motorPulse += incrementSpeed;
    }
    sendMSpulse(motorPulse);
    digitalWrite(LED_BUILTIN, HIGH);
    Serial.println(motorPulse);

  }

  while (digitalRead(backwardPin))
  {
    if (motorPulse > (middleSpace - calculatePulseOffset())) {
      motorPulse -= incrementSpeed;
    }
    sendMSpulse(motorPulse);
    digitalWrite(LED_BUILTIN, HIGH);
    Serial.println(motorPulse);

  }

}
bool rockerForward()
{
  return bool(digitalRead(forwardPin));
}
bool rockerBackward()
{
  return bool(digitalRead(backwardPin));
}

float calculatePulseOffset() {
  //                 min  max 1mph 2mph
  return map(average, 120, 240, 0, 500);
}

void smoothDialVal() {
  val = 1 + analogRead(A0);          // reads the value of the potentiometer (value between 0 and 1023)
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
void sendMSpulse(int pulseLength) {
  digitalWrite(PWM_out, HIGH);
  delayMicroseconds(pulseLength);
  digitalWrite(PWM_out, LOW);
  delay(20);
}
