# Correctore_Core [beta]
Устройство для отслеживания положения спины

ВАЖНО!!

Загружать файлы только в репозиторий Correcotor_Core v.2.0

Теория по работе с MPU6050: https://alexgyver.ru/arduino-mpu6050/


Обязательные библы (*можно скачать в папке libs):
  
  -I2Cdev
  
  -MPU6050

========= Полезные ссылки =========

Начало работы с ESP: https://alexgyver.ru/lessons/esp8266/?ysclid=m38kz3xxtj564148781

Тутор по чат боту телеграма : https://kit.alexgyver.ru/tutorials/fastbot/

Разрабатываемое приложение на GyverHub инфа: https://github.com/GyverLibs/GyverHub

# Туториал по GyverHub
## 1 Установка библиотеки
Добавить ESP в список плат в Arduino IDE
 - ESP8266 v2.7+ https://arduino.esp8266.com/stable/package_esp8266com_index.json
 - ESP32 v2+ https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json

<img src="https://github.com/user-attachments/assets/cfab045a-42d3-43ac-905c-e4d19a74af35" 
width=45% height=45%>

Для работы GyverHub необходимы библиотеки:
-	pubsubclient
-	arduinoWebSockets
-	StringUtils
-	GSON
-	Pairs
  
Основные понятия

-Устройство - микроконтроллер (далее МК) с программой на базе библиотеки GyverHub. МК является сервером, к которому подключаются клиенты

-Клиент - сайт или приложение GyverHub, которое подключено к устройству

-Имя сети (префикс) - уникальное имя сети "клиент-устройство", используется клиентом для поиска устройств и по сути является паролем, без которого не получится обнаружить устройство

## 2 Шаблон структуры кода
 (не является рабочей прошивкой, это описание функций, которые надо создать)
Инфа взята отсюда: https://github.com/GyverLibs/GyverHub/blob/main/docs/2.start.md
-	Объект GyverHub – с параметрами «Имя сети», «Имя устройства», «иконка».
-	Запустить объект GyverHub – hub.begin().
-	Создать функцию void.build – это билдер ИНТЕРФЕЙСА. В нем собирается весь интерфейс пользователя.
-	Запустить билдер – hub.onBuild().
-	Обновлять интерфейс в void loop – hub.tick().
```cpp

#include <GyverHub.h>
GyverHub hub("MyDevices", "ESP8266", "");  // имя сети, имя устройства, иконка

// билдер
void build(gh::Builder& b) {
}

void setup() {
    // подключение к WiFi..
    // настройка MQTT/Serial/Bluetooth..
    hub.onBuild(build); // подключаем билдер
    hub.begin();        // запускаем систему
}

void loop() {
    hub.tick();         // тикаем тут
}
```

### Настройка WiFi
Для подключения к WiFi роутеру можно использовать стандартный код для ESP:
```cpp
void setup() {
    WiFi.mode(WIFI_STA);
    WiFi.begin("SSID", "PASS");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    // ...
}
```

Работа в режиме точки доступа (телефон подключается к сети, раздаваемой ESP).
```cpp
void setup() {
    WiFi.mode(WIFI_AP);
    WiFi.softAP("My Device");
    // ...
}
```

