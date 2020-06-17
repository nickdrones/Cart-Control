#include <TM1637Display.h> //import library for 7-segment diaplsy
#include <Arduino.h>
#include <SPI.h>
#include <SD.h>

//Initializing pins on Arduino Pro Mini
int forwardPin = 4;  //forward side of dead-man rocker switch
int backwardPin = 5; //backward side of dead-man rocker switch
int PWM_out = 6; //PWM out to motor controller
float val;   //temp variable for potentiometer value
float mappedVal;  //temp calculated variable for potentiometer value

float linear; //float with calculated linear value of log pot

int average = 0;

float motorPulse = 1500; //original starting motor pulse
float middleSpace = 1500; //middle motor pulse (no movement)
float incrementSpeed = 5; //how fast to increment motor speed by
float decrementSpeed = 20; //how fast to decrement motor speed by

String statusOfCart = "STOPPED";  //keep track of current robot status

#define CLK 2  //connector pins for I2C for 7-seg display
#define DIO 3

float max = 0; //min PWM pulse val
float min = 2000; //max PWM pulse val

TM1637Display display(CLK, DIO); //init 7-seg display object to pins

uint8_t data[] = { 0xff, 0xff, 0xff, 0xff }; //hex vals for all segments on, holds current val to be shown

const int chipSelect = 10;  //activator pin SD card is connected to

void setup() {
  pinMode(forwardPin, INPUT);
  pinMode(backwardPin, INPUT); //Init the pins as in or out
  pinMode(PWM_out, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(9600);
  uint8_t blank[] = { 0x00, 0x00, 0x00, 0x00 }; //hex vals for all segments off
  display.setSegments(blank); //Set display to blank

  Serial.print("Initializing SD card..."); //for debugging purposes
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1);
  }
  Serial.println("card initialized.");
  newBootUp();
  createLogFileLine("Robot Booted and Card Initialized");
}
void loop() {
  uint8_t blank[] = { 0x00, 0x00, 0x00, 0x00 }; //hex vals for all segments off
  display.setBrightness(0x0f); //set brightness to max

  while (not digitalRead(forwardPin) && not digitalRead(backwardPin)) { //while move button is not on
    smoothDialVal();
    if (!statusOfCart.equals("STOPPED")) {
      changeStatus("STOPPED");
      createLogFileLine("Cart Stopped");
    }
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
    delay(1);
  }

  while (digitalRead(forwardPin)) //while forward is pressed on switch
  {
    logCurrent();
    if (motorPulse < (middleSpace + calculatePulseOffset())) { //ramp cart speed up
      motorPulse += incrementSpeed;
    }
    if (!statusOfCart.equals("FORWARD")) {
      changeStatus("FORWARD");
      createLogFileLine("Cart Moving Forward");
    }
    sendMSpulse(motorPulse);    //send pulse to motor
    digitalWrite(LED_BUILTIN, HIGH);
    //Serial.println(motorPulse);
    writeToDisplay();
    delay(1);
  }

  while (digitalRead(backwardPin))  //while backward is pressed on switch
  {
    logCurrent();
    if (motorPulse > (middleSpace - calculatePulseOffset())) {  //ramp cart speed up
      motorPulse -= incrementSpeed;
    }
    if (!statusOfCart.equals("BACKWARD")) {
      changeStatus("BACKWARD");
      createLogFileLine("Cart Moving Backward");
    }
    sendMSpulse(motorPulse);      //send pulse to motor
    digitalWrite(LED_BUILTIN, HIGH);
    //Serial.println(motorPulse);
    writeToDisplay();
    delay(1);
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
  return map(average, 476, 1023, 0, 500); //map values from log pot to equivelant pulse difference from 1500 (ie: max speed is 240 on dial so 500 on mapped value so 1500+500=2000 which is max PWM pulse)
}

void smoothDialVal() {
  //int sensorValue = log(analogRead(A0))  / log(1000) * 1023;
  int sensorValue = analogRead(A0);
  if (abs(sensorValue - average) > 5) {
    average = sensorValue;
    String message = "Adjusting dial";
    createLogFileLine(message);
  }
}

void writeToDisplay() {
  int percentage = map(average, 476, 1023, 0, 100);
  //data[0] = display.encodeDigit(0);
  //data[1] = display.encodeDigit(int(percent / 100));
  //data[2] = display.encodeDigit(int(percent / 10));
  //data[3] = display.encodeDigit(int(percent % 10));
  if (percentage < 0) {
    display.showNumberDec(0, false);
  }
  else if (percentage > 100) {
    display.showNumberDec(100, false);
  }
  else {
    display.showNumberDec(percentage, false);
  }
}

void sendMSpulse(int pulseLength) { //send PWM pulse to motor
  digitalWrite(PWM_out, HIGH);
  delayMicroseconds(pulseLength);
  digitalWrite(PWM_out, LOW);
  delay(20);
}

void changeStatus(String currentStatus) {

}
void createLogFileLine(String message) {
  File dataFile = SD.open("datalog.txt", FILE_WRITE);
  // if the file is available, write to it:
  if (dataFile) {
    int hr = millis() / 3600000;
    int mn = (millis() / 60000) % 60;
    int sec = (millis() / 1000) % 60;
    int mil = map(millis() % 1000, 0, 999, 0, 99);
    if (hr < 10) dataFile.print("0");
    dataFile.print(hr);
    dataFile.print(":");
    if (mn < 10) dataFile.print("0");
    dataFile.print(mn);
    dataFile.print(":");
    if (sec < 10) dataFile.print("0");
    dataFile.print(sec);
    dataFile.print(":");
    if (mil < 10) dataFile.print("0");
    dataFile.print(mil);
    dataFile.print(" - ");
    dataFile.println(message);
    dataFile.close();
  }
  else {
    Serial.println("error opening datalog.txt");
  }
}


void newBootUp() {
  File dataFile = SD.open("datalog.txt", FILE_WRITE);
  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println("##########################################################");
    dataFile.println("   APACHE UVC CART CONTROL SYSTEM BOOTED - CLOCK RESET");
    dataFile.println("##########################################################");
    dataFile.close();
  }
  else {
    Serial.println("error opening datalog.txt");
  }
}
String getCurrentOnMotor() {
  int sensorValue = analogRead(A0);
  return String(map(sensorValue, 0, 1023, 0, 200));
}
void logCurrent() {
  File dataFile = SD.open("datalog.txt", FILE_WRITE);
  // if the file is available, write to it:
  if (dataFile) {
    int hr = millis() / 3600000;
    int mn = (millis() / 60000) % 60;
    int sec = (millis() / 1000) % 60;
    int mil = map(millis() % 1000, 0, 999, 0, 99);
    if (hr < 10) dataFile.print("0");
    dataFile.print(hr);
    dataFile.print(":");
    if (mn < 10) dataFile.print("0");
    dataFile.print(mn);
    dataFile.print(":");
    if (sec < 10) dataFile.print("0");
    dataFile.print(sec);
    dataFile.print(":");
    if (mil < 10) dataFile.print("0");
    dataFile.print(mil);
    dataFile.print(" - ");
    dataFile.print(" Motor Current: ");
    dataFile.print(getCurrentOnMotor());
    dataFile.println(" Amps");
    dataFile.close();
  }
  else {
    Serial.println("error opening datalog.txt");
  }
}
