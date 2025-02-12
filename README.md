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

Для подключения к плате нужно подключиться к её созданной точке доуступа. Имя точки доступа указывается командой:
```cpp
WiFi.softAP("My Device");
```

Затем  в настройках приложения GyverHub задать имя префикса, указанного при создании объекта ```GyverHub hub("test", "ESP8266", "");``` и выбрать плату.

<img src="https://github.com/user-attachments/assets/971fce72-3ad2-4f77-9e18-831a3f189285" 
width=25% height=25%> 
<img src="https://github.com/user-attachments/assets/914855dc-e878-4453-bf49-ba33e3d95e45" 
width=25% height=25%> 

## 3 Примеры контейнеров (билдеров) кнопок из примера basic:

Их просто надо запихнуть в билдер. Для каждого объекта можно отдельный билдер, можно все в общий:

```cpp
//общий билдер для 2-х контейнеров
void build(gh::Builder& b) {
b.Button();
    {
        gh::Row r(b);  // контейнер сам создастся здесь
        b.Button();
        b.Button();
    }
}
//билдер для каждого контейнера отдельно

//билдер для 1-го
void builder_1 (gh::Builder& b) {
b.Button();
}

//билдер для 2-го
void builder_2 (gh::Builder& b) {
{
        gh::Row r(b);  // контейнер сам создастся здесь
        b.Button();
        b.Button();
    }
}
```

### Просто 1 кнопка на строке:
```cpp
b.Button();
```
<img src="https://github.com/user-attachments/assets/5980d098-4f2a-43c8-90a9-a6412ec3db3d" 
width=25% height=25%>

### Несколько кнопок на 1 строке:

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

### Кнопка с параметрами:
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

### Виджеты кнопок с сигналами о действий
```cpp

// =============== ПОДКЛЮЧЕНИЕ ПЕРЕМЕННОЙ ===============

    if (b.beginRow()) {
        // библиотека позволяет подключить к активному виджету переменную для чтения и записи
        // я создам статические переменные для ясности. Они могут быть глобальными и так далее
        // таким образом изменения останутся при перезагрузке страницы
        static int sld;
        static String inp;
        static bool sw;

        // для подключения нужно передать переменную по адресу
        // библиотека сама определит тип переменной и будет брать из неё значение и записывать при действиях
        // библиотека поддерживает все стандартные типы данных, а также некоторые свои (Pairs, Pos, Button, Log...)
        b.Slider(&sld).size(2);
        b.Input(&inp).size(2);

        // внутри обработки действия переменная уже будет иметь новое значение:
        if (b.Switch(&sw).size(1).click()) {
            Serial.print("switch: ");
            Serial.println(sw);
        }

        b.endRow();
    }
```


<img src="https://github.com/user-attachments/assets/e9b61cc9-c973-4ebf-a085-c237cadcef18" 
width=25% height=25%>

### Обновление показаний

```cpp

// ==================== ОБНОВЛЕНИЕ ====================

    // библиотека позволяет обновлять значения на виджетах. Это можно делать
    // - из основной программы (см. ниже)
    // - из билдера - но только при действиях по виджетам!

    // для отправки обновления нужно знать ИМЯ компонента. Его можно задать почти у всех виджетов
    // к функции добавляется подчёркивание, всё остальное - как раньше

    if (b.beginRow()) {
        b.Label_(F("label")).size(2).value("default");  // с указанием стандартного значения

        if (b.Button().size(1).click()) {
            hub.update(F("label")).value(random(100, 500));
        }

        b.endRow();
    }
```


<img src="https://github.com/user-attachments/assets/4cf89f87-d1b8-4a70-83c0-9974de38f7ca" 
width=25% height=25%>

### Пример управления светодиодом на 4 порту ESP по кнопке

```cpp
#include <GyverHub.h>
#define LED2 D4

GyverHub hub("test", "ESP8266", "");  // имя сети, имя устройства, иконка
bool sost = true; //состояние светодиода
gh::Flag gflag;   // флаг кнопки  данный флаг сам сбросится в false при проверке!
// билдер
void build(gh::Builder& b) {
  if (b.beginRow()) {
        // параметры виджета можно задавать цепочкой. Например:
        b.Button().label(F("LED2")).color(gh::Colors::Red);
        b.widget.attach(&gflag);
        b.endRow();
    }
}

void setup() {
    // подключение к WiFi..
    pinMode(LED2, OUTPUT);
    WiFi.mode(WIFI_AP);
    WiFi.softAP("My Device");
    // ...

    // настройка MQTT/Serial/Bluetooth..
    hub.onBuild(build); // подключаем билдер
    hub.begin();        // запускаем систему
}

void loop() {
    hub.tick();         // тикаем тут
    if (gflag) {
      // здесь gflag уже false
      sost = !sost;
      digitalWrite(LED2, sost);
    }
}
```

### Пример управления светодиодом на 4 порту ESP по свитчу

```cpp
#include <GyverHub.h>
#define LED2 D4

GyverHub hub("test", "ESP8266", "");  // имя сети, имя устройства, иконка
bool gflag = 0;   // флаг свитча
// билдер
void build(gh::Builder& b) {
  if (b.beginRow()) {
      //проверяем состояние свитча. Оно записывается в переменную gflag
      b.Switch(&gflag).size(1).click();   
   }
  b.endRow();
}

void setup() {
    // подключение к WiFi..
    pinMode(LED2, OUTPUT);
    WiFi.mode(WIFI_AP);
    WiFi.softAP("My Device");
    // ...

    // настройка MQTT/Serial/Bluetooth..
    hub.onBuild(build); // подключаем билдер
    hub.begin();        // запускаем систему
}

void loop() {
    hub.tick();         // тикаем тут
    if(!gflag) {
      digitalWrite(LED2, HIGH);
    }
    else {
      digitalWrite(LED2, LOW);
    }
}
```

<img src="https://github.com/user-attachments/assets/1eaa2170-972a-43ff-91aa-429ad39a19cf" 
width=25% height=25%>

### Dpad (тач пад с 4 кнопками) гайвер называет крестовина

Для использования Дпада и джойстика к их виджету нужно привязать переменную типа Pos: ```gh::Pos pos_dpad``` для сохранения в неё показаний. Использовать переменную pos_dpad нужно так:

```
pos_dpad.changed(); - вернет 1 если значение менялось и 0 если нет

pos_dpad.x; - координату x. Для дпада: 0 - не нажата, 1 - клик по правой стрелке, -1 клик по левой стрелке (в сооттветсвии со скрином 2 с приложения)

pos_dpad.y; - координату y. Для дпада: 0 - не нажата, 1 - клик по верхней стрелке, -1 клик по нижней стрелке (в сооттветсвии со скрином 2 с приложения)

есть еще .dist - расстояние до точки

и вот эти

// точка лежит внутри прямоугольника
bool inRect(int16_t rx, int16_t ry, uint16_t w, uint16_t h);

// точка лежит внутри окружности
bool inCircle(int16_t cx, int16_t cy, uint16_t r);
```

Пример вывода состояния Dpad

```cpp
#include <GyverHub.h>

GyverHub hub("test", "ESP8266", "");  // имя сети, имя устройства, иконка

gh::Pos pos_dpad; //переменная для dpad ТОЛЬКО типа POS (встроенный тип данных)
// билдер
void build(gh::Builder& b) {
  if (b.beginRow()) {
      //прикручиваем dpad к переменной pos_dpad по адресу
      if (b.Dpad(&pos_dpad).color(gh::Colors::Blue).click()) {
        //после клика выводим показания
        Serial.print("dpad: ");
        Serial.print(pos_dpad.x);
        Serial.print(",");
        Serial.println(pos_dpad.y);
      }
      //b.Space(); //пустой виджет меняет размер
  }
  b.endRow();
}

void setup() {
    // подключение к WiFi..
    WiFi.mode(WIFI_AP);
    WiFi.softAP("My Device");
    // ...
    Serial.begin(115200);
    // настройка MQTT/Serial/Bluetooth..
    hub.onBuild(build); // подключаем билдер
    hub.begin();        // запускаем систему
}

void loop() {
    hub.tick();         // тикаем тут
}
```

<img src="https://github.com/user-attachments/assets/1bcd6da9-984d-4c92-919a-360dddbf3034" 
width=25% height=25%>
<img src="https://github.com/user-attachments/assets/7b1c4360-a7b4-4c3c-aa11-cfae466ba522" 
width=25% height=25%>
<img src="https://github.com/user-attachments/assets/b80f7225-14ed-45fe-a59c-e7cbde21a109" 
width=25% height=25%>
