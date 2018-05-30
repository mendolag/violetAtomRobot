/*
Exploring Arduino - Code Listing 6-1: Potentiometer Serial Print Test Program
http://www.exploringarduino.com/content/ch6
Copyright 2013 Jeremy Blum ( http://www.jeremyblum.com )
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License v3 as published by
the Free Software Foundation.
*/

#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "sensorbar.h"
  //Simple Serial Printing Test with a Potentiometer

Adafruit_MotorShield AFMS = Adafruit_MotorShield();

Adafruit_DCMotor * right = AFMS.getMotor(2);
Adafruit_DCMotor * left = AFMS.getMotor(1);

const int ANALOGRIGHT = 0; //ANALOGRIGHT on analog pin 0
const int ANALOGCENTER = 1; //Pot on analog pin 0
const int ANALOGLEFT = 2; //Pot on analog pin 0
const int DESTRA = 2;
const int SINISTRA = 1;
const int DRITTO = 0;
const int MARGINE = 5;
const int DIM_BUFFER = 10;
const int DELAY = 25;

CircularBuffer bufferCenter(DIM_BUFFER);
CircularBuffer bufferRight(DIM_BUFFER);
CircularBuffer bufferLeft(DIM_BUFFER);

void setup() {
  Serial.begin(9600); //Start serial port with baud = 9600
  AFMS.begin();
}

void loop() {
  int dir = direzione();

  left -> setSpeed(255);
  right -> setSpeed(255);
  if (dir == DRITTO) {
    Serial.println("DRITTO");
    left -> run(FORWARD);
    right -> run(FORWARD);
  } else if (dir == SINISTRA) {
    Serial.println("SINISTRA");
    left -> run(BACKWARD);
    right -> run(FORWARD);
  } else {
    Serial.println("DESTRA");
    left -> run(FORWARD);
    right -> run(BACKWARD);
  }
  delay(DELAY); 
}

int direzione() {
  int valRight = analogRead(ANALOGRIGHT); //Read potentiometer 
  int valCenter = analogRead(ANALOGCENTER); //Read potentiometer 
  int valLeft = analogRead(ANALOGLEFT); //Read potentiometer 
  int distRightCurrent = map(valRight, 0, 700, 30, 4); //Convert to centimeters
  int distCenterCurrent = map(valCenter, 0, 700, 30, 4); //Convert to centimeters
  int distLeftCurrent = map(valLeft, 0, 700, 30, 4); //Convert to centimeters
  bufferCenter.pushElement(distCenterCurrent);
  bufferLeft.pushElement(distLeftCurrent);
  bufferRight.pushElement(distRightCurrent);
//  Serial.print("Analog Reading: ");
//  Serial.print(valRight); //Print raw analog value
//  Serial.print("\t");
  Serial.print(valCenter); //Print raw analog value
  Serial.print("\t");
//  Serial.print(valLeft); //Print raw analog value
//  Serial.print("\t");
//  Serial.print("  Centimeters: ");
//  Serial.print("right : ");
//  Serial.print(distRightCurrent); //Print distance
//  Serial.print("cm"); //Print centimeter abbrev
//  Serial.print("\t");
  Serial.print("center : ");
  Serial.print(distCenterCurrent); //Print distance
  Serial.print("cm"); //Print centimeter abbrev
//  Serial.print("\t");
//  Serial.print("left : ");
//  Serial.print(distLeftCurrent); //Print distance
//  Serial.print("cm"); //Print centimeter abbrev
//  Serial.print("\t");

  int distCenter = bufferCenter.averageLast(DIM_BUFFER);
  int distRight = bufferRight.averageLast(DIM_BUFFER);
  int distLeft = bufferLeft.averageLast(DIM_BUFFER);

  Serial.print("   Centimeters Avg: ");
  Serial.print("right : ");
  Serial.print(distRight); //Print distance
  Serial.print("cm"); //Print centimeter abbrev
  Serial.print("\t");
  Serial.print("center : ");
  Serial.print(distCenter); //Print distance
  Serial.print("cm"); //Print centimeter abbrev
  Serial.print("\t");
  Serial.print("left : ");
  Serial.print(distLeft); //Print distance
  Serial.print("cm"); //Print centimeter abbrev
  Serial.println("\t");

  if (distCenter >= distRight && distCenter >= distLeft) {
    if (distRight < MARGINE) {
      return SINISTRA;
    } else if (distLeft < MARGINE) {
      return DESTRA;
    } else {
      return DRITTO;
    }
  } else if (distLeft > distRight && distLeft > distCenter) {
    return SINISTRA;
  } else {
    return DESTRA;
  }
}
