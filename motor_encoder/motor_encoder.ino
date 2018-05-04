#include "TBMotor.h"
#include <Encoder.h>
#include "pins_arduino.h"

Encoder rightEnc(9,9);
Encoder leftEnc(10,10);

int pos = 0;    // variable to store the servo position

int counter1 = 0;
int counter2 = 0;

long oldPosition1  = -999;
long oldPosition2  = -999;


OseppTBMotor leftMotor(7, 6);
OseppTBMotor rightMotor(4, 5);

void setup() {
  Serial.begin(9600);
}

void loop() {
//  moveLeftForward(100,980);
  moveRightForward(100,980);
}

void moveLeftForward(int motorSpeed, int encSteps){
    leftMotor.forward(motorSpeed); 
  long newPosition1 = leftEnc.read();
  if (newPosition1 != oldPosition1) {
    oldPosition1 = newPosition1;
    counter1++;
  }
  if(counter1==encSteps){
    leftMotor.forward(0);
    counter1=0;
    delay(5000);
 }
}

void moveRightForward(int motorSpeed, int encSteps){
  rightMotor.forward(motorSpeed);
  long newPosition2 = rightEnc.read();
  if (newPosition2 != oldPosition2) {
    oldPosition2 = newPosition2;
    counter2++;
  }
  if(counter2==encSteps){
    rightMotor.forward(0);
    counter2=0;
    delay(5000);
 }
}


