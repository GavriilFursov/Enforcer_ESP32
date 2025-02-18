#pragma once

#define ADC_RESOLUTION              12
const int ADC_MAX_VALUE = pow(2, ADC_RESOLUTION) - 1;    // Максимальное значение АЦП

// #define PIN_SENSOR_SMOKE_MQ2        34          // Датчик задымленности + нагреватель
#define PIN_SENSOR_TEMP             33          // Датчик температуры
#define PIN_SENSOR_LIGHT            35          // Датчик освещенности
#define PIN_SENSOR_CHECK_ERROR      21          // Пин для определения ошибки на драйвере
#define PIN_SENSOR_VOLTAGE          34          // Делитель напружения, чтобы контролировать уровень напряжения! 
#define PIN_SENSOR_ANGLE            32          // Потенциометр

// Привод (конструктор встроенный)
#define PIN_MOTOR_STEP              18          // Фиолетовый
#define PIN_MOTOR_DIR               19

// Кнопки (конструктор встроенный)
#define PIN_BUTTON_UP               23
#define PIN_BUTTON_DOWN             5


// // Диоды 
#define PIN_LED_UP                  16
#define PIN_LED_DOWN                17
#define PIN_LED_BODY                3

#define PIN_POWER_RELAY             1           // Реле питания

void setupPins(){
    // pinMode(PIN_SENSOR_ANGLE, INPUT);
    // pinMode(PIN_SENSOR_LIGHT, INPUT);
    // pinMode(PIN_SENSOR_CHECK_ERROR, INPUT);
    // pinMode(PIN_SENSOR_VOLTAGE, INPUT);
    // pinMode(PIN_LED_BODY, OUTPUT);
    // pinMode(PIN_POWER_RELAY, OUTPUT);

    analogReadResolution(ADC_RESOLUTION);
    // analogWriteResolution(10);
}