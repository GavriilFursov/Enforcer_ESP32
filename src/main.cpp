#include <Arduino.h>

#include <pinmap.h>
#include <sensors.h>
#include <server.h>

void sendingData(){
  server.handleClient();
  updateSensorData();
}

void setup(){
  Serial.begin(9600);
  setupServer();
}

void loop(){
}
