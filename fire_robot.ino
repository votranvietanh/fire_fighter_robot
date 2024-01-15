#include <Servo.h>

int pos = 0 ;

const int sensorMin = 0;     // sensor minimum
const int sensorMax = 865;  // sensor maximum
int mode = 1;

int nextTurn = 1;
//flame sensor

#define flameSensormoveLeft A1 //Flame sensor 1 pin
#define flameSensorRight A5 // Flame sensor 2 pin
#define flameSensorAhead A3 // Flame sensor 3 pin

//
#define obs1Trigger A0             // left
#define obs1Echo  6
#define obs2Trigger  A2           // ahead
#define obs2Echo  7
#define obs3Trigger  A4           // Right
#define obs3Echo  8
int flag = 0;
long duration1;
int distance1;
long duration2;
int distance2;
long duration3;
int distance3;
const int rightDelay = 460 ;
const int leftDelay = 440;

char incomingValue = 0;

int servoPosTop = 0;

int servoPosBot = 90;

Servo servoT;  // Servo for X-axis
Servo servoB;  // Servo for Y-axis

int Relay = 11;
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

  servoT.attach(9);  // Pin 9 for servo X-axisa
  servoB.attach(10); // Pin 10 for servo Y-axis

  pinMode(motorPin1, OUTPUT);
  pinMode(motorPin2, OUTPUT);
  pinMode(enablePinA, OUTPUT);
  pinMode(motorPin3, OUTPUT);
  pinMode(motorPin4, OUTPUT);
  pinMode(enablePinB, OUTPUT);
  //  servoB.detach();
  pinMode(Relay, OUTPUT);
}

void loop() {
  if (Serial.available()) //if some date is sent, reads it and saves in state
  {
    incomingValue = Serial.read();
    Serial.println(incomingValue);
    processBluetoothCommand();

  }
  //
  if (incomingValue == 'M') //Auto
  {
    mode = 1;
  }
  else if (incomingValue == 'A')   //Manual
  {
    mode = 0;
  }

  if (mode == 0) {
    processBluetoothCommand();
    Serial.print("manual");
  }
  if (mode == 1) {

    Serial.println("Automatic");
        int a = analogRead(flameSensormoveLeft);
        int c = analogRead(flameSensorAhead);
        int b = analogRead(flameSensorRight);


        Serial.println(a);//trai
        Serial.println(c);//giua
        Serial.println(b);//phai
        delay(1000);
    

    long aheadDistance = measureDistance(obs1Trigger, obs1Echo);
    Serial.print("left Distance: ");
    Serial.print(aheadDistance);
    Serial.println(" cm");


    long moveLeftDistance = measureDistance(obs2Trigger, obs2Echo);
    Serial.print("ahead: ");
    Serial.print(moveLeftDistance);
    Serial.println(" cm");

    long rightDistance = measureDistance(obs3Trigger, obs3Echo);
    Serial.print("Right Distance: ");
    Serial.print(rightDistance);
    Serial.println(" cm");

    Serial.println("__");
    ////    delay(2000);
    automatic();
  }

}
long measureDistance(int triggerPin, int echoPin) {
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  long distance = duration * 0.034 / 2;  // Tính khoảng cách dựa trên thời gian và vận tốc âm thanh

  return distance;
}
void processBluetoothCommand() {

  Serial.print("Trong blue");
  Serial.println(incomingValue);
  switch (incomingValue) {
    case '1':
      Serial.println("Forward-1");
      moveForward1();
      break;
    case '2':
      moveBackward();
      break;
    case '3':
      Serial.println("moveLeft");
      moveLeft();
      break;
    case '4':
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


const int MIN_OBSTACLE_DISTANCE = 19;
const int LEFT_TURN_DELAY = leftDelay;
const int RIGHT_TURN_DELAY = rightDelay;

void stopAndDelay(int delayTime) {
  stopMotors();
  delay(delayTime);
}

void turnLeft(int delayTime) {
  moveLeft();
  delay(delayTime);
}

void turnRight(int delayTime) {
  moveRight();
  delay(delayTime);
}

void automatic() {
  if (obstacleAhead())
  {
    stopMotors();
    Serial.println("Brake in obstacle Ahead");

    // RẼ TRÁI
    if (obstacleRight() && !obstacleLeft()) { //  bên phải chặn, bên trái trống -> rẽ trái
      Serial.println("bên phải chặn, bên trái trống -> rẽ trái");
      turnLeft(LEFT_TURN_DELAY);
      moveForward();
      stopAndDelay(400);
      if (obstacleRight()) { // bên phải chặn -> rẽ trái
        turnLeft(LEFT_TURN_DELAY);
        nextTurn = 1;
      } else {
        turnRight(RIGHT_TURN_DELAY);
      }
      // RẼ PHẢI
    } else if (obstacleLeft() && !obstacleRight()) { // BÊN TRÁI BỊ CHẶN, BÊN PHẢI TRỐNG -> RẼ PHẢI
      Serial.println("BÊN TRÁI BỊ CHẶN, BÊN PHẢI TRỐNG -> RẼ PHẢI");
      turnRight(RIGHT_TURN_DELAY);
      moveForward();
      stopAndDelay(400);

      if (obstacleLeft()) { // TRÁI BỊ CHẶN -> RẼ PHẢI
        Serial.println("TRÁI BỊ CHẶN -> RẼ PHẢI");
        turnRight(RIGHT_TURN_DELAY);
        nextTurn = 0;
      } else {
        Serial.println("Rẽ trái");
        turnLeft(LEFT_TURN_DELAY);
      }

      // ĐI LÙI
    }
    else if (obstacleLeft() && obstacleRight())
    { // ĐI VÔ NGÕ HẸP
      Serial.println("lUI");
      moveBackward();
      delay(1000);

      if (obstacleRight()) // BÊN PHẢI BỊ CHẶN
      {
        Serial.print("rẽ trái do phải bị chặn");
        moveLeft();
        delay(leftDelay);
        moveForward();
        delay(500);
        moveRight();
        delay(rightDelay);
      } else if (obstacleLeft())
      { // bên trái bị chăn
        Serial.print("rẽ phải do trái bị chặn");
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
      if (nextTurn == 1)
      { // đã vượt qua vật cản bên phải,đã rẽ trái tránh -> rẽ phải đi tiếp
        turnRight(RIGHT_TURN_DELAY);
        moveForward();
        delay(500);
        stopMotors();
        Serial.println("brake in case #4");

        if (obstacleLeft()) {
          Serial.println("#4-1");
          turnRight(RIGHT_TURN_DELAY);
          nextTurn = 0; // trái bị chặn -> rẽ phải -> passed.
          Serial.println("trái bị chặn -> rẽ phải -> passed.");
        }
        else
        {
          Serial.println("#4-2");
          turnLeft(LEFT_TURN_DELAY);
          moveForward();
          stopAndDelay(850);
          stopMotors();
          turnLeft(LEFT_TURN_DELAY);
          moveForward();
          stopAndDelay(500);
          stopMotors();
          turnRight(RIGHT_TURN_DELAY);
        }
      } else if (nextTurn == 0) // rẽ trái đã vượt qua vật cản bên trái
      {
        Serial.println("#5  rẽ trái đã vượt qua vật cản bên trái");
        turnLeft(LEFT_TURN_DELAY);
        moveForward();
        stopMotors();
        Serial.println("brake in case #5");

        if (obstacleRight()) {
          Serial.println("#5-1");
          moveLeft();
          delay(leftDelay);
          nextTurn = 1;
        } else {
          Serial.println("#5-2");
          turnRight(RIGHT_TURN_DELAY);
          moveForward();
          stopAndDelay(850);
          stopMotors();
          turnRight(RIGHT_TURN_DELAY);
          moveForward();
          stopAndDelay(500);
          stopMotors();
          turnLeft(LEFT_TURN_DELAY);
        }
      }
    }
  }
  else if (obstacleRight()&& !obstacleLeft() && !obstacleAhead())
    { 
      Serial.println("Có vật thể mạn phải");
      moveLeft();
      delay(1000);
      }
  else if (obstacleLeft()&& !obstacleRight() && !obstacleAhead())
    { 
      Serial.println("Có vật thể mạn trái");
      moveRight();
      delay(1000);
      }
  else
  {
    moveForward();
    Serial.println("#Không có vật cản-> đi thẳng");
  }
  delay(1000);
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
  Serial.println("Servo tang");
  pos = pos + 20;
  if (pos > limit) {
    pos = limit;
  }
  servoT.write(pos);
  Serial.println(pos);
  delay(15);
}
void decrementServoBPosition(int &pos, Servo & servoB, int limit) {
  Serial.println("Servo giam");
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
  delay(10);
}

void onPump() {
  digitalWrite(Relay, HIGH);
  delay(10);
}

void moveLeft() {
  Serial.println("hàm rẽ trái ,Left");
  digitalWrite(motorPin1, HIGH);
  digitalWrite(motorPin2, LOW);
  digitalWrite(motorPin3, HIGH);
  digitalWrite(motorPin4, LOW);
}

void moveRight() {
  Serial.println("rẽ phải, hàm,Right");
  digitalWrite(motorPin1, LOW);
  digitalWrite(motorPin2, HIGH);
  digitalWrite(motorPin3, LOW);
  digitalWrite(motorPin4, HIGH);
}

const int OBS_TRIGGER = obs2Trigger; // Ahead only
const int OBS_ECHO = obs2Echo; 

void moveForward() {
  // Check for fire in different directions


  bool fire1 = fireAhead();
  bool fire2 = fireLeft();
  bool fire3 = fireRight();

  // Check for obstacle
  digitalWrite(OBS_TRIGGER, LOW);
  delayMicroseconds(2);
  digitalWrite(OBS_TRIGGER, HIGH);
  delayMicroseconds(10);
  digitalWrite(OBS_TRIGGER, LOW);

  long duration = pulseIn(OBS_ECHO, HIGH);

  long cmMiddle = duration * 0.034 / 2;

  Serial.print("Obstacle Middle: ");
  Serial.println(cmMiddle);

  if (cmMiddle <= 19) {
    stopMotors();
    Serial.println("Stop, Trước có vật cản.");
    // Consider a non-blocking delay approach if necessary
    delay(3000);
    Serial.println("Chờ");
    return;
  } 
    else {
    // Move forward
    Serial.println("FOWARD");
    digitalWrite(motorPin1, LOW);
    digitalWrite(motorPin2, HIGH);
    digitalWrite(motorPin3, HIGH);
    digitalWrite(motorPin4, LOW);
  }
}
void moveForward1() {
  Serial.print("FOWARD");
  digitalWrite(motorPin1, LOW);
  digitalWrite(motorPin2, HIGH);
  digitalWrite(motorPin3, HIGH);
  digitalWrite(motorPin4, LOW);
}
void moveBackward() {
  Serial.println("Back");
  digitalWrite(motorPin1, HIGH);
  digitalWrite(motorPin2, LOW);
  digitalWrite(motorPin3, LOW);
  digitalWrite(motorPin4, HIGH);
}

void stopMotors() {
  Serial.println("S");
  digitalWrite(motorPin1, LOW);
  digitalWrite(motorPin2, LOW);
  digitalWrite(motorPin3, LOW);
  digitalWrite(motorPin4, LOW);
  delay(800);
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
void extinguishFire() {
  servoB.attach(10);
  onPump();
  for (int pos = 90; pos <= 100; pos += 1) {
    servoB.write(pos);
    delay(10);
  }

  for (int pos = 100; pos >= 90; pos -= 1) {
    servoB.write(pos);
    delay(10);
  }
  offPump();
  servoB.detach();
}

bool fireDetection(int flameSensorPin, int threshold, int rangeThreshold, int moveDelay) {
  int sensorReading = analogRead(flameSensorPin);
  int range = map(sensorReading, sensorMin, sensorMax, 0, 10);

  Serial.print("flameReading ");
  Serial.print(" is : ");
  Serial.println(sensorReading);
  Serial.println(range);

  if (range <= rangeThreshold) {
    Serial.println("Fire detected!!!");

    stopMotors();

    while (range <= 7) {
      stopMotors();
      Serial.println("it's range <= 8");
      extinguishFire();
      int newReading = analogRead(flameSensorPin);
      range = map(newReading, sensorMin, sensorMax, 0, 10);
    }

    Serial.print("Range after detection < 8: ");
    Serial.println(range);

    extinguishFire();
    return true;
  } else {
    return false;
  }
}

bool fireAhead() {
  int sensorReading;
  sensorReading = analogRead(flameSensorAhead);
  int range = map(sensorReading, sensorMin, sensorMax, 0, 10);
  //  Serial.println(sensorReading);
  //  Serial.println(range);
  if (sensorReading <= 45)
  {
    Serial.println("Fire Ahead!!!");
    Serial.println(sensorReading);
    stopMotors();
    while (range <= 😎
    {
      stopMotors();
      Serial.println("In the loop");
      sensorReading = analogRead(flameSensorAhead);
      range = map(sensorReading, sensorMin, sensorMax, 0, 10);
      servoB.attach(9);
      if (flag == 0) {
        onPump();
        for (pos = 90; pos <= 100; pos += 1) { // goes from 0 degrees to 180 degrees
          // in steps of 1 degree
          servoB.write(pos);              // tell servo to go to position in variable 'pos'
          delay(10);                       // waits 15ms for the servo to reach the position
        }
        for (pos = 100; pos >= 90; pos -= 1) { // goes from 180 degrees to 0 degrees
          servoB.write(pos);              // tell servo to go to position in variable 'pos'
          Serial.println(pos);
          delay(10);                       // waits 15ms for the servo to reach the position
        }
        flag = 1;
      }
      //      Serial.println(range);

    }
    //        stop spraying
    flag = 0;
    offPump();
    servoB.detach();
    return true;

  }
  else {
    return false;
  }

}


bool fireLeft() {
  int sensorReading;
  sensorReading = analogRead(flameSensormoveLeft);
  int rangeLeft = map(sensorReading, sensorMin, sensorMax, 0, 10);
  //  Serial.println(sensorReading);
  //  Serial.println(rangeLeft);
  if (rangeLeft <= 6)
  {
    Serial.println("Fire Left!!!");
    Serial.println(sensorReading);
    stopMotors();
    moveLeft();
    delay(leftDelay);
    stopMotors();
    moveBackward();
    delay(450);
    stopMotors();
    while (true) {
      int sensorReading;
      sensorReading = analogRead(flameSensormoveLeft);
      int rangeLeft = map(sensorReading, sensorMin, sensorMax, 0, 10);
      //      Serial.println(sensorReading);
      //      Serial.println("rangeLeft :");
      //      Serial.println(rangeLeft);
      //      Serial.println("_");

      delay(500);
      //left();
      if (rangeLeft == 0)
      {
        stopMotors();
        while (rangeLeft <= 😎
        {
          //          Serial.println(sensorReading);
          //          Serial.println("22rangeLeft :");
          //          Serial.println(rangeLeft);
          sensorReading = analogRead(flameSensormoveLeft);
          rangeLeft = map(sensorReading, sensorMin, sensorMax, 0, 10);
          //          start spraying
          onPump();
          servoB.attach(9);
          if (flag == 0) {
            for (pos = 90; pos <= 100; pos += 1) { // goes from 0 degrees to 180 degrees
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
        offPump();
        servoB.detach();

        return true;

      }

    }

  }
  else
    return false;

}

bool fireRight() {
  int sensorReading;
  sensorReading = analogRead(flameSensorRight);
  int range = map(sensorReading, sensorMin, sensorMax, 0, 10);
  //  Serial.println(sensorReading);
  //  Serial.println(range);
  if (range <= 6)
  {
    Serial.println("Fire Right!!!");
    Serial.println(sensorReading);
    stopMotors();
    delay(100);
    moveRight();
    delay(rightDelay);
    stopMotors();
    moveBackward();
    delay(450);
    while (true) {
      int sensorReading1;
      sensorReading1 = analogRead(flameSensorRight);
      int range1 = map(sensorReading1, sensorMin, sensorMax, 0, 10);
      //Serial.println(sensorReading1);
      Serial.println("Fire ahead");
      //      Serial.println(range1);
      //right();
      if (range1 == 0)
      {
        stopMotors();

        while (range1 <= 😎
        {
          moveForward();
          sensorReading1 = analogRead(flameSensorRight);
          range1 = map(sensorReading1, sensorMin, sensorMax, 0, 10);
          if (range1 <= 6)
          {
            stopMotors();
            break;
          }
        }
        while (range1 <= 😎
        {
          sensorReading1 = analogRead(flameSensorRight);
          range1 = map(sensorReading1, sensorMin, sensorMax, 0, 10);
          //          start spraying
          onPump();
          servoB.attach(9);
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
        offPump();
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
bool obstacleSensor(String positions, int obsTrigger, int obsEcho, int threshold) {
  bool fire1 = fireAhead();
  bool fire3 = fireRight();
  bool fire2 = fireLeft();

  digitalWrite(obsTrigger, LOW);
  delayMicroseconds(2);
  digitalWrite(obsTrigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(obsTrigger, LOW);

  long duration = pulseIn(obsEcho, HIGH);
  long distance = duration * 0.034 / 2;  // Tính khoảng cách dựa trên thời gian và vận tốc âm thanh

  Serial.print("Obstacle distance ");
  Serial.print(positions);
  Serial.print(" is ");
  Serial.println(distance);

  return (distance <= threshold);
}

bool obstacleAhead() {
  return obstacleSensor("truoc", obs2Trigger, obs2Echo, 46);
}

bool obstacleLeft() {
  return obstacleSensor("trai", obs1Trigger, obs1Echo, 50);
}

bool obstacleRight() {
  return obstacleSensor("phai", obs3Trigger, obs3Echo, 50);
}
