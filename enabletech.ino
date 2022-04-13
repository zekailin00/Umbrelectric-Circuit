#include <EEPROM.h>
#include <Servo.h>
#include <Stepper.h>


const int STEP_PER_REVOLUTION = 200;
const int REVOLUTIONS_TO_OPEN = 1;
const int STEPS_TO_OPEN = REVOLUTIONS_TO_OPEN * STEP_PER_REVOLUTION;
const int STEPPER_SPEED = 60;
const int SERVO_PORT = 6;
const int SERVO_SWITCH_PORT = 2;
const int STEPPER_SWITCH_PORT = 3;


Servo servo;
Stepper stepper(STEP_PER_REVOLUTION, 8, 9, 10, 11);


bool isSwitched(int pin) {
  return digitalRead(pin) == HIGH;
}


bool hasRun() {
  return EEPROM.read(0) == 1;
}


void setupMemory() {
  EEPROM.write(0, 1); // "program setup has run" flag
  EEPROM.write(1, 0); // clamp
  setSteps(0);
}


bool isClampOpen() {
  return EEPROM.read(1) == 1;
}


void closeClamp() {
  servo.write(90);
  EEPROM.write(1, 0);
}


void openClamp() {
  servo.write(0);
  EEPROM.write(1, 1);
}


int getSteps() {
  return EEPROM.read(2);
}


void setSteps(int steps) {
  EEPROM.write(2, steps);
}


void setup() {
  pinMode(SERVO_SWITCH_PORT, INPUT);
  pinMode(STEPPER_SWITCH_PORT, INPUT);

  servo.attach(SERVO_PORT);
  stepper.setSpeed(STEPPER_SPEED);

  if (!hasRun()) {
    setupMemory();
  }

  Serial.begin(9600);
}


void loop() {  
  if (isSwitched(SERVO_SWITCH_PORT) && !isClampOpen()) {
    openClamp();
    Serial.println("open");
  }

  if (!isSwitched(SERVO_SWITCH_PORT) && isClampOpen()) {
    closeClamp();
    Serial.println("close");
  }
  
  if (isSwitched(STEPPER_SWITCH_PORT)) {
//    for (int steps = getSteps(); steps < STEPS_TO_OPEN; steps++) {
     stepper.step(+1);
//      setSteps(steps + 1);
//      Serial.println(steps);
//    }
  }
  
  if (!isSwitched(STEPPER_SWITCH_PORT)) {
//    for (int steps = getSteps(); steps > 0; steps--) {
     stepper.step(-1);
//      setSteps(steps - 1);
//      Serial.println(steps);
//    }
  }
}
