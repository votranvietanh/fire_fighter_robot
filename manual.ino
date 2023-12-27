#include <Servo.h>

int pos = 0 ;

const int sensorMin = 0;     // sensor minimum
const int sensorMax = 1024;  // sensor maximum
const int mode = 1;
const int smokeSensor = A0;
//flame sensor
const int flameSensorLeft = A1;//Flame sensor 1 pin
const int flameSensorRight = A2; // Flame sensor 2 pin
const int flameSensorAhead = A3; // Flame sensor 3 pin
const int threshold = 1000;
//
const int trig = 7;             // Ultrasonic sensor trig pin
const int echo = 6;

int flag = 0;
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
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);

  servoT.attach(9);  // Pin 9 for servo X-axis
  servoB.attach(10); // Pin 10 for servo Y-axis

  pinMode(motorPin1, OUTPUT);
  pinMode(motorPin2, OUTPUT);
  pinMode(enablePinA, OUTPUT);
  pinMode(motorPin3, OUTPUT);
  pinMode(motorPin4, OUTPUT);
  pinMode(enablePinB, OUTPUT);

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
        Serial.println("Left");
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
  int flameReadingLeft = analogRead(flameSensorLeft);
  int flameReadingRight = analogRead(flameSensorRight);
  int flameReadingAhead = analogRead(flameSensorAhead);

  int averageReading = (flameReadingLeft + flameReadingRight + flameReadingAhead) / 3;

  if (averageReading < threshold) {
    //    Serial.println("Fire detected!");
    Serial.print("Flame Sensor left: ");
    Serial.print(flameReadingLeft);
    Serial.print(", Sensor right: ");
    Serial.print(flameReadingRight);
    Serial.print(", Sensor ahead: ");
    Serial.println(flameReadingAhead);

    // Perform actions for firefighting robot (e.g., activate water pump, move to extinguish fire)
    // Add your firefighting robot control logic here.

  } else {
    Serial.println("No fire detected.");
  }
  if ( flameReadingRight < 500) {
    Serial.println("Fire at Right");
    moveRight();
    delay(200);
  } else if (flameReadingLeft < 500) {
    moveLeft();
    Serial.println("Fire at left");
  }
  else if (flameReadingAhead < 500) {
    Serial.println("Fire ahead");
    moveForward();
  }
  else  Serial.println("No fire");
  // Ultrasonic distance measurement
  unsigned long duration;
  int distance;
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(5);
  digitalWrite(trig, LOW);
  duration = pulseIn(echo, HIGH);
  distance = duration / 58.2;
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
  digitalWrite(motorPin1, LOW);
  digitalWrite(motorPin2, HIGH);
  digitalWrite(motorPin3, HIGH);
  digitalWrite(motorPin4, LOW);
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

  int flameReadingLeft = analogRead(flameSensorLeft);
  int flameReadingRight = analogRead(flameSensorRight);
  int flameReadingAhead = analogRead(flameSensorAhead);

  //int range = map(sensorReading, sensorMin, sensorMax, 0, 10);
  int averageReading = (flameReadingLeft + flameReadingRight + flameReadingAhead) / 3;

  if (averageReading < threshold) {
    //    Serial.println("Fire detected!");
    Serial.print("Flame Sensor left: ");
    Serial.print(flameReadingLeft);
    Serial.print(", Sensor right: ");
    Serial.print(flameReadingRight);
    Serial.print(", Sensor ahead: ");
    Serial.println(flameReadingAhead);

    // Perform actions for firefighting robot (e.g., activate water pump, move to extinguish fire)
    // Add your firefighting robot control logic here.

  } else {
    Serial.println("No fire detected.");
  }
  if ( flameReadingRight < 500) {
    Serial.println("Fire at Right");
  } else if (flameReadingLeft < 500)
    Serial.println("Fire at left");
  else if (flameReadingAhead < 500)
    Serial.println("Fire ahead");
  else  Serial.println("No fire");
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
  //  Serial.println(range);
  //  digitalWrite(buzzer, HIGH);
}
//        stop spraying
//delay(10000);
//flag = 0;
//servoB.detach();
//       myservo.write(100);
//digitalWrite(buzzer, LOW);
//return true;
//
//}
//else {
//  return false;
//}
