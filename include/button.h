#pragma once

#include <DButton.h>

Button btnUp(PIN_BUTTON_UP, false);
Button btnDown(PIN_BUTTON_DOWN, false);
ButtonLed ledUp(PIN_LED_UP);
ButtonLed ledDown(PIN_LED_DOWN);
ButtonLed ledBody(PIN_LED_BODY);

bool isUp = false;
bool isDown = false;
bool isWork = false;
bool isLowVoltage = false;
bool isCheckError = false;

void getButtonState() {
  if (btnUp.click()) {
    if (isUp) {
      isUp = false;
      isDown = false;
      ledUp.turnOff(); 
      ledDown.turnOff(); 
      ledBody.turnOff();
    } else if(isDown){
      isDown = false;
      isUp = false;   
      ledUp.turnOff(); 
      ledDown.turnOff(); 
      ledBody.turnOff();
    } else if(!isUp){
      isUp = true;
      isDown = false;
      ledUp.turnOn();
      ledBody.blink();
    }
  } else if (btnDown.click()) {
    if (isDown) {
      isDown = false;
      isUp = false;
      ledUp.turnOff(); 
      ledDown.turnOff(); 
      ledBody.turnOff();
    } else if(isUp){
      isDown = false;
      isUp = false;   
      ledUp.turnOff(); 
      ledDown.turnOff(); 
      ledBody.turnOff();
    } else if(!isDown){
      isDown = true;
      isUp = false;
      ledDown.turnOn(); 
      ledBody.blink();
    }
  }
}

// Обновляем состояние светодиодов кнопок
void updateLed(){
  ledUp.update();
  ledDown.update();
  ledBody.update();
}