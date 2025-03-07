#pragma once

#include <OneWire.h>
#include <DFilters.h>

const float DRUM_DIAMETR = 2.5;
const int POT_TURNS = 10;
const float REDUCTION_RATIO_DRUM = 2.5;
const float CIRCUM_LENGTH = PI * DRUM_DIAMETR;
const float MIN_LENGTH = 73.0;

// Показания ДУП фильтрованное
int getAngleADC(){
  return analogRead(PIN_SENSOR_ANGLE);
}

// Обороты потенциометра
float getPotTurns(){
    return ((float)getAngleADC() / ADC_MAX_VALUE) * POT_TURNS;
}

// Длина троса в см
float getRopeLength(){
    return getPotTurns() * CIRCUM_LENGTH * REDUCTION_RATIO_DRUM;
}

// Показания датчика освещенности (фильтрованные и переведенные в LUX)
float getLightLevel(){
    static ERA_filter<float> light_sensor_filter(0.5);
    int light_sensor_reading = analogRead(PIN_SENSOR_LIGHT);
    float ratio = (ADC_MAX_VALUE / (float)light_sensor_reading) - 1;
    unsigned long photocell_resistor = 10000 * ratio;
    return light_sensor_filter.filtered(32017200 / (float)pow(photocell_resistor, 1.5));;
}

// Показания термометра
float getOverboardTemp(){
    //Объект OneWire для термометра
    static OneWire DS18B20(PIN_SENSOR_TEMP);
    static byte _data[12];
    static byte _addr[8];   

    if(!DS18B20.search(_addr)) DS18B20.reset_search();
    
    DS18B20.reset();
    DS18B20.select(_addr);
    DS18B20.write(0x44, 1);        
    DS18B20.reset();
    DS18B20.select(_addr);    
    DS18B20.write(0xBE);         
    
    for(byte i = 0; i < 9; i++) _data[i] = DS18B20.read();
    int16_t _raw = (_data[1] << 8) | _data[0];    
    return (float)_raw / 16.0;;
}

void getCheckError(){
    digitalRead(PIN_SENSOR_CHECK_ERROR);
    if(digitalRead(PIN_SENSOR_CHECK_ERROR)) isCheckError = true;
    else isCheckError = false; 
}   

void getPowerVoltage() {
    float _vin = ((float)analogRead(PIN_SENSOR_VOLTAGE) * 3.3) / ADC_MAX_VALUE;
    if (_vin < 0.69) {
        isLowVoltage = true;
    } else {
        isLowVoltage = false;
    }
}

float mapf(float x, float in_min, float in_max, float out_min, float out_max) {
    const float run = in_max - in_min;
    if(run == 0){
        log_e("map(): Invalid input range, min == max");
        return -1; // AVR returns -1, SAM returns 0
    }
    const float rise = out_max - out_min;
    const float delta = x - in_min;
    if ((delta * rise / run + out_min) <= out_max && ((delta * rise / run + out_min) >= out_min)) return (delta * rise / run + out_min);
    else if ((delta * rise / run + out_min) > out_max) return out_max;
    else if ((delta * rise / run + out_min) < out_min) return out_min;
}