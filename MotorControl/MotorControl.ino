#include <CheapStepper.h>

// Declare the stepper on pins 8, 9, 10, 11
CheapStepper stepper(8, 9, 10, 11);

const int buttonPin = 2;
bool moveClockwise = true;
bool modelOpen = false;

void handleSerialCommands() {
  if (Serial.available() <= 0) {
    return;
  }

  String command = Serial.readStringUntil('\n');
  command.trim();
  command.toUpperCase();

  // OPEN maps to pull-up LOW behavior, CLOSE maps to HIGH behavior.
  if (command == "OPEN") {
    modelOpen = true;
    Serial.println("Command OPEN received (virtual LOW)");
  } else if (command == "CLOSE") {
    modelOpen = false;
    Serial.println("Command CLOSE received (virtual HIGH)");
  }
}

void setup() {
  Serial.begin(9600);
  stepper.setRpm(12);

  // Initialize button with internal pull-up
  pinMode(buttonPin, INPUT_PULLUP);

  Serial.println("Non-blocking mode ready.");
  Serial.println("Control with OPEN/CLOSE over serial or by pressing the button.");
}

void loop() {
  handleSerialCommands();

  // 1. Combine physical button and serial command state.
  bool buttonPressed = (digitalRead(buttonPin) == LOW);
  bool isPressed = buttonPressed || modelOpen;

  if (isPressed) {
    // If the button is pressed and the motor isn't already moving,
    // start a very large move (e.g., 100 rotations)
    if (stepper.getStepsLeft() == 0) {
      stepper.newMove(moveClockwise, 40960); 
      Serial.println("Start signal: Motor running...");
    }
  } 
  else {
    // If the button is released and the motor is still trying to move,
    // stop it immediately.
    if (stepper.getStepsLeft() > 0) {
      stepper.stop();
      Serial.println("Stop signal: Motor stopped.");
    }
  }

  // 2. THE ENGINE: This must be called every loop iteration.
  // It checks if it's time to take a step based on the RPM and moves if needed.
  stepper.run();

  // 3. MULTI-TASKING: Because this is non-blocking, you can do other things here!
  // For example, blinking an LED or reading another sensor without stopping the motor.
}
