// Based on:
/* Sweep
 by BARRAGAN <http://barraganstudio.com>
 This example code is in the public domain.
 modified 8 Nov 2013
 by Scott Fitzgerald
 http://www.arduino.cc/en/Tutorial/Sweep
*/

#include <EEPROM.h>
#include <Servo.h>
#include <Stepper.h>

const int stepsPerRevolution = 200;
const int revolutionsToOpen = 1;
const int stepToOpen = 200;
const int stepperSpeed = 60;
const int servoPort = 6;
const int servoSwitchPort = 2;
const int stepperSwitchPort = 3;

Servo servo;
Stepper stepper(stepsPerRevolution, 8, 9, 10, 11);

bool isSwitched(int pin) {
  return digitalRead(pin) == HIGH;
}

bool hasRun() {
  return EEPROM.read(0) == 1;
}

bool isClampOpen() {
  return EEPROM.read(1) == 1;
}

int getSteps() {
  return EEPROM.read(2);
}

void setSteps(int steps) {
  EEPROM.write(2, steps);
}

void setup() {
  servo.attach(servoPort);
  pinMode(servoSwitchPort, INPUT);
  pinMode(stepperSwitchPort, INPUT);

  stepper.setSpeed(stepperSpeed);

  if (!hasRun()) {
    EEPROM.write(0, 1); // "program setup has run" flag
    EEPROM.write(1, 0); // clamp
    setSteps(0); // steps
  }

  setSteps(0);
  Serial.begin(9600);
}

void loop() {
  // this program controls 3 switches:
  // - switch #1 opens and closes the clamp
  // - switch #2 extends and closes the umbrella
  // - switch #3 is the emergency stop

  // TODO: figure out a way to emergency stop that powers off the arduino (remember that the functions being used are blocking)
  
  if (isSwitched(servoSwitchPort) && !isClampOpen()) {
    servo.write(0);
    EEPROM.write(1, 1); // set the clamp state to open
  }

  if (!isSwitched(servoSwitchPort) && isClampOpen()) {
    servo.write(90);
    EEPROM.write(1, 0); // set the clamp state to closed
  }
  
  if (isSwitched(stepperSwitchPort)) {
    for (int steps = getSteps(); steps < stepToOpen; steps++) {
      stepper.step(+1);
      setSteps(steps + 1);
    }
  }
  
  if (!isSwitched(stepperSwitchPort)) {
    for (int steps = getSteps(); steps > 0; steps--) {
      stepper.step(-1);
      setSteps(steps - 1);
    }
  }
}
