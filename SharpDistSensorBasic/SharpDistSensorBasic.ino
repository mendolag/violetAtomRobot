#include <Adafruit_MotorShield.h>
#include "TBMotor.h"
#include <Encoder.h>
#include "pins_arduino.h"
#include "utils.h"

//motors definitions
Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 
Adafruit_DCMotor *rightMotor = AFMS.getMotor(2);
Adafruit_DCMotor *leftMotor = AFMS.getMotor(1);
Encoder encoder(9,10);

//Case definitions
#define IDLE_STATE 0
#define GO_FORWARD 2
#define GO_LEFT 3
#define GO_RIGHT 4
#define GO_BACKWARD 5
#define GO_LEFT_FAR 8
#define GO_RIGHT_FAR 9
#define READ_DISTANCE 6
#define READ_MORE 7
int state = IDLE_STATE;

const int DIM_BUFFER = 5;

CircularBuffer bufferCenter(DIM_BUFFER);
CircularBuffer bufferRight(DIM_BUFFER);
CircularBuffer bufferLeft(DIM_BUFFER);
//Encoder utils;
long oldPosition  = -999;

//Part for IRSensors
double const mult=0.0048828125;
int const distanceThreshold=7;
#define sensorRight A0 
#define sensorCenter A1
#define sensorLeft A2
#define margin 7

double distanceRight;
double distanceCenter;
double distanceLeft;
boolean motorOn=false;
int topSpeed=100;
int minCenter=30;

void setup() {
  Serial.begin(9600); // start the serial port
  pinMode(sensorRight, INPUT);
  pinMode(sensorCenter, INPUT);
  pinMode(sensorLeft, INPUT);
  pinMode(8,INPUT);

    AFMS.begin(); 

}

 double get_IR (uint16_t value) {
        if (value < 16)  value = 16;
        return 2076.0 / (value - 11.0);
}

void loop() {
  if(digitalRead(8)==HIGH && !motorOn){
    motorOn=true;
    Serial.println("ON");
    rightMotor->setSpeed(topSpeed);
    leftMotor->setSpeed(topSpeed);
    delay(1000);
  }
  if(digitalRead(8)==HIGH && motorOn){
    motorOn=false;
    Serial.println("OFF");
         rightMotor->setSpeed(0);
      leftMotor->setSpeed(0);
              rightMotor->setSpeed(0);
      leftMotor->setSpeed(0);
    delay(1000);
  }
  if(!motorOn){
    Serial.println(encoder.read());
    state=IDLE_STATE;
  }
  delay(30);
  Serial.println();
      rightMotor->setSpeed(topSpeed);
      leftMotor->setSpeed(topSpeed);
      
      Serial.println(state);
      Serial.print("right:");
      Serial.println(distanceRight);
      Serial.print("center:");
      Serial.println(distanceCenter);
      Serial.print("left:");
      Serial.println(distanceLeft);

      
  switch(state){
    case READ_DISTANCE:
      
  // 5v
  bufferRight.pushElement(get_IR(analogRead(sensorRight)));
  distanceRight = bufferRight.averageLast(DIM_BUFFER);
  delay(10);// value from sensor * (5/1024)
  bufferCenter.pushElement(get_IR(analogRead(sensorCenter)));
  distanceCenter = bufferCenter.averageLast(DIM_BUFFER);
  delay(10);// value from sensor * (5/1024)
  bufferLeft.pushElement(get_IR(analogRead(sensorLeft)));
  distanceLeft = bufferLeft.averageLast(DIM_BUFFER);  // value from sensor * (5/1024)
  if(distanceCenter<margin && distanceLeft< margin && distanceRight < margin){
    state=GO_BACKWARD;
    break;
  }
  
if(distanceCenter<minCenter && distanceCenter < distanceLeft-distanceThreshold 
  && distanceLeft>distanceRight){
    state=GO_LEFT_FAR;
    Serial.print("far");
    break;
  }else if(distanceCenter<minCenter  && distanceCenter < distanceRight-distanceThreshold && 
  distanceRight > distanceLeft ){
    state=GO_RIGHT_FAR;
    Serial.print("far");

    break;
  }else{
     if(distanceLeft<margin && distanceRight<margin){
      if(distanceLeft<distanceRight){
        state=GO_RIGHT;
      }else{
        state=GO_LEFT;
      }
    Serial.print("near");
    }else if(distanceLeft<margin){
      state=GO_RIGHT;
      Serial.print("near");
    }
    else if(distanceRight<margin){
      state=GO_LEFT;
      Serial.print("near");

    }else{
      if(distanceCenter<margin){
        if(distanceLeft<distanceRight){
        state=GO_RIGHT;
      }else{
        state=GO_LEFT;
      }
      }else{
      state=GO_FORWARD;
      }
      Serial.print("far");

    }
    break;

  }
    case READ_MORE:
    int rm;
    rm=0;
      rightMotor->setSpeed(0);
      leftMotor->setSpeed(0);
      rightMotor->run(RELEASE);
      leftMotor->run(RELEASE);
    while(rm<=10){
  bufferRight.pushElement(get_IR(analogRead(sensorRight)));
  distanceRight = bufferRight.averageLast(DIM_BUFFER);
  delay(10);// value from sensor * (5/1024)
  bufferCenter.pushElement(get_IR(analogRead(sensorCenter)));
  distanceCenter = bufferCenter.averageLast(DIM_BUFFER);
  delay(10);// value from sensor * (5/1024)
  bufferLeft.pushElement(get_IR(analogRead(sensorLeft)));
  distanceLeft = bufferLeft.averageLast(DIM_BUFFER); 
  rm++;
    }
    state=READ_DISTANCE;
    break;
    
    case IDLE_STATE:
      rightMotor->setSpeed(0);
      leftMotor->setSpeed(0);
      rightMotor->run(RELEASE);
      leftMotor->run(RELEASE);
      state=READ_DISTANCE;
      break;
    case GO_FORWARD:
        rightMotor->run(FORWARD);
        leftMotor->run(FORWARD);
        state=READ_DISTANCE;
        break;
    case GO_BACKWARD:
      rightMotor->run(BACKWARD);
      leftMotor->run(BACKWARD);
      state=READ_DISTANCE; 
      break;  
    case GO_LEFT_FAR:
      rightMotor->run(FORWARD);
      leftMotor->run(BACKWARD);
      moveUntil(300);
      break;
    case GO_RIGHT_FAR:
      rightMotor->run(BACKWARD);
      leftMotor->run(FORWARD);
      moveUntil(300);
      break;
    case GO_LEFT:
      rightMotor->run(FORWARD);
      leftMotor->run(BACKWARD);
      state=READ_DISTANCE;

      break;
    case GO_RIGHT:
      rightMotor->run(BACKWARD);
      leftMotor->run(FORWARD);
      state=READ_DISTANCE;
      break;
  }
  }

  void moveUntil(int steps){
    int counter=0;
    while (counter<steps){
    long newPosition = encoder.read();
    if (newPosition != oldPosition) {
        oldPosition = newPosition;
        counter++;
    }
  }
  state=READ_MORE;
    }
 
  

