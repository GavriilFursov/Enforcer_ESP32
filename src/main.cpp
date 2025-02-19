#include <Arduino.h>

#include <pinmap.h>
#include <button.h>
#include <sensors.h>
#include <server.h>
#include <drive.h>

void checkError(){
  static unsigned long _chech_error_timer = 0; 
  unsigned long _delta_timer = millis() - _chech_error_timer;
  if(_delta_timer >= 500){
    if(!isLowVoltage) {
      getPowerVoltage();
      getCheckError();
    }
    _chech_error_timer = millis();
  }
}

void sendingData(){
  server.handleClient();
  updateSensorData();
}

void setup(){
  Serial.begin(9600);
  setupServer();
  setupPins();
  stepper.setAcceleration(40000);
  setCpuFrequencyMhz(240); 
  digitalWrite(PIN_POWER_RELAY, HIGH);
}

void loop(){
  checkError();
  mainControl();
  updateLed();
  if (!isWork){
    sendingData();
  }
}
