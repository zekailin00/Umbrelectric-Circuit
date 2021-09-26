#include <Stepper.h>
#include <Servo.h>

// stepper motor
const int stepsPerRevolution = 200;
Stepper myStepper(stepsPerRevolution, 8, 9, 10, 11);
const int STEP_MAX = 100;
const int STEP_MIN = 0;
int stepCount = 0;

// servo
Servo servo_13;

// button
int buttonState_7 = 0;
int buttonState_2 = 0;

void setup() {
  servo_13.attach(13, 500, 2500);
  pinMode(2, INPUT);
  pinMode(7, INPUT);

  myStepper.setSpeed(200);

  Serial.begin(9600);
}

void loop() {
  buttonState_2 = digitalRead(2);
  buttonState_7 = digitalRead(7);

  if (buttonState_2 == HIGH && stepCount < STEP_MAX) {
    myStepper.step(+1);
    stepCount++;
  } else if (buttonState_2 == LOW && stepCount > STEP_MIN) {
    myStepper.step(-1);
    stepCount--;
  }

  if (buttonState_7 == HIGH) {
    servo_13.write(0);
  } else {
    servo_13.write(90);
  }

  Serial.println(stepCount);
}
