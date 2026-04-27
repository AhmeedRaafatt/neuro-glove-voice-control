#include <CheapStepper.h>

// Declare the stepper on pins 8, 9, 10, 11
CheapStepper stepper(8, 9, 10, 11);

const int buttonPin = 2;
bool moveClockwise = true;

void setup() {
  Serial.begin(9600);
  stepper.setRpm(12);

  // Initialize button with internal pull-up
  pinMode(buttonPin, INPUT_PULLUP);

  Serial.println("Non-blocking mode ready.");
  Serial.println("Motor will move only while button is held.");
}

void loop() {
  // 1. Check the button state
  bool isPressed = (digitalRead(buttonPin) == LOW);

  if (isPressed) {
    // If the button is pressed and the motor isn't already moving,
    // start a very large move (e.g., 100 rotations)
    if (stepper.getStepsLeft() == 0) {
      stepper.newMove(moveClockwise, 40960); 
      Serial.println("Button Pressed: Starting movement...");
    }
  } 
  else {
    // If the button is released and the motor is still trying to move,
    // stop it immediately.
    if (stepper.getStepsLeft() > 0) {
      stepper.stop();
      Serial.println("Button Released: Stopping.");
    }
  }

  // 2. THE ENGINE: This must be called every loop iteration.
  // It checks if it's time to take a step based on the RPM and moves if needed.
  stepper.run();

  // 3. MULTI-TASKING: Because this is non-blocking, you can do other things here!
  // For example, blinking an LED or reading another sensor without stopping the motor.
}
