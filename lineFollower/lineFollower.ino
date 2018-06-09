/******************************************************************************
AveragingReadBarOnly.ino

A sketch for reading sensor data into a circular buffer

Marshall Taylor, SparkFun Engineering

5-27-2015

Library:
https://github.com/sparkfun/SparkFun_Line_Follower_Array_Arduino_Library
Product:
https://github.com/sparkfun/Line_Follower_Array

This sketch shows how to use the circular buffer class to create a history of
sensor bar scans.
The buffer configured with CBUFFER_SIZE to have a length of 100 16bit integers.

Resources:
sensorbar.h

Development environment specifics:
arduino > v1.6.4
hw v1.0

This code is released under the [MIT License](http://opensource.org/licenses/MIT).
Please review the LICENSE.md file included with this example. If you have any questions 
or concerns with licensing, please contact techsupport@sparkfun.com.
Distributed as-is; no warranty is given.
******************************************************************************/
#define CBUFFER_SIZE 100

#include "Wire.h"
#include "sensorbar.h"
#include <Adafruit_MotorShield.h>

// Create the motor shield object with the default I2C address
Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 
// Or, create it with a different I2C address (say for stacking)
// Adafruit_MotorShield AFMS = Adafruit_MotorShield(0x61); 

// Select which 'port' M1, M2, M3 or M4. In this case, M1
Adafruit_DCMotor *rightMotor = AFMS.getMotor(3); //(3);
// You can also make another motor on port M2
Adafruit_DCMotor *leftMotor = AFMS.getMotor(4); //(4);

// Uncomment one of the four lines to match your SX1509's address
//  pin selects. SX1509 breakout defaults to [0:0] (0x3E).
const uint8_t SX1509_ADDRESS = 0x3E;  // SX1509 I2C address (00)
//const byte SX1509_ADDRESS = 0x3F;  // SX1509 I2C address (01)
//const byte SX1509_ADDRESS = 0x70;  // SX1509 I2C address (10)
//const byte SX1509_ADDRESS = 0x71;  // SX1509 I2C address (11)

SensorBar mySensorBar(SX1509_ADDRESS);

CircularBuffer positionHistory(CBUFFER_SIZE);

//Define the states that the decision making machines uses:
#define IDLE_STATE 0
#define READ_LINE 1
#define GO_FORWARD 2
#define GO_LEFT 3
#define GO_RIGHT 4

int zeroes_cnt = 0;
int16_t lastPos = 0;

uint8_t rawValue = 0;


bool activateMotor = false;

boolean rawIsSet = false;

boolean motorOn=false;

void setup()
{
  
  pinMode(8, INPUT); 
  pinMode(9, INPUT); 
  Serial.begin(115200);  // start serial for output
  Serial.println("Program started.");
  Serial.println();
  
  //For this demo, the IR will only be turned on during reads.
  //mySensorBar.setBarStrobe();
  //Other option: Command to run all the time
  mySensorBar.clearBarStrobe();

  //Default dark on light
  mySensorBar.clearInvertBits();
  //Other option: light line on dark
  //mySensorBar.setInvertBits();
  
  uint8_t returnStatus = mySensorBar.begin();
  if(returnStatus)
  {
	  Serial.println("sx1509 IC communication OK");
  }
  else
  {
	  Serial.println("sx1509 IC communication FAILED!");
	  while(1);
  }
  Serial.println();

  AFMS.begin();  // create with the default frequency 1.6KHz
  //AFMS.begin(1000);  // OR with a different frequency, say 1KHz
  
  // Set the speed to start, from 0 (off) to 255 (max speed)
  
}

void loop()
{
  //Wait 50 ms
  //delay(50);


  //Get the data from the bar and save it to the circular buffer positionHistory.
  int temp = mySensorBar.getDensity();
  if( (temp < 4)&&(temp > 0) )
  {
    positionHistory.pushElement( mySensorBar.getPosition());
    rawValue = mySensorBar.getRaw();
  }
  else if (temp > 0)
  {   
    rawValue = mySensorBar.getRaw();
  }

  boolean isBitSet_0 = rawValue & (1 << 0);
  boolean isBitSet_1 = rawValue & (1 << 1);
  boolean isBitSet_2 = rawValue & (1 << 2);
  boolean isBitSet_3 = rawValue & (1 << 3);
  boolean isBitSet_4 = rawValue & (1 << 4);
  boolean isBitSet_5 = rawValue & (1 << 5);
  boolean isBitSet_6 = rawValue & (1 << 6);
  boolean isBitSet_7 = rawValue & (1 << 7);

  Serial.print("density: ");
  Serial.println(temp);

  Serial.print("isBitSet_0: ");
  Serial.println(isBitSet_0);
  
  Serial.print("isBitSet_1: ");
  Serial.println(isBitSet_1);
  
  Serial.print("isBitSet_2: ");
  Serial.println(isBitSet_2);
  
  Serial.print("isBitSet_3: ");
  Serial.println(isBitSet_3);
  
  Serial.print("isBitSet_4: ");
  Serial.println(isBitSet_4);
  
  Serial.print("isBitSet_5: ");
  Serial.println(isBitSet_5);

  Serial.print("isBitSet_6: ");
  Serial.println(isBitSet_6);
  
  Serial.print("isBitSet_7: ");
  Serial.println(isBitSet_7);

  int lenLeft = 0;
  int lenRight = 0;

  if(isBitSet_7 == 1) lenLeft += 1;
  if(isBitSet_6 == 1) lenLeft += 1;
  if(isBitSet_5 == 1) lenLeft += 1;
  if(isBitSet_4 == 1) lenLeft += 1;

  if(isBitSet_3 == 1) lenRight += 1;
  if(isBitSet_2 == 1) lenRight += 1;
  if(isBitSet_1 == 1) lenRight += 1;
  if(isBitSet_0 == 1) lenRight += 1;

  Serial.print("lenLeft: ");
  Serial.println(lenLeft);
  
  Serial.print("lenRight: ");
  Serial.println(lenRight);



  /*if (digitalRead(8) == HIGH)
  {
    activateMotor = true;
  }
  else if (digitalRead(8) == LOW){
    activateMotor = false;
  }*/

  
  
  if(digitalRead(8)==HIGH && !motorOn){
    motorOn=true;
    activateMotor = true;
    delay(1000);
  }
  if(digitalRead(8)==HIGH && motorOn){
    motorOn=false;
    activateMotor = false;
    
    delay(1000);
  }

  Serial.print("activate motor: ");
  Serial.println(activateMotor);

  if(activateMotor == true)
  {

    int16_t avePos = positionHistory.averageLast( 10 );
    rightMotor->setSpeed(150);
    leftMotor->setSpeed(150);

    if (temp == 0 && lenLeft > lenRight)
    {
      avePos = -51;
    }

    if (temp == 0 && lenRight > lenLeft)
    {
      avePos = 51;
    }

    Serial.print("avepos: ");
    Serial.println(avePos);
  
    if( avePos < -50 )
    {
      rightMotor->run(FORWARD);
      leftMotor->run(BACKWARD);
    }
    else if( avePos > 50 )
    {
      rightMotor->run(BACKWARD);
      leftMotor->run(FORWARD);
      
    }
    else
    {
      rightMotor->run(FORWARD);
      leftMotor->run(FORWARD);
    }
    
  }
  else
  {
    rightMotor->setSpeed(0);
    leftMotor->setSpeed(0);
  }

}






