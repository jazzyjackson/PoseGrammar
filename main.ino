#include "ModeSwitch.h"  // handles 3-position throw switch
#include "Broadcast.h"   // writes PWM output for pose-followers
#include "DutyStruct.h"  // keeps up with the output from another bot's Broadcast
#include "KnobState.h"   // keeps the state of the knobs, X and Y
#include "LightStage.h"  // provides simple interface to NeoPixels, avoids unnecessary writes
#include "MotorState.h"  // writes positions to servos X and Y
#include "Posegram.h"    // interfaces with EEPROM to remember poses programmed, X and Y, beats 1-8.

#define KNOB_X            A0 // ADC0
#define KNOB_Y            A1 // ADC1
#define PWM_INPUT_X       2  // INT0
#define PWM_INPUT_Y       3  // INT1
#define MODESWITCH_PIN_A  4  // digitalRead
#define MOTOR_X           5  // Clock 0 PWM
#define NEOPIXEL_PIN      6  // Clock 0 PWM - shared with MOTOR X, don't push too many simultaneous events
#define MODESWITCH_PIN_B  8  // digitalRead
#define BROADCAST_X       9  // Clock 1 PWM
#define BROADCAST_Y       10 // Clock 1 PWM
#define MOTOR_Y           11 // Clock 2 PWM
#define NEXTBEAT_PIN      12 // digitalRead

KnobState knobState(
  KNOB_X,
  KNOB_Y
);
DutyStruct X_INPUT = {PWN_INPUT_X, 0, 0, 0, 0};
DutyStruct Y_INPUT = {PWM_INPUT_Y, 0, 0, 0, 0};
ModeSwitch modeSwitch(
  MODESWITCH_PIN_A,
  MODESWITCH_PIN_B
);
MotorState motorState(
  MOTOR_X,
  MOTOR_Y,
);
LightStage lightStage(
  NEOPIXEL_PIN
);
Broadcast broadcast(
  BROADCAST_X,
  BROADCAST_Y
);

Posegram posegram(
  modeSwitch,
  broadcast,
  dutystruct,
  knobState,
  lightStage,
  motorState,
  NEXTBEAT_PIN
);

void nextBeatHelper(){ posegram.nextBeat(); }
void updateXHelper(){ X_INPUT.updateDutyCycle(); }
void updateYHelper(){ Y_INPUT.updateDutyCycle(); }

void setup() {
  attachInterrupt(digitalPinToInterrupt(NEXTBEAT_PIN), nextBeatHelper, RISING);
  attachInterrupt(digitalPinToInterrupt(MODESWITCH_PIN_A), updateXHelper, CHANGE);
  attachInterrupt(digitalPinToInterrupt(MODESWITCH_PIN_B), updateYHelper, CHANGE);
}

void loop() {
  switch(modeSwitch.getModeState()){
    case 0: posegram.programPosition(); break;
    case 1: posegram.playEachPose();    break;
    case 2: posegram.followTheLeader(); break;
  }
}