#include <Arduino.h>


#define LEFT_MOTOR_FORWARD_PIN 2  // Set it HIGH to move forward and LOW to move backward
#define LEFT_MOTOR_BACKWARD_PIN 3 // Set it HIGH to move backward and LOW to move forward
#define RIGHT_MOTOR_FORWARD_PIN 4
#define RIGHT_MOTOR_BACKWARD_PIN 5

// Front Ultrasonic sensors pins
#define LEFT_SR04_TRIG_PIN 6
#define LEFT_SR04_ECHO_PIN 7
#define MID_SR04_TRIG_PIN 8
#define MID_SR04_ECHO_PIN 9
#define RIGHT_SR04_TRIG_PIN 10
#define RIGHT_SR04_ECHO_PIN 11

// Side Ultrasonic sensor pins
#define SIDE_SR04_TRIG_PIN A0
#define SIDE_SR04_ECHO_PIN A1

// Line sensor pins
#define LEFT_LINE_SENSOR_PIN A2
#define RIGHT_LINE_SENSOR_PIN A3

#define BACKWARD_DELAY 1000
#define SIDE_ROTATE_DELAY 1000


void setup()
{
  Serial.begin(9600);
  pinMode(LEFT_MOTOR_FORWARD_PIN, OUTPUT);
  pinMode(LEFT_MOTOR_BACKWARD_PIN, OUTPUT);
  pinMode(RIGHT_MOTOR_FORWARD_PIN, OUTPUT);
  pinMode(RIGHT_MOTOR_BACKWARD_PIN, OUTPUT);

  pinMode(LEFT_SR04_TRIG_PIN, OUTPUT);
  pinMode(LEFT_SR04_ECHO_PIN, INPUT);
  pinMode(MID_SR04_TRIG_PIN, OUTPUT);
  pinMode(MID_SR04_ECHO_PIN, INPUT);
  pinMode(RIGHT_SR04_TRIG_PIN, OUTPUT);
  pinMode(RIGHT_SR04_ECHO_PIN, INPUT);
  pinMode(SIDE_SR04_TRIG_PIN, OUTPUT);
  pinMode(SIDE_SR04_ECHO_PIN, INPUT);

  pinMode(LEFT_LINE_SENSOR_PIN, INPUT);
  pinMode(RIGHT_LINE_SENSOR_PIN, INPUT);
}

void moveForward()
{
  digitalWrite(LEFT_MOTOR_FORWARD_PIN, HIGH);
  digitalWrite(LEFT_MOTOR_BACKWARD_PIN, LOW);
  digitalWrite(RIGHT_MOTOR_FORWARD_PIN, HIGH);
  digitalWrite(RIGHT_MOTOR_BACKWARD_PIN, LOW);
}

void moveBackward()
{
  digitalWrite(LEFT_MOTOR_FORWARD_PIN, LOW);
  digitalWrite(LEFT_MOTOR_BACKWARD_PIN, HIGH);
  digitalWrite(RIGHT_MOTOR_FORWARD_PIN, LOW);
  digitalWrite(RIGHT_MOTOR_BACKWARD_PIN, HIGH);
}

void turnLeft()
{
  digitalWrite(LEFT_MOTOR_FORWARD_PIN, LOW);
  digitalWrite(LEFT_MOTOR_BACKWARD_PIN, HIGH);
  digitalWrite(RIGHT_MOTOR_FORWARD_PIN, HIGH);
  digitalWrite(RIGHT_MOTOR_BACKWARD_PIN, LOW);
}

void turnRight()
{
  digitalWrite(LEFT_MOTOR_FORWARD_PIN, HIGH);
  digitalWrite(LEFT_MOTOR_BACKWARD_PIN, LOW);
  digitalWrite(RIGHT_MOTOR_FORWARD_PIN, LOW);
  digitalWrite(RIGHT_MOTOR_BACKWARD_PIN, HIGH);
}

void stop()
{
  digitalWrite(LEFT_MOTOR_FORWARD_PIN, LOW);
  digitalWrite(LEFT_MOTOR_BACKWARD_PIN, LOW);
  digitalWrite(RIGHT_MOTOR_FORWARD_PIN, LOW);
  digitalWrite(RIGHT_MOTOR_BACKWARD_PIN, LOW);
}

int getDistance(int trigPin, int echoPin)
{
  // Max distance is 50cm and speed of sound is 340m/s so the time travel twice the distance is 50cm * 2 / 340m/s = 2.94ms = 2940us
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH, 2940);
  int distance = duration * 0.034 / 2;
  return distance;
}

bool opponentDetected(int distance)
{
  if (distance != 0)
  {
    return true;
  }
  return false;
}

bool opponentInFront(bool leftDetected, bool midDetected, bool rightDetected)
{
  if (midDetected)
  {
    return true;
  }
  if (leftDetected && rightDetected)
  {
    return true;
  }
  return false;
}

bool opponentOnLeft(bool leftDetected, bool midDetected, bool rightDetected)
{
  if (leftDetected && !rightDetected)
  {
    return true;
  }
  return false;
}

bool opponentOnRight(bool leftDetected, bool midDetected, bool rightDetected)
{
  if (!leftDetected && rightDetected)
  {
    return true;
  }
  return false;
}

bool lineDetected(int leftPin, int rightPin)
{
  bool leftWhite = digitalRead(leftPin);
  bool rightWhite = digitalRead(rightPin);
  if (leftWhite || rightWhite)
  {
    return true;
  }
  return false;
}

void findOpponent()
{
  int leftDistance = getDistance(LEFT_SR04_TRIG_PIN, LEFT_SR04_ECHO_PIN);
  int midDistance = getDistance(MID_SR04_TRIG_PIN, MID_SR04_ECHO_PIN);
  int rightDistance = getDistance(RIGHT_SR04_TRIG_PIN, RIGHT_SR04_ECHO_PIN);
  int sideDistance = getDistance(SIDE_SR04_TRIG_PIN, SIDE_SR04_ECHO_PIN);

  bool leftDetected = opponentDetected(leftDistance);
  bool midDetected = opponentDetected(midDistance);
  bool rightDetected = opponentDetected(rightDistance);
  bool sideDetected = opponentDetected(sideDistance);

  if (opponentInFront(leftDetected, midDetected, rightDetected))
  {
    moveForward();
    return;
  }
  if (opponentOnLeft(leftDetected, midDetected, rightDetected))
  {
    turnLeft();
    return;
  }
  if (opponentOnRight(leftDetected, midDetected, rightDetected))
  {
    turnRight();
    return;
  }
  if (sideDetected)
  {
    turnLeft();
    delay(SIDE_ROTATE_DELAY);
    return;
  }
  turnRight();
}

void loop()
{
  if (lineDetected(LEFT_LINE_SENSOR_PIN, RIGHT_LINE_SENSOR_PIN))
  {
    moveBackward();
    delay(BACKWARD_DELAY);
  }
  findOpponent();
}
