
#include <Adafruit_MotorShield.h>
#include "sensorbar.h"

Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 
Adafruit_DCMotor *leftMotor = AFMS.getMotor(1);
Adafruit_DCMotor *rightMotor = AFMS.getMotor(2);

#define IDLE_STATE 0
#define READ_LINE 1
#define GO_FORWARD 2
#define GO_LEFT 3
#define GO_RIGHT 4


const uint8_t SX1509_ADDRESS = 0x3E;
SensorBar mySensorBar(SX1509_ADDRESS);

// from left to right
const int s1 = 0;
const int s2 = 1;
const int s3 = 2;
const int s4 = 3;
const int s5 = 4;
const int s6 = 5;
const int s7 = 6;
const int s8 = 7;

// 
const int Kp = 25;
const int Kd = 25;

int p = 0;
int d = 0;
int pidValue = 0;
int previousError = 0;
int leftMotorSpeed = 0;
int rightMotorSpeed = 0;

void setup() {
  pinMode(8, INPUT); 
  pinMode(9, INPUT); 
  Serial.begin(115200);
  Serial.println("Program started.");
  
  //Default: the IR will only be turned on during reads.
  mySensorBar.setBarStrobe();

  //Don't forget to call .begin() to get the bar ready.  This configures HW.
  uint8_t returnStatus = mySensorBar.begin();
  if(returnStatus)
  {
    Serial.println("sx1509 IC communication OK");
  }
  else
  {
    Serial.println("sx1509 IC communication FAILED!");
  }
  Serial.println();
}



void loop() {
  uint8_t rawValue = mySensorBar.getRaw();
  boolean sensorRead[8] = {0, 0, 0, 0, 0, 0, 0, 0};

  for (int i = 0; i<8; i++) {
    sensorRead[i] = rawValue & (1 << i);
  }
  
  int error = 0;

  if (sensorRead[s1] && 
      !sensorRead[s2] && !sensorRead[s3] && !sensorRead[s4] && !sensorRead[s5] && 
      !sensorRead[s6] && !sensorRead[s7] && !sensorRead[s8]) {
     error = -3; 
    }
  else if (sensorRead[s2] && 
          !sensorRead[s1] && !sensorRead[s3] && !sensorRead[s4] && !sensorRead[s5] && 
          !sensorRead[s6] && !sensorRead[s7] && !sensorRead[s8]){
    error = -2;          
  }
  else if (sensorRead[s3] && 
          !sensorRead[s1] && !sensorRead[s2] && !sensorRead[s4] && !sensorRead[s5] && 
          !sensorRead[s6] && !sensorRead[s7] && !sensorRead[s8]){
    error = -1;          
  }
  else if (sensorRead[s4] && 
          !sensorRead[s1] && !sensorRead[s2] && !sensorRead[s3] && !sensorRead[s5] && 
          !sensorRead[s6] && !sensorRead[s7] && !sensorRead[s8]){
    error = 0;          
  }
   else if (sensorRead[s5] && 
          !sensorRead[s1] && !sensorRead[s2] && !sensorRead[s3] && !sensorRead[s4] && 
          !sensorRead[s6] && !sensorRead[s7] && !sensorRead[s8]){
    error = 0;          
  } 
  else if (sensorRead[s6] && 
          !sensorRead[s1] && !sensorRead[s2] && !sensorRead[s3] && !sensorRead[s4] && 
          !sensorRead[s5] && !sensorRead[s7] && !sensorRead[s8]){
    error = 1;          
  }
  else if (sensorRead[s7] && 
          !sensorRead[s1] && !sensorRead[s2] && !sensorRead[s3] && !sensorRead[s4] && 
          !sensorRead[s5] && !sensorRead[s6] && !sensorRead[s8]){
    error = 2;          
  }
  else if (sensorRead[s8] && 
          !sensorRead[s1] && !sensorRead[s2] && !sensorRead[s3] && !sensorRead[s4] && 
          !sensorRead[s5] && !sensorRead[s6] && !sensorRead[s7]){
    error = 3;          
  }


  p = error;
  d = error - previousError;
  pidValue = Kp*p + Kd*d;
  previousError = error; 
  
  leftMotorSpeed = 128 + pidValue;
  int rightMotorSpeed rightMotorSpeed = 128 -pidValue;

  leftMotor->setSpeed(leftMotorSpeed);
  rightMotor->setSpeed(rightMotorSpeed); 
  

}
