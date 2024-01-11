#include <Servo.h>

int pos = 0 ;

const int sensorMin = 0;     // sensor minimum
const int sensorMax = 1024;  // sensor maximum
int mode = 1;
const int smokeSensor = A0;
int nextTurn = 1;
//flame sensor
const int flameSensormoveLeft = A3;//Flame sensor 1 pin
const int flameSensorRight = A4; // Flame sensor 2 pin
const int flameSensorAhead = A5; // Flame sensor 3 pin
const int threshold = 1000;
//
const int obs1Trigger = A0;             // Ahead
const int obs1Echo = 11;
const int obs2Trigger = A1;             // moveLeft
const int obs2Echo = 12;
const int obs3Trigger = A2;             // Right
const int obs3Echo = 13;
int flag = 0;

const int rightDelay = 460 ;
const int leftDelay = 440;

char incomingValue = 0;

int servoPosTop = 0;

int servoPosBot = 90;

Servo servoT;  // Servo for X-axis
Servo servoB;  // Servo for Y-axis

int Relay = 8;
// Motor A
const int motorPin1 = 2;  // Connect to the input pin 1 of the L298N for Motor A
const int motorPin2 = 3;  // Connect to the input pin 2 of the L298N for Motor A
const int enablePinA = 9; // Connect to the enable pin of the L298N for Motor A

// Motor B
const int motorPin3 = 4;  // Connect to the input pin 3 of the L298N for Motor B
const int motorPin4 = 5;  // Connect to the input pin 4 of the L298N for Motor B
const int enablePinB = 10; // Connect to the enable pin of the L298N for Motor B

void setup() {
  Serial.begin(9600);


  //ultrasonic
  pinMode(obs1Trigger, OUTPUT);
  pinMode(obs1Echo, INPUT);
  pinMode(obs2Trigger, OUTPUT);
  pinMode(obs2Echo, INPUT);
  pinMode(obs3Trigger, OUTPUT);
  pinMode(obs3Echo, INPUT);

  servoT.attach(9);  // Pin 9 for servo X-axis
  servoB.attach(10); // Pin 10 for servo Y-axis

  pinMode(motorPin1, OUTPUT);
  pinMode(motorPin2, OUTPUT);
  pinMode(enablePinA, OUTPUT);
  pinMode(motorPin3, OUTPUT);
  pinMode(motorPin4, OUTPUT);
  pinMode(enablePinB, OUTPUT);
  servoB.detach();
  pinMode(Relay, OUTPUT);
}

void loop() {
  if (Serial.available() > 0) //if some date is sent, reads it and saves in state
  {
    incomingValue = Serial.read();
  }

  if (incomingValue == 'A') //Auto
  {
    mode = 1;
  }
  else if (incomingValue == 'M')   //Manual
  {
    mode = 0;
  }

  if (mode == 0) {
    processBluetoothCommand();
  }
  if (mode == 1) {
    automatic();
  }
}


void processBluetoothCommand() {
  if (Serial.available()) {
    incomingValue = Serial.read();
    switch (incomingValue) {
      case '1':
        Serial.println("Forward");
        moveForward();
        break;
      case '2':
        Serial.println("Backward");
        moveBackward();
        break;
      case '3':
        Serial.println("moveLeft");
        moveLeft();
        break;
      case '4':
        Serial.println("Right");
        moveRight();
        break;
      case '0':
        Serial.println("Stop");
        stopMotors();
        break;
      case 'U':
        Serial.println("U");
        incrementServoTPosition(servoPosTop, servoT, 180);
        break;
      case 'D':
        Serial.println("D");
        decrementServoTPosition(servoPosTop, servoT, 0);
        break;
      case 'L':
        Serial.println("L");
        incrementServoBPosition(servoPosBot, servoB, 180);
        break;
      case 'R':
        Serial.println("R");
        decrementServoBPosition(servoPosBot, servoB, 0);
        break;
      case 'T':
        Serial.println("Turn on");
        onPump();
        break;
      case 'F':
        Serial.println("Turn off");
        offPump();
        break;
      default:
        break;
    }

  }
  delay(10);
}
void automatic() {
  if (obstacleAhead())
  {
    stopMotors();
    Serial.println("Brake in obstacle Ahead");
    if (obstacleRight() && !obstacleLeft()) {
      Serial.println("#1");
      //moveLeft();
      moveLeft();
      delay(leftDelay);
      moveForward();
      delay(400);
      stopMotors();
      Serial.println("brake in case #1");
      if (obstacleRight())
      {
        moveLeft();
        delay(leftDelay);
        nextTurn = 1;
      }
      else {
        moveRight();
        delay(rightDelay);
      }
    }
    else if (obstacleLeft() && !obstacleRight())
    {
      Serial.println("#2");
      moveRight();
      delay(rightDelay);
      //nextTurn=0;
      moveForward();
      delay(400);
      stopMotors();
      Serial.println("brake in case #2");

      if (obstacleLeft())
      {
        Serial.println("#2-1");
        moveRight();
        delay(rightDelay);
        nextTurn = 0;
      }
      else {
        Serial.println("#2-2");
        moveLeft();
        delay(leftDelay);
      }
      //delay(2400);
    }
    else if (obstacleLeft() && obstacleRight())
    {
      Serial.println("#3");
      moveBackward();
      delay(1000);
      if (obstacleRight())
      {
        moveLeft();
        delay(leftDelay);
        moveForward();
        delay(500);
        moveRight();
        delay(rightDelay);
      }
      else if (obstacleLeft())
      {
        moveRight();
        delay(rightDelay);
        moveForward();
        delay(500);
        moveLeft();
        delay(leftDelay);
      }
    }
    else
    {
      if (nextTurn == 1) {
        Serial.println("#4");
        moveRight();
        delay(rightDelay);
        moveForward();
        delay(500);
        stopMotors();
        Serial.println("brake in case #4");
        if (obstacleLeft())
        {
          Serial.println("#4-1");
          moveRight();
          int del = rightDelay;
          delay(del);
          nextTurn = 0;
        }
        else {
          Serial.println("#4-2");
          moveLeft();
          delay(leftDelay);
          moveForward();
          delay(850);
          stopMotors();
          moveLeft();
          delay(leftDelay);
          moveForward();
          delay(500);
          stopMotors();
          moveRight();
          delay(rightDelay);
        }
      }
      else if (nextTurn == 0)
      {
        Serial.println("#5");
        moveLeft();
        delay(leftDelay);
        moveForward();
        delay(500);
        stopMotors();
        Serial.println("brake in case #5");
        if (obstacleRight())
        {
          Serial.println("#5-1");
          moveLeft();
          delay(leftDelay);
          nextTurn = 1;
        }
        else {
          Serial.println("#5-2");
          moveRight();
          delay(rightDelay);
          moveForward();
          delay(850);
          stopMotors();
          moveRight();
          delay(rightDelay);
          moveForward();
          delay(500);
          stopMotors();
          moveLeft();
          delay(leftDelay);
        }
      }
    }
  }
  else {
    moveForward();
    Serial.println("#4-1");
  }
  delay(1000);
}

long microsecondsToInches(long microseconds)
{
  return microseconds / 74 / 2;
}

long microsecondsToCentimeters(long microseconds)
{
  return microseconds / 29 / 2;
}


void incrementServoBPosition(int &pos, Servo & servoB, int limit) {
  pos = pos + 20;
  if (pos > limit) {
    pos = limit;
  }
  servoB.write(pos);
  Serial.println(pos);
  delay(15);
}
void incrementServoTPosition(int &pos, Servo & servoT, int limit) {
  pos = pos + 20;
  if (pos > limit) {
    pos = limit;
  }
  servoT.write(pos);
  Serial.println(pos);
  delay(15);
}
void decrementServoBPosition(int &pos, Servo & servoB, int limit) {
  pos = pos - 20;
  if (pos < limit) {
    pos = limit;
  }
  servoB.write(pos);
  Serial.println(pos);
  delay(15);

}
void decrementServoTPosition(int &pos, Servo & servoT, int limit) {
  pos = pos - 20;
  if (pos < limit) {
    pos = limit;
  }
  servoT.write(pos);
  Serial.println(pos);
  delay(15);
}
void offPump() {
  digitalWrite(Relay, LOW);
}

void onPump() {
  digitalWrite(Relay, HIGH);
}


void moveLeft() {
  digitalWrite(motorPin1, HIGH);
  digitalWrite(motorPin2, LOW);
  digitalWrite(motorPin3, HIGH);
  digitalWrite(motorPin4, LOW);
}

void moveRight() {
  digitalWrite(motorPin1, LOW);
  digitalWrite(motorPin2, HIGH);
  digitalWrite(motorPin3, LOW);
  digitalWrite(motorPin4, HIGH);
}

void moveForward() {
  bool fire1 = fireAhead();
  bool fire2 = fireLeft();
  bool fire3 = fireRight();

  analogWrite(obs1Trigger, 0);
  delayMicroseconds(2);

  analogWrite(obs1Trigger, 255) ;
  delayMicroseconds(10);

  analogWrite(obs1Trigger, 0) ;
  long duration, cmMiddle;
  duration = pulseIn(obs1Echo, HIGH);
  cmMiddle = microsecondsToCentimeters(duration);
  Serial.print("obstacle Middle *:  ");
  Serial.println(cmMiddle);
  if (cmMiddle <= 19)
  {
    stopMotors();
    Serial.println("Brake in forward()");
    delay(2000);
    Serial.println("DELAY OVER");
    return ;
  }
  else {
    digitalWrite(motorPin1, LOW);
    digitalWrite(motorPin2, HIGH);
    digitalWrite(motorPin3, HIGH);
    digitalWrite(motorPin4, LOW);
  }
}
void moveBackward() {
  digitalWrite(motorPin1, HIGH);
  digitalWrite(motorPin2, LOW);
  digitalWrite(motorPin3, LOW);
  digitalWrite(motorPin4, HIGH);
}

void stopMotors() {
  digitalWrite(motorPin1, LOW);
  digitalWrite(motorPin2, LOW);
  digitalWrite(motorPin3, LOW);
  digitalWrite(motorPin4, LOW);
}
void rotate()
{
  Serial.println("Rotate");
  digitalWrite(motorPin1, HIGH);
  digitalWrite(motorPin2, LOW);
  digitalWrite(motorPin3, HIGH);
  digitalWrite(motorPin4, LOW);
}

void sweepServo() {
  if (flag == 0) {
    for (pos = 90; pos <= 100; pos += 1) { // goes from 0 degrees to 180 degrees
      // in steps of 1 degree
      servoB.write(pos);              // tell servo to go to position in variable 'pos'
      delay(10);                       // waits 15ms for the servo to reach the position
    }
    for (pos = 100; pos >= 90; pos -= 1) { // goes from 180 degrees to 0 degrees
      servoB.write(pos);              // tell servo to go to position in variable 'pos'
      delay(10);                       // waits 15ms for the servo to reach the position
      //count++;
    }
    flag = 1;
  }
}
///
bool fireAhead() {

  int flameReadingAhead = analogRead(flameSensorAhead);

  int range = map(flameReadingAhead, sensorMin, sensorMax, 0, 10);
  Serial.print("flameReadingAhead: ");
  Serial.println(flameReadingAhead);
  Serial.println(range);
  if (flameReadingAhead <= 50)
  {
    Serial.println("Fire Ahead!!!");
    stopMotors();
    while (range <= 8)
    {
      stopMotors();
      Serial.println("In the loop");
      int flameReadingAhead = analogRead(flameSensorAhead);
      range = map(flameReadingAhead, sensorMin, sensorMax, 0, 10);
      servoB.attach(10);
      onPump()
      if (flag == 0) {
        for (pos = 90; pos <= 100; pos += 1) { // goes from 0 degrees to 180 degrees
          // in steps of 1 degree
          servoB.write(pos);              // tell servo to go to position in variable 'pos'
          delay(10);                       // waits 15ms for the servo to reach the position
        }
        for (pos = 100; pos >= 90; pos -= 1) { // goes from 180 degrees to 0 degrees
          servoB.write(pos);              // tell servo to go to position in variable 'pos'
          delay(10);                       // waits 15ms for the servo to reach the position
          //count++;
        }
        flag = 1;
      }
    }
    Serial.print("range sau dkien < 8");
    Serial.println(range);

    flag = 0;
    ///????
    offPump()
    servoB.detach();
    //       myservo.write(100);
    //      digitalWrite(buzzer, LOW);
    return true;

  }
  else {
    return false;
  }

}

bool fireLeft() {
  int sensorReading;
  sensorReading = analogRead(flameSensormoveLeft);
  int range = map(sensorReading, sensorMin, sensorMax, 0, 10);
  Serial.print("flameReadingLeft: ");
  Serial.println(sensorReading);
  Serial.println(range);
  if (range <= 6)
  {
    Serial.println("Fire moveLeft!!!");
    stopMotors();
    moveLeft();
    delay(leftDelay);
    stopMotors();
    moveBackward();
    delay(450);
    stopMotors();
    while (true) {
      int sensorReading;
      sensorReading = analogRead(flameSensorAhead);
      int range = map(sensorReading, sensorMin, sensorMax, 0, 10);
      Serial.println(sensorReading);
      Serial.println("Fire left");
      Serial.println(range);
      //moveLeft();
      if (range == 0)
      {
        stopMotors();
        while (range <= 8)
        {
          sensorReading = analogRead(flameSensorAhead);
          range = map(sensorReading, sensorMin, sensorMax, 0, 10);
          //          start spraying
          servoB.attach(10);
          onPump();
          if (flag == 0) {
            for (pos = 90; pos <= 100; pos += 1) { // goes from 0 degrees to 180 degrees
              // in steps of 1 degree
              servoB.write(pos);              // tell servo to go to position in variable 'pos'
              delay(10);                       // waits 15ms for the servo to reach the position
            }
            for (pos = 100; pos >= 90; pos -= 1) { // goes from 180 degrees to 0 degrees
              servoB.write(pos);              // tell servo to go to position in variable 'pos'
              delay(10);                       // waits 15ms for the servo to reach the position
              //count++;
            }
            flag = 1;
          }

        }
        //        stop spraying
        flag = 0;
        onPump()
        servoB.detach();
        //            digitalWrite(buzzer, LOW);
        return true;

      }
    }
  }
  else
    return false;

}

/// right
bool fireRight() {
  int sensorReading;
  sensorReading = analogRead(flameSensorRight);
  int range = map(sensorReading, sensorMin, sensorMax, 0, 10);
  Serial.print("flameReadingRight: ");
  Serial.println(sensorReading);
  Serial.println(range);
  if (range <= 6)
  {
    Serial.println("Fire Right!!!");
    stopMotors();
    moveRight();
    delay(rightDelay);
    stopMotors();
    moveBackward();
    delay(450);
    while (true) {
      int sensorReading1;
      sensorReading1 = analogRead(flameSensorAhead);
      int range1 = map(sensorReading1, sensorMin, sensorMax, 0, 10);
      Serial.println("Fire ahead");
      Serial.println(range1);
      //right();
      if (range1 == 0)
      {
        stopMotors();

        while (range1 <= 8)
        {
          moveForward();
          sensorReading1 = analogRead(flameSensorAhead);
          range1 = map(sensorReading1, sensorMin, sensorMax, 0, 10);
          if (range1 <= 6)
          {
            stopMotors();
            break;
          }
        }
        while (range1 <= 8)
        {
          sensorReading1 = analogRead(flameSensorAhead);
          range1 = map(sensorReading1, sensorMin, sensorMax, 0, 10);
          //          start spraying
          servoB.attach(10);
          onPump();
          if (flag == 0) {
            for (pos = 90; pos <= 100; pos += 1) { // goes from 0 degrees to 180 degrees
              // in steps of 1 degree
              servoB.write(pos);              // tell servo to go to position in variable 'pos'
              delay(10);                       // waits 15ms for the servo to reach the position
            }
            for (pos = 100; pos >= 90; pos -= 1) { // goes from 180 degrees to 0 degrees
              servoB.write(pos);              // tell servo to go to position in variable 'pos'
              delay(10);                       // waits 15ms for the servo to reach the position
              //count++;
            }
            flag = 1;
          }
        }
        //        stop spraying
        onPump()
        servoB.detach();
        if (range1 > 7) {
          stopMotors();
        }
        return true;
      }
    }
  }
  else
    return false;
}

//Obstacle
bool obstacleAhead() {
  bool fire1 = fireAhead();
  bool fire2 = fireLeft();
  bool fire3 = fireRight();
  analogWrite(obs1Trigger, 0);
  delayMicroseconds(2);
  analogWrite(obs1Trigger, 255) ;
  delayMicroseconds(10);
  analogWrite(obs1Trigger, 0) ;
  long duration, cmMiddle;
  duration = pulseIn(obs1Echo, HIGH);
  cmMiddle = microsecondsToCentimeters(duration);
  Serial.print("obstacle Middle: ");
  Serial.println(cmMiddle);


  if (cmMiddle <= 19)
  {
    return true;
  }
  return false;
}
bool obstacleLeft() {
  bool fire1 = fireAhead();
  bool fire2 = fireLeft();
  bool fire3 = fireRight();
  analogWrite(obs2Trigger, 0);
  delayMicroseconds(2);
  analogWrite(obs2Trigger, 255);
  delayMicroseconds(10);
  analogWrite(obs2Trigger, 0);
  long duration, cmLeft;
  duration = pulseIn(obs2Echo, HIGH);
  cmLeft = microsecondsToCentimeters(duration);
  Serial.print("obstacle Left: ");
  Serial.println(cmLeft);
  if (cmLeft <= 30)
  {
    return true;
  }
  return false;
}

bool obstacleRight() {
  bool fire1 = fireAhead();
  bool fire2 = fireLeft();
  bool fire3 = fireRight();
  analogWrite(obs3Trigger, 0);
  delayMicroseconds(2);
  analogWrite(obs3Trigger, 255);
  delayMicroseconds(10);
  analogWrite(obs3Trigger, 0);
  long duration, cmRight;
  duration = pulseIn(obs3Echo, HIGH);
  cmRight = microsecondsToCentimeters(duration);
  Serial.print("obstacle Right: ");
  Serial.println(cmRight);

  if (cmRight <= 30)
  {
    return true;
  }
  return false;
}
