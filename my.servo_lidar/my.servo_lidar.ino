//#include <Servo.h>


//Servo myservo;  // create servo object to control a servo
// twelve servo objects can be created on most boards

int pos = 0;    // variable to store the servo position

void setup() {
  Serial.begin(115200);
  //myservo.attach(9);
}


unsigned int readDistance() {
  while (Serial.available() >= 9)
  {
    if ((0x59 == Serial.read()) && (0x59 == Serial.read())) //Byte1 & Byte2
    {
      unsigned int t1 = Serial.read(); //Byte3
      unsigned int t2 = Serial.read(); //Byte4

      t2 <<= 8;
      t2 += t1;

      for (int i = 0; i < 5; i++)
      {
        Serial.read(); ////Byte5,6,7,8,9
      }
      if(t2>=25 && t2<=1300){
        return t2;
      }
    }
  }
  return 0;
}

void loop() {
  int read = digitalRead(8);
  Serial.print((String) "read: " + read);
  /*unsigned int distance = readDistance();
    Serial.print((String)"position " + pos + "\tdistance " + distance + "\n");
    delay(40);
    */
  /*for (pos = 45; pos <= 125; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    unsigned int distance = readDistance();
    Serial.print((String)"position " + pos + "\tdistance " + distance + "\n");
    delay(40);                       // waits 15ms for the servo to reach the position
  }
  for (pos = 125; pos >= 45; pos -= 1) { // goes from 180 degrees to 0 degrees
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    unsigned int distance = readDistance();
    Serial.print((String)"position " + pos + "\tdistance " + distance + "\n");
    delay(40);                       // waits 15ms for the servo to reach the position
  }
  */
}

