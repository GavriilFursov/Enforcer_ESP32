#pragma once

#include <WiFi.h>
#include <WebServer.h>
#include "SPIFFS.h"
#include <cstdlib>
#include <ArduinoJson.h>
#include <ESPmDNS.h>

// ----- Настройки сети -----
const char *ssid = "MyESP32AP";
const char *password = "password123";
const char *host_name = "piranha_esp32";

// ----- Глобальные переменные для хранения данных -----
float travel_distance;
float speed;
int battery_level;
float room_temperature;
float light_level;
float cable_extension;

// ----- Пароль для доступа к настройкам -----
const char *settings_password = "admin";
bool settings_authorized = false;
unsigned long startTime; // Время запуска контроллера

// ----- Имя файла для сохранения настроек -----
const char *settings_file = "/settings.json";

// ----- Экземпляр веб-сервера -----
WebServer server(80);

// ----- Функция для загрузки настроек из SPIFFS -----
void loadSettings()
{
    if (SPIFFS.exists(settings_file))
    {
        File file = SPIFFS.open(settings_file, "r");
        if (file)
        {
            size_t size = file.size();
            if (size > 1024)
            {
                Serial.println("Settings file too large!");
                file.close();
                return;
            }
            std::unique_ptr<char[]> buf(new char[size]);
            file.readBytes(buf.get(), size);
            file.close();
            DynamicJsonDocument doc(1024);
            DeserializationError error = deserializeJson(doc, buf.get());
            if (error)
            {
                Serial.println("Failed to parse settings file");
                return;
            }
            if (doc.containsKey("travel_distance"))
            {
                travel_distance = doc["travel_distance"].as<float>();
            }
            if (doc.containsKey("speed"))
            {
                speed = doc["speed"].as<float>();
            }
            if (doc.containsKey("runtime"))
            {
                startTime = millis() - (doc["runtime"].as<unsigned long>() * 1000);
            }
        }
        else
        {
            Serial.println("Failed to open settings file for reading");
        }
    }
}

// ----- Функция для сохранения настроек в SPIFFS -----
void saveSettings()
{
    DynamicJsonDocument doc(1024);
    doc["travel_distance"] = travel_distance;
    doc["speed"] = speed;
    doc["runtime"] = (millis() - startTime) / 1000; // Сохраняем время работы в секундах
    File file = SPIFFS.open(settings_file, "w");
    if (!file)
    {
        Serial.println("Failed to open settings file for writing");
        return;
    }
    if (serializeJson(doc, file) == 0)
    {
        Serial.println("Failed to save settings to file");
    }
    file.close();
}

// ----- Функция для создания HTML-страницы -----
String getWebPage(String currentUri, String message = "", bool success = false)
{
    String htmlPage = R"=====(
    <!DOCTYPE html>
    <html>
    <head>
        <title>ENFORCER System</title>
        <meta charset="UTF-8">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <style>
            body { font-family: 'Arial', sans-serif; margin: 0; padding: 0; background-color: #1a1a1a; color: #ffffff; }
            .container { width: 90%; max-width: 1200px; margin: 20px auto; padding: 20px; }
            h1 { color: #ffa500; text-align: center; margin-bottom: 20px; }
            nav ul { list-style: none; padding: 0; margin: 0; display: flex; justify-content: center; align-items: center; background-color: #333; }
            nav li { margin: 0; }
           nav a { display: inline-block; padding: 10px 20px; text-decoration: none; color: #ffa500; }
           nav a:hover { background-color: #555; color: #ffcc66; }
            nav a.active { font-weight: bold; background-color: #555; color: #ffcc66; }
            .content { margin-top: 20px; padding: 20px; background-color: #333; border-radius: 5px; }
            .image-container { text-align: center; margin-bottom: 20px; }
            .image-container img { max-width: 100%; height: auto; margin-bottom: 10px; }
            .data-line { display: flex; align-items: center; margin-bottom: 10px; }
            .data-item { margin-right: 10px; }
            .battery-image { width: 30px; margin-right: 5px; vertical-align: middle; }
            .settings-form { margin-top: 20px; }
            input[type="number"] { margin-bottom: 5px; background-color: #555; border: 1px solid #777; color: #fff; padding: 8px; border-radius: 3px; }
           input[type="submit"] { margin-top: 10px; background-color: #ffa500; border: none; padding: 10px 15px; color: #000; cursor: pointer; border-radius: 3px; }
            input[type="submit"]:hover { background-color: #ffcc66; }
            img { vertical-align: middle; }
           .message { margin-top: 10px; ";
    if (success) {
        htmlPage += "color: green;";
   } else {
       htmlPage += "color: red;";
    }
    htmlPage += "}";
    htmlPage += R"=====(
        </style>
        <script>
            function fetchSensorData() {
             fetch('/sensor_data')
             .then(response => response.json())
              .then(data => {
                document.getElementById('battery_data').innerHTML = data.battery;
                document.getElementById('temperature_data').innerHTML = data.temperature;
                 document.getElementById('light_data').innerHTML = data.light;
                document.getElementById('cable_data').innerHTML = data.cable + " %";
                 document.getElementById('runtime_data').innerHTML = data.runtime;
                var battery_element = document.getElementById('battery_data');
                  if(data.battery_level >= 75){
                    battery_element.innerHTML = "<img src='/battery_full.png' class='battery-image' alt='Battery Full'>" + data.battery + "%";
                    }else if(data.battery_level >= 50){
                    battery_element.innerHTML = "<img src='/battery_medium.png'  class='battery-image' alt='Battery Medium'>" + data.battery + "%";
                    } else {
                    battery_element.innerHTML = "<img src='/battery_low.png'  class='battery-image' alt='Battery Low'>" + data.battery + "%";
                   }
              });
            }
           setInterval(fetchSensorData, 1000);
        </script>
    </head>
    <body>
        <div class="container">
            <h1>ENFORCER System</h1>
            <nav>
                <ul>
                  <li><a href="/" class='";
    if (currentUri == "/") {
      htmlPage += "active";
    }
    htmlPage += "'>Главная</a></li><li><a href='/system_data' class='";
    if (currentUri == "/system_data") {
       htmlPage += "active";
    }
    htmlPage += "'>Показания системы</a></li><li><a href='/system_settings' class='";
    if (currentUri == "/system_settings") {
        htmlPage += "active";
   }
    htmlPage += "'>Настройки системы</a></li>
                </ul>
            </nav>

)=====";

    if (currentUri == "/")
    {
        htmlPage += R"=====(<div class='content'><div class='image-container'><img src='/image1.png' alt='Exoskeleton Promo'></div><div class='image-container'><img src='/image2.png' alt='Second Exoskeleton Image'></div></div>)=====";
    }
    else if (currentUri == "/system_data")
    {
        htmlPage += R"=====(<div class='content'><h2>Показания системы</h2><div class='data-line'><span class='data-item'>Аккумулятор:</span><span class='data-item' id='battery_data'></span></div><div class='data-line'></span></div><div class='data-line'><span class='data-item'>Температура:</span><span class='data-item' id='temperature_data'></span></div><div class='data-line'><span class='data-item'>Освещенность:</span><span class='data-item' id='light_data'></span></div><div class='data-line'><span class='data-item'>Вытяжка троса:</span><span class='data-item' id='cable_data'></span></div></div>)=====";
    }
    else if (currentUri == "/system_settings")
    {
        if (!settings_authorized)
        {
            htmlPage += R"=====(<div class='content'><h2>Требуется авторизация</h2><form action='/login' method='post' class='settings-form'><label for='password'>Пароль:</label><br><input type='password' id='password' name='password'><br><input type='submit' value='Войти'></form>)=====";
            if (!message.isEmpty())
            {
                htmlPage += R"=====(<p class='message'>)=====";
                htmlPage += message;
                htmlPage += R"=====(</p>)=====";
            }
            htmlPage += R"=====(</div>)=====";
        }
        else
        {
            htmlPage += R"=====(<div class='content'><h2>Настройки системы</h2>)=====";
            if (!message.isEmpty())
            {
                htmlPage += R"=====(<p class='message'>)=====";
                htmlPage += message;
                htmlPage += R"=====(</p>)=====";
            }
            htmlPage += R"=====(<form action='/save_settings' method='post' class='settings-form'><label for='travel_distance'>Рабочий ход троса (см):</label><br><input type='number' id='travel_distance' name='travel_distance'><br><label for='speed'>Скорость подъема (см/с):</label><br><input type='number' id='speed' name='speed'><br><input type='submit' value='Сохранить'></form><div id='settings_output'>)=====";
            if (travel_distance > 0)
            {
                htmlPage += R"=====(<p>Рабочий ход троса: )=====";
                htmlPage += String(travel_distance);
                htmlPage += R"=====( см)=====";
                if (travel_distance < 100 || travel_distance > 180)
                {
                    htmlPage += R"=====( - Введите значение от 100 до 180 см)=====";
                }
                htmlPage += R"=====(</p>)=====";
            }
            if (speed > 0)
            {
                htmlPage += R"=====(<p>Скорость подъема: )=====";
                htmlPage += String(speed);
                htmlPage += R"=====( см/с)=====";
                if (speed < 8 || speed > 24)
                {
                    htmlPage += R"=====( - Скорость должна быть между 8 и 24 см/с)=====";
                }
                htmlPage += R"=====(</p>)=====";
            }

            // Добавляем отображение времени работы
            unsigned long runtimeSeconds = (millis() - startTime) / 1000;
            unsigned int hours = runtimeSeconds / 3600;
            unsigned int minutes = (runtimeSeconds % 3600) / 60;
            unsigned int seconds = runtimeSeconds % 60;
            char timeBuffer[20];
            sprintf(timeBuffer, "%02d:%02d:%02d", hours, minutes, seconds);
            htmlPage += R"=====(<p>Время работы: )=====";
            htmlPage += timeBuffer;
            htmlPage += R"=====(</p>)=====";

            htmlPage += R"=====(</div></div>)=====";
        }
    }
    htmlPage += R"=====(
        </div>
    </body>
    </html>
  )=====";
    return htmlPage;
}

// ----- Обработчик корневого запроса (/) -----
void handleRoot()
{
    server.send(200, "text/html", getWebPage(server.uri()));
}

// ----- Обработчик запросов изображений -----
void handleImage()
{
    String path = server.uri();
    File file = SPIFFS.open(path, "r");
    if (!file)
    {
        server.send(404, "text/plain", "File not found");
        return;
    }
    server.streamFile(file, "image/png");
    file.close();
}

// ----- Обработчик для запроса сенсорных данных (JSON) -----
void handleSensorData()
{
    DynamicJsonDocument doc(1024);
    doc["battery"] = battery_level;
    doc["battery_level"] = battery_level;
    doc["temperature"] = String(room_temperature, 1) + " °C";
    doc["light"] = String(light_level, 0) + " lux";
    doc["cable"] = String(cable_extension, 1);
    unsigned long runtimeSeconds = (millis() - startTime) / 1000;
    unsigned int hours = runtimeSeconds / 3600;
    unsigned int minutes = (runtimeSeconds % 3600) / 60;
    unsigned int seconds = runtimeSeconds % 60;
    char timeBuffer[20];
    sprintf(timeBuffer, "%02d:%02d:%02d", hours, minutes, seconds);
    doc["runtime"] = timeBuffer;
    String jsonString;
    serializeJson(doc, jsonString);
    server.send(200, "application/json", jsonString);
}

// ----- Обработчик для авторизации -----
void handleLogin()
{
    if (server.hasArg("password"))
    {
        String password = server.arg("password");
        if (password == settings_password)
        {
            settings_authorized = true;
            server.sendHeader("Location", "/system_settings");
            server.send(303);
            return;
        }
        else
        {
            server.send(200, "text/html", getWebPage("/system_settings", "Неверный пароль."));
            return;
        }
    }
    server.send(200, "text/html", getWebPage("/system_settings", "Неверный запрос."));
}

// ----- Обработчик для сохранения настроек -----
void handleSaveSettings()
{
    if (server.hasArg("travel_distance") && server.hasArg("speed"))
    {
        String travel_distance_str = server.arg("travel_distance");
        String speed_str = server.arg("speed");
        float new_travel_distance, new_speed;

        if (travel_distance_str.isEmpty() || speed_str.isEmpty())
        {
            server.send(200, "text/html", getWebPage("/system_settings", "Ошибка: Поля не могут быть пустыми."));
            return;
        }

        char *endptr;
        new_travel_distance = strtof(travel_distance_str.c_str(), &endptr);
        if (*endptr != '\0' || isnan(new_travel_distance))
        {
            server.send(200, "text/html", getWebPage("/system_settings", "Ошибка: Некорректный формат длины троса."));
            return;
        }

        new_speed = strtof(speed_str.c_str(), &endptr);
        if (*endptr != '\0' || isnan(new_speed))
        {
            server.send(200, "text/html", getWebPage("/system_settings", "Ошибка: Некорректный формат скорости."));
            return;
        }

        if (new_travel_distance < 100 || new_travel_distance > 180 || new_speed < 8 || new_speed > 24)
        {

            if (new_travel_distance < 100 || new_travel_distance > 180)
            {
                server.send(200, "text/html", getWebPage("/system_settings", "Ошибка: Рабочий ход троса должен быть от 100 до 180 см."));
                return;
            }

            if (new_speed < 8 || new_speed > 24)
            {
                server.send(200, "text/html", getWebPage("/system_settings", "Ошибка: Скорость должна быть между 8 и 24 см/с."));
                return;
            }
        }

        travel_distance = new_travel_distance;
        speed = new_speed;
        Serial.print("Travel distance: ");
        Serial.println(travel_distance);
        Serial.print("Speed: ");
        Serial.println(speed);
        saveSettings();
        server.send(200, "text/html", getWebPage("/system_settings", "Изменения сохранены.", true));
        server.sendHeader("Refresh", "1; url=/");
    }
    else
    {
        server.send(200, "text/html", getWebPage("/system_settings", "Ошибка: Неверные параметры."));
    }
}

// ----- Функция для симуляции показаний датчиков -----
void updateSensorData()
{
    static ERA_filter<float> angle_filer(0.1);
    static ERA_filter<float> battery_filer(0.1);
    battery_level = mapf((battery_filer.filtered(analogRead(PIN_SENSOR_VOLTAGE) * 3.3) / ADC_MAX_VALUE), 2.64, 3.03, 0.0, 100.0);
    room_temperature = getOverboardTemp();
    light_level = getLightLevel();
    cable_extension = map(angle_filer.filtered(getRopeLength()), 15.0 + 17.5, travel_distance - 15, 0.0, 100.0);
}

// ----- Функция setup (инициализация) -----
void setupServer()
{
    Serial.println("Configuring WiFi AP ...");
    WiFi.mode(WIFI_AP);
    WiFi.setHostname(host_name);
    WiFi.softAP(ssid, password);
    IPAddress ip = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(ip);

    if (!SPIFFS.begin(true))
    {
        Serial.println("An Error has occurred while mounting SPIFFS");
        return;
    }
    if (MDNS.begin("enforcer"))
    {
        Serial.println("mDNS responder started");
    }
    else
    {
        Serial.println("Error starting mDNS");
    }
    startTime = millis();
    loadSettings();

    server.on("/", handleRoot);
    server.on("/system_data", handleRoot);
    server.on("/system_settings", handleRoot);
    server.on("/save_settings", HTTP_POST, handleSaveSettings);
    server.on("/login", HTTP_POST, handleLogin);
    server.on("/sensor_data", handleSensorData);
    server.on("/image1.png", handleImage);
    server.on("/image2.png", handleImage);
    server.on("/battery_full.png", handleImage);
    server.on("/battery_medium.png", handleImage);
    server.on("/battery_low.png", handleImage);

    server.begin();
    Serial.println("HTTP server started");
}