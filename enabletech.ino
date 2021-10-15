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
const int stepperSpeed = 60;
const int switch1Port = 7; // power
const int switch2Port = A0; // servo
const int switch3Port = A1; // stepper

Servo myservo;  // create servo object to control a servo
Stepper myStepper(stepsPerRevolution, 8, 9, 10, 11);

int pos = 0;    // variable to store the servo position

void setup() {
  myStepper.setSpeed(stepperSpeed);
  myservo.attach(6);  // attaches the servo on pin 9 to the servo object
  Serial.begin(9600);
  if(EEPROM.read(0) != 1) {
    EEPROM.write(0, 1); // "program setup has run" flag
    EEPROM.write(1, 0); // clamp
    EEPROM.write(2, 0); // steps
    EEPROM.write(3, 0); // revolutions
  }
}

bool is_switched(int pin) {
  return analogRead(pin) > 1000;
}

void loop() {
  // this program controls 3 switches:
  // - switch #1 opens and closes the clamp
  // - switch #2 extends and closes the umbrella
  // - switch #3 is the emergency stop

  // TODO: figure out a way to emergency stop that powers off the arduino (remember that the functions being used are blocking)
  Serial.println(analogRead(switch2Port));
  if(is_switched(switch2Port) && EEPROM.read(1) == 0) { // if switch 2 is powered, and the clamp is closed
    myservo.write(0); // TODO: figure out if this is the correct angle
    EEPROM.write(1, 1); // set the clamp state to open
  }
  if(!is_switched(switch2Port) && EEPROM.read(1) == 1) { // if switch 2 is not powered and the clamp is open
    myservo.write(90); // TODO: figure out if this is the correct angle
    EEPROM.write(1, 0); // set the clamp state to closed
  }
  
  if(is_switched(switch3Port) && EEPROM.read(3) <= revolutionsToOpen) { // if switch 3 is powered and we still have revolutions to go
    int stepsTaken = EEPROM.read(2) + EEPROM.read(3) * stepsPerRevolution; // number of steps we have taken to open the umbrella fully
    for(int i = stepsTaken; i < stepsPerRevolution * revolutionsToOpen; i++) { // open the umbrella, using more steps for greater control
      myStepper.step(+1);
      int steps = EEPROM.read(2); // get the number of steps
      if(steps == stepsPerRevolution - 1) { // if the number of steps is one away from a revolution
        EEPROM.write(2, 0); // set the number of steps to 0
        int revolutions = EEPROM.read(3); // get the number of revolutions
        if(revolutions == revolutionsToOpen - 1) { // if the number of revolutions is one away from the umbrella being fully open
          EEPROM.write(3, revolutionsToOpen); // set the number of revolutions to the max
          break; // exit out of the loop, to prevent the stepper motor from moving further
        }
        EEPROM.write(3, revolutions + 1); // otherwise, simply increment the number of revolutions by 1
      }
      else {
        EEPROM.write(2, steps + 1); // otherwise, simply step by 1
      }
    }
  }
  if(!is_switched(switch3Port) && (EEPROM.read(2) > 0 || EEPROM.read(3) > 0)) { // if switch 3 is not powered and we are not in the closed state
    int stepsTaken = EEPROM.read(2) + EEPROM.read(3) * stepsPerRevolution; // number of steps we have taken to open the umbrella fully
    for(int i=stepsTaken - 1; i >= 0; i--) { // close the umbrella, using more steps for greater control.
      myStepper.step(-1);
      int steps = EEPROM.read(2); // get the number of steps
      if(steps == 0) { // if we are at 0 steps
        int revolutions = EEPROM.read(3); // get the number of revolutions
        if(revolutions == 0) { // if the number of revolutions is 0
          break; // then we are done, and we exit the loop to avoid moving the motor further
        }
        EEPROM.write(2, stepsPerRevolution - 1); // otherwise, set the number of steps to stepsPerRevolution - 1
        EEPROM.write(3, revolutions - 1); // and decrement the number of revolutions by 1
      }
      else {
        EEPROM.write(2, steps - 1); // otherwise, simply decrement the number of steps by 1
      }
    }
  }
}
