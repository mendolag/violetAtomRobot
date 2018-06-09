
#include <Servo.h> 
#include "Wire.h"
#include "sensorbar.h"
#include <Adafruit_MotorShield.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Encoder.h>
#include "utils.h"


const int STATE_LINE=10;
const int STATE_INIT_LABYRINTH=20;
const int STATE_INIT_COMPLEX_LABYRINTH=30;
const int STATE_INIT_SKITTLES=40;
const int STATE_INIT_BASKET=50;
const int STATE_INIT_KEYSTONE=60;
const int STATE_INIT_RUBBLE=70;
const int STATE_INIT_STEPS=80;
const int STATE_INIT_PICK_AND_SHIFT=90;

const int STATE_END_LABYRINTH=21;
const int STATE_END_COMPLEX_LABYRINTH=31;
const int STATE_END_SKITTLES=41;
const int STATE_END_BASKET=51;
const int STATE_END_KEYSTONE=61;
const int STATE_END_RUBBLE=71;
const int STATE_END_STEPS=81;
const int STATE_END_PICK_AND_SHIFT=91;


#define IDLE_STATE 0
#define GO_FORWARD 2
#define GO_LEFT 3
#define GO_RIGHT 4
#define GO_BACKWARD 5
#define GO_LEFT_FAR 8
#define GO_RIGHT_FAR 9
#define READ_DISTANCE 6
#define READ_MORE 7

const int DELAY=50;

int currentState=STATE_LINE;

//Motor 
Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 
Adafruit_DCMotor *rightMotor = AFMS.getMotor(3);
Adafruit_DCMotor *leftMotor = AFMS.getMotor(4);

//Encoder;
Encoder encoder(6,3);
long oldPosition  = -999;

//Line
int zeroes_cnt = 0;
int16_t lastPos = 0;
uint8_t rawValue = 0;
bool activateMotor = false;
boolean motorOn=false;

//ARRAY
const uint8_t SX1509_ADDRESS = 0x3E;
SensorBar mySensorBar(SX1509_ADDRESS);
#define CBUFFER_SIZE 100
CircularBuffer positionHistory(CBUFFER_SIZE);

//ARM 
Servo servoArm;
int armDown=180;
int armUp=40;
boolean isDown;

//RFID 
constexpr uint8_t RST_PIN = 5;          // Configurable, see typical pin layout above
constexpr uint8_t SS_PIN = 10; // Configurable, see typical pin layout above

//SENSORS
int state = IDLE_STATE;

const int DIM_BUFFER = 5;

CircularBuffer bufferCenter(DIM_BUFFER);
CircularBuffer bufferRight(DIM_BUFFER);
CircularBuffer bufferLeft(DIM_BUFFER);
double const mult=0.0048828125;
int const distanceThreshold=7;
#define sensorRight A0 
#define sensorCenter A1
#define sensorLeft A2
#define margin 7

double distanceRight;
double distanceCenter;
double distanceLeft;
int topSpeed=100;
int minCenter=30;
int labirinthSpeed=100;


MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance

void setup() {
  // put your setup code here, to run once:
    Serial.begin(115200);
    pinMode(8, INPUT); 
    pinMode(sensorRight, INPUT);
    pinMode(sensorCenter, INPUT);
    pinMode(sensorLeft, INPUT);

    servoArm.attach(9);
    isDown=false;

    AFMS.begin();
    
    //ARRAY
    mySensorBar.clearBarStrobe();
    mySensorBar.clearInvertBits();
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
    while (!Serial);    // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
  SPI.begin();      // Init SPI bus
  mfrc522.PCD_Init();   // Init MFRC522
  mfrc522.PCD_DumpVersionToSerial();  // Show details of PCD - MFRC522 Card Reader details
  Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));
  lowerArm();
}

void stopMotors(){
    rightMotor->setSpeed(0);
    leftMotor->setSpeed(0);
    rightMotor->run(RELEASE);
    leftMotor->run(RELEASE);
}

double get_IR (uint16_t value) {
        if (value < 16)  value = 16;
        return 2076.0 / (value - 11.0);
}

void liftArm(){
  if(!isDown){
      servoArm.write(armUp);
   for(int pos=armUp; pos!=armDown; pos++){
        servoArm.write(pos);
        delay(20);
      }
    isDown=true;
    }

  }

void lowerArm(){
    if(isDown){
      servoArm.write(armDown);
      for(int pos=armDown; pos!=armUp; pos--){
        servoArm.write(pos);
        delay(20);
      }
      isDown=false;
    }
  }

int readRFID(){
  Serial.println("readRFID");
if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  // Dump debug info about the card; PICC_HaltA() is automatically called
//  mfrc522.PICC_DumpToSerial(&(mfrc522.uid));
  long uid = 0;
  uid += (long) mfrc522.uid.uidByte[0] << 24;
  uid += (long) mfrc522.uid.uidByte[1] << 16;
  uid += (long) mfrc522.uid.uidByte[2] << 8;
  uid += (long) mfrc522.uid.uidByte[3];
  
  if(currentState==STATE_LINE){
   if(uid==0x35D68628 || uid==0x3571D528 ){
    stopMotors();
    liftArm();
    currentState=STATE_INIT_LABYRINTH;
   }else if(uid==0xC529FB35){
    stopMotors();
    liftArm();
    currentState=STATE_INIT_SKITTLES;
   }else if(uid==0x5E3DFA35){
    stopMotors();
    liftArm();
    currentState=STATE_INIT_KEYSTONE;
   }else if(uid==0xB7150665){
    stopMotors();
    liftArm();
    currentState=STATE_INIT_BASKET;
   }else if(uid==0xE7F6F735){
    stopMotors();
    liftArm();
    currentState=STATE_INIT_SKITTLES;
   }else if(uid==0xB397FA35){
    liftArm();
    currentState=STATE_INIT_STEPS;
   }else if(uid==0xC7C70D65){
    stopMotors();
    liftArm();
    currentState=STATE_INIT_PICK_AND_SHIFT;
   }else{
    stopMotors();
    lowerArm();
    currentState=STATE_LINE;
   }
   }else{
    stopMotors();
    lowerArm();
    currentState=STATE_LINE;
   }
//   Serial.println(uid);
//   Serial.println(currentState);
}

void stateLine(){
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

//  Serial.print("density: ");
//  Serial.println(temp);
//
//  Serial.print("isBitSet_0: ");
//  Serial.println(isBitSet_0);
//  
//  Serial.print("isBitSet_1: ");
//  Serial.println(isBitSet_1);
//  
//  Serial.print("isBitSet_2: ");
//  Serial.println(isBitSet_2);
//  
//  Serial.print("isBitSet_3: ");
//  Serial.println(isBitSet_3);
//  
//  Serial.print("isBitSet_4: ");
//  Serial.println(isBitSet_4);
//  
//  Serial.print("isBitSet_5: ");
//  Serial.println(isBitSet_5);
//
//  Serial.print("isBitSet_6: ");
//  Serial.println(isBitSet_6);
//  
//  Serial.print("isBitSet_7: ");
//  Serial.println(isBitSet_7);

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

  
  
  /*if(digitalRead(8)==HIGH && !motorOn){
    motorOn=true;
    activateMotor = true;
    delay(1000);
  }
  if(digitalRead(8)==HIGH && motorOn){
    motorOn=false;
    activateMotor = false;
    
    delay(1000);
  }*/

  
//  if (digitalRead(8) == HIGH)
//  {
//    activateMotor = true;
//  }
//  else if (digitalRead(8) == LOW){
//    activateMotor = false;
//  }
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
void stateLabyrinth(){
    rightMotor->setSpeed(topSpeed);
    leftMotor->setSpeed(topSpeed);
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
//    Serial.print("far");
    break;
  }else if(distanceCenter<minCenter  && distanceCenter < distanceRight-distanceThreshold && 
  distanceRight > distanceLeft ){
    state=GO_RIGHT_FAR;
//    Serial.print("far");

    break;
  }else{
     if(distanceLeft<margin && distanceRight<margin){
      if(distanceLeft<distanceRight){
        state=GO_RIGHT;
      }else{
        state=GO_LEFT;
      }
//    Serial.print("near");
    }else if(distanceLeft<margin){
      state=GO_RIGHT;
//      Serial.print("near");
    }
    else if(distanceRight<margin){
      state=GO_LEFT;
//      Serial.print("near");

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
//      Serial.print("far");

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
      state=READ_MORE;
      break;
    case GO_RIGHT_FAR:
      rightMotor->run(BACKWARD);
      leftMotor->run(FORWARD);
      state=READ_MORE;
      break;
    case GO_LEFT:
      rightMotor->run(FORWARD);
      leftMotor->run(BACKWARD);
      state=READ_DISTANCE;
      moveUntil(300);
      break;
    case GO_RIGHT:
      rightMotor->run(BACKWARD);
      leftMotor->run(FORWARD);
      state=READ_DISTANCE;
      moveUntil(300);
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
    
  }
void stateComplexLabyrinth(){}
void stateSkittles(){
  runForward(150);
  }
void stateBasket(){runForward(80);}
void stateKeystone(){
  runForward(180);}
void stateRubble(){
  runForward(200);
  }
void stateSteps(){runForward(200);}
void statePickAndShift(
  runForward(150);
  ){}


void runForward(int speed){
    rightMotor->run(FORWARD);
    leftMotor->run(FORWARD);
    rightMotor->setSpeed(200);
    leftMotor->setSpeed(200);
}

void loop() {
  if(digitalRead(8) == HIGH){
    stopMotors();
    return;
  }
  currentState=readRFID();
  if(currentState==STATE_INIT_LABYRINTH){
    Serial.println("STATE_INIT_LABYRINTH");
    while(currentState!=STATE_END_LABYRINTH){
      stateLabyrinth();
      currentState=readRFID();
      delay(DELAY);
     }
    Serial.println("STATE_END_LABYRINTH");
  }
  else if(currentState==STATE_INIT_COMPLEX_LABYRINTH){
    Serial.println("STATE_INIT_COMPLEX_LABYRINTH");
    while(currentState!=STATE_END_COMPLEX_LABYRINTH){
      stateComplexLabyrinth();
      currentState=readRFID();
      delay(DELAY);
     }
     Serial.println("STATE_END_COMPLEX_LABYRINTH");
  }
  else if(currentState==STATE_INIT_SKITTLES){
    Serial.println("STATE_INIT_SKITTLES");
    while(currentState!=STATE_END_SKITTLES){
      stateSkittles();
      currentState=readRFID();
      delay(DELAY);
     }
     Serial.println("STATE_END_SKITTLES");
  }
  else if(currentState==STATE_INIT_BASKET){
    Serial.println("STATE_INIT_BASKET");
    while(currentState!=STATE_END_BASKET){
      stateBasket();
      currentState=readRFID();
      delay(DELAY);
     }
     Serial.println("STATE_END_BASKET");
  }
  else if(currentState==STATE_INIT_KEYSTONE){
    Serial.println("STATE_INIT_KEYSTONE");
    while(currentState!=STATE_END_KEYSTONE){
      stateKeystone();
      currentState=readRFID();
      delay(DELAY);
     }
     Serial.println("STATE_END_KEYSTONE");
  }
  else if(currentState==STATE_INIT_RUBBLE){
    Serial.println("STATE_INIT_RUBBLE");
    while(currentState!=STATE_END_RUBBLE){
      stateSteps();
      currentState=readRFID();
      delay(DELAY);
     }
     Serial.println("STATE_END_RUBBLE");
  }
  else if(currentState==STATE_INIT_STEPS){
    Serial.println("STATE_INIT_STEPS");
    while(currentState!=STATE_END_STEPS){
      stateSteps();
      currentState=readRFID();
      delay(DELAY);
     }
     Serial.println("STATE_END_STEPS");
  }
  else if(currentState==STATE_INIT_PICK_AND_SHIFT){
    Serial.println("STATE_INIT_PICK_AND_SHIFT");
    while(currentState!=STATE_END_PICK_AND_SHIFT){
      statePickAndShift();
      currentState=readRFID();
      delay(DELAY);
     }
     Serial.println("STATE_END_PICK_AND_SHIFT");
  }
  else{
    Serial.println("STATE_LINE");
    stateLine();
  }
   delay(DELAY);
}



