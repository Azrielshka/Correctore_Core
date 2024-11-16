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

Нейросеть можно прикурить вот так!!! https://alexgyver.ru/aiframe/

Разрабатываемое приложение на GyverHub инфа: https://github.com/GyverLibs/GyverHub

# Туториал по GyverHub
## 1 Установка библиотеки
Добавить ESP в список плат в Arduino IDE
 - ESP8266 v2.7+ https://arduino.esp8266.com/stable/package_esp8266com_index.json
 - ESP32 v2+ https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json

<img src="https://github.com/user-attachments/assets/cfab045a-42d3-43ac-905c-e4d19a74af35" 
width=45% height=45%>

Выбрать менеджер плат:
-	Скачать esp8266
-	Скачать esp32

<img src="https://github.com/user-attachments/assets/6088c21d-1ff2-4e80-adc2-159ffded3840" 
width=30% height=30%>


Для работы GyverHub необходимы библиотеки:
-	pubsubclient: https://github.com/knolleary/pubsubclient
-	arduinoWebSockets: https://github.com/Links2004/arduinoWebSockets
-	StringUtils: https://github.com/GyverLibs/StringUtils
-	GSON: https://github.com/GyverLibs/GSON
-	Pairs: https://github.com/GyverLibs/Pairs
  
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
    Serial.println();
    Serial.println(WiFi.localIP());
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
Файл basic для теста контейнеров кнопки (для работы в режиме точки доступа, нужно закомментировать строки 225 - 238 и расскоментировать строки 241 - 244)

## 3 Примеры контейнеров (билдеров) кнопок из примера basic:

Просто 1 кнопка на строке:
```cpp
b.Button();
```
<img src="https://github.com/user-attachments/assets/5980d098-4f2a-43c8-90a9-a6412ec3db3d" 
width=25% height=25%>

Несколько кнопок на 1 строке:

```cpp
// есть ещё такой трюк - контейнер сам себя закроет в рамках блока {}
    // назвать его можно как угодно, передать во внутрь аргумент билдера
    {
        gh::Row r(b);  // контейнер сам создастся здесь
        b.Button();
        b.Button();
    }  // контейнер сам закроется здесь
```
<img src="https://github.com/user-attachments/assets/2e62121c-0d34-48e3-bad1-e5faac0ded6a" 
width=25% height=25%>

Кнопка с параметрами:
```cpp
if (b.beginRow()) {
        // параметры виджета можно задавать цепочкой. Например:
        b.Button().label(F("my button 1")).color(gh::Colors::Red);

        // также можно продолжить настраивать ПРЕДЫДУЩИЙ виджет, обратившись к widget:
        b.Button();                        // кнопка без параметров
        b.widget.label(F("my button 2"));  // настраиваем кнопку выше
        b.widget.color(gh::Colors::Blue);  // её же

        b.endRow();
    }
```
<img src="https://github.com/user-attachments/assets/2132f279-a544-4d14-8d57-5af2c48c412f" 
width=25% height=25%>

<img src="https://github.com/user-attachments/assets/e9b61cc9-c973-4ebf-a085-c237cadcef18" 
width=25% height=25%>

<img src="https://github.com/user-attachments/assets/4cf89f87-d1b8-4a70-83c0-9974de38f7ca" 
width=25% height=25%>


