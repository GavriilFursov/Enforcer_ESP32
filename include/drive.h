#pragma once

#include <AccelStepper.h>

const int STEP = 1000;
const int REDUCTION_RATIO = 20; 
const int STEPS_PER_CM = (STEP * REDUCTION_RATIO) / CIRCUM_LENGTH; // Количество импульсов на см
const float MIN_LENGTH = 15.0;

AccelStepper stepper(AccelStepper::DRIVER, PIN_MOTOR_STEP, PIN_MOTOR_DIR);

// Функция для расчета количества импульсов для мотора
int getMotorPulseCount() {
  int _currentLength = getRopeLength();
  int _targetLength;

  if (isUp) {
    if(_currentLength < MIN_LENGTH + 17.5) return 0;
    else _targetLength = MIN_LENGTH + 10;
  } else if (isDown) {
    if(_currentLength > travel_distance - 15) return 0;
    else _targetLength = travel_distance - 10;
  } else {
    return 0;
  }

  int lengthDifference = (_targetLength - _currentLength);
  return (lengthDifference * STEPS_PER_CM) * -1;
}

int getSpeedMotor(){
  float _speed = (speed / (DRUM_DIAMETR / 2.0f));
  float speed_rpm = (_speed / (2*PI)) * 60.0f;
  return static_cast<int>(speed_rpm * REDUCTION_RATIO);
}

void clearError(){
  digitalWrite(PIN_POWER_RELAY, LOW);
  delay(1000);
  digitalWrite(PIN_POWER_RELAY, HIGH);
  ledUp.turnOff(); 
  ledDown.turnOff();
  ledBody.turnOff();
  delay(1000);
  isCheckError = false;
  isUp = false;
  isDown = false;
}

void mainControl() {
  static bool lastStateUp = false;
  static bool lastStateDown = false;

  if(!isLowVoltage)
    if(!isCheckError){
      getButtonState();
      bool modeChanged = (isUp != lastStateUp) || (isDown != lastStateDown);
      lastStateUp = isUp;
      lastStateDown = isDown;
      if (isUp || isDown) {
        if (modeChanged) {
          stepper.setMaxSpeed(getSpeedMotor() * STEP / 60);
          stepper.moveTo(getMotorPulseCount());
          Serial.print(travel_distance);
          Serial.print(" , ");
          Serial.println(getRopeLength());
          isWork = true;
        }
        stepper.run();
        if (stepper.distanceToGo() == 0) {
          isUp = false; 
          isDown = false;
          isWork = false;
          ledUp.turnOff();
          ledDown.turnOff();
          ledBody.turnOff();
        }
      } else {
        if (modeChanged) {
          stepper.setCurrentPosition(0);
          stepper.setMaxSpeed(0);
          isWork = false;
        }
      }
    }
    else{
      clearError();
    }
  else{
    ledUp.blink();
    ledDown.blink();
    ledBody.turnOff();
    digitalWrite(PIN_POWER_RELAY, LOW);
  }
}

// bool flagButton = false;

// void motorControl(){
//   if(!isCheckError){
//     if(btnUp.hold()){
//       if(flagButton == false){
//         stepper.setMaxSpeed((1800 * 1000) / 60);
//         stepper.moveTo(10000 * 1000);
//         ledUp.turnOn();
//         flagButton = true;
//       }
//       stepper.run();
//     }
//     else if(btnDown.hold()){
//       if(flagButton == false){
//         stepper.setMaxSpeed((1800 * 1000) / 60);
//         stepper.moveTo(-10000 * 1000);
//         flagButton = true;
//         ledDown.turnOn();
//       }
//       stepper.run();
//     }
//     else{
//       if(flagButton == true){
//         stepper.setCurrentPosition(stepper.currentPosition());
//         stepper.move(0);
//         stepper.setMaxSpeed(0);
//         flagButton = false;
//         ledUp.turnOff();
//         ledDown.turnOff();
//       }
//     }
//   }
//   else{
//     clearError();
//   }
// }