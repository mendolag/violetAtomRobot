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

const int DELAY=50;

int currentState=STATE_LINE;



void setup() {
  // put your setup code here, to run once:
    Serial.begin(9600);

}


int readRFID(){
  int randNumber = random(1, 10)*10;
  int randNumber1 = random(0, 2);
  int randN=randNumber+randNumber1;
  return randN;
}

void stateLine(){}
void stateLabyrinth(){}
void stateComplexLabyrinth(){}
void stateSkittles(){}
void stateBasket(){}
void stateKeystone(){}
void stateRubble(){}
void stateSteps(){}
void statePickAndShift(){}

void loop() {
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



