#include <CheapStepper.h>

// Declare the stepper on pins 8, 9, 10, 11
CheapStepper stepper(8, 9, 10, 11);

const int buttonPin = 2;

// Definitions for absolute positioning
const float ROTATIONS = 0.25; 
const float MAX_ROTATION_STEPS = ROTATIONS * 4096L; 

long currentAbsolutePosition = 0;
long targetPosition = 0;
long moveTargetPosition = 0; // Where the stepper is currently instructed to go
bool isMoving = false;

void stopMotor() {
  stepper.stop();
  // We use long for the calculation to be safe
  long stepsLeft = stepper.getStepsLeft(); 
  
  if (moveTargetPosition > currentAbsolutePosition) {
    // We were moving CW
    currentAbsolutePosition = moveTargetPosition - stepsLeft;
  } else if (moveTargetPosition < currentAbsolutePosition) {
    // We were moving CCW
    currentAbsolutePosition = moveTargetPosition + stepsLeft;
  }
  
  moveTargetPosition = currentAbsolutePosition;
  targetPosition = currentAbsolutePosition;
  isMoving = false;
  Serial.print("Motor stopped. Current Position: ");
  Serial.println(currentAbsolutePosition);
}

void startMotorToTarget(long target) {
  targetPosition = target;
  if (targetPosition == currentAbsolutePosition) return;
  
  bool cw = targetPosition > currentAbsolutePosition;
  unsigned long distance = abs(targetPosition - currentAbsolutePosition);
  
  stepper.newMove(cw, distance);
  moveTargetPosition = targetPosition;
  isMoving = true;
  
  Serial.print("Motor starting. Target Position: ");
  Serial.print(targetPosition);
  Serial.print(", Current Position: ");
  Serial.println(currentAbsolutePosition);
}

void handleSerialCommands() {
  if (Serial.available() <= 0) {
    return;
  }

  String command = Serial.readStringUntil('\n');
  command.trim();
  command.toUpperCase();

  if (command == "OPEN") {
    Serial.println("Command OPEN received");
    if (isMoving) stopMotor();
    startMotorToTarget(MAX_ROTATION_STEPS); // Rotate into open direction
  } else if (command == "CLOSE") {
    Serial.println("Command CLOSE received");
    if (isMoving) stopMotor();
    startMotorToTarget(-MAX_ROTATION_STEPS); // Rotate into close direction
  } else if (command == "STOP") {
    Serial.println("Command STOP received");
    if (isMoving) stopMotor();
  }
}

void setup() {
  Serial.begin(9600);
  stepper.setRpm(12);

  // Initialize button with internal pull-up
  pinMode(buttonPin, INPUT_PULLUP);

  Serial.println("Absolute positioning mode ready.");
  Serial.println("Control with OPEN/CLOSE/STOP over serial.");
}

void loop() {
  handleSerialCommands();

  // Button functions as an emergency stop
  if (digitalRead(buttonPin) == LOW && isMoving) {
    Serial.println("Button pressed: Emergency STOP");
    stopMotor();
    // Wait until button is released so it won't constantly spam stop
    while(digitalRead(buttonPin) == LOW) {
       delay(10);
    }
  }

  stepper.run();

  // Check if target was reached successfully
  if (isMoving && stepper.getStepsLeft() == 0) {
    currentAbsolutePosition = moveTargetPosition;
    isMoving = false;
    Serial.print("Target reached. Current Position: ");
    Serial.println(currentAbsolutePosition);
  }
}
