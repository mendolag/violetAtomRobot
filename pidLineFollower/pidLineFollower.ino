
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

// from right to left
const int s1 = 0;
const int s2 = 1;
const int s3 = 2;
const int s4 = 3;
const int s5 = 4;
const int s6 = 5;
const int s7 = 6;
const int s8 = 7;

// 
const int Kp = 15;
const int Kd = 50;

int p = 0;
int d = 0;
int pidValue = 0;
int previousError = 0;
int leftMotorSpeed = 0;
int rightMotorSpeed = 0;

boolean isMotorOn = false;

void setup() {
  pinMode(8, INPUT); // red button
  pinMode(9, INPUT); 
  Serial.begin(115200);
  Serial.println("Program started.");
  
  //Default: the IR will only be turned on during reads.
  mySensorBar.clearBarStrobe();

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

  AFMS.begin();
}



void loop() {
  if (digitalRead(8) == HIGH){
    if (isMotorOn){
      isMotorOn = false;
    } else {
      isMotorOn = true;
    }
    delay(500);
  }

  
  uint8_t rawValue = mySensorBar.getRaw();
//  Serial.println("rawValue: " + rawValue);
  boolean sensorRead[8] = {0, 0, 0, 0, 0, 0, 0, 0};

  for (int i = 0; i<8; i++) {
    sensorRead[i] = rawValue & (1 << i);
//   Serial.println("s" + (String)(i+1) + ": " + (String)sensorRead[i]);
  }
  
  int error = 0;
  boolean center=sensorRead[s4] || sensorRead[s5];
  int left=0;
  int right=0;
  if(sensorRead[s1]){
    left+=4;
  }else if(sensorRead[s2]){
    left+=2;
  }
  if(sensorRead[s3]){
    left+=1;
  }
  if(sensorRead[s8]){
    right+=4;
  }else if(sensorRead[s7]){
    right+=2;
  }
  if(sensorRead[s6]){
     right+=1;
  }



  if(right>left){
    if((right%2==0 && center)){
      error=0;
    }else{
      error=right*-1;
    }
  }
  else if (right<left){
    if((left%2==0 && center)){
      error=0;
    }else{
      error=left*1;
    }
  }else{
    error=0;
  }

//Serial.print("error:");
//Serial.println(error);

  p = error;
  d = error - previousError;
  pidValue = Kp*p + Kd*d;
  int backupError=previousError;

  previousError = error; 
    
  leftMotorSpeed = 150 + pidValue;
  rightMotorSpeed = 150-pidValue;
//  Serial.print(leftMotorSpeed);
//  Serial.print(rightMotorSpeed);

  if (!isMotorOn){
    leftMotor->setSpeed(0);
    rightMotor->setSpeed(0);
    leftMotor->run(RELEASE);
    rightMotor->run(RELEASE);
  } else {
  if(!center){
    if(error==0){
     leftMotor->setSpeed(200);
    rightMotor->setSpeed(200);
      if(backupError<0){
      leftMotor->run(BACKWARD);
      rightMotor->run(FORWARD);
      delay(200);
        }else if(backupError>0){
          leftMotor->run(FORWARD);
      rightMotor->run(BACKWARD);
      delay(200);
        }else{
      leftMotor->run(BACKWARD);
      rightMotor->run(BACKWARD);
      delay(100);}
      return;
    }
    else{
      if(leftMotorSpeed>=120){
      leftMotor->run(FORWARD);
     }
     else {
      leftMotor->run(BACKWARD);
     }
    if(rightMotorSpeed>=120){
      rightMotor->run(FORWARD);
     }
     else {
      rightMotor->run(BACKWARD);
     }
    }
  }else{
  leftMotor->run(FORWARD);
  rightMotor->run(FORWARD);
  
  }
    
    Serial.println("motorOn");
    leftMotor->setSpeed(abs(leftMotorSpeed));
    rightMotor->setSpeed(abs(rightMotorSpeed));


  }
   

}

