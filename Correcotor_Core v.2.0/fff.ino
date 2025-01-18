//#include <AnalogKey.h>
//#include <GyverButton.h>
#include <Wire.h>
#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"

//вставка
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
//вставка

#define BUFFER_SIZE 100

//вставка
#define WIFI_SSID "CPOD"
#define WIFI_PASSWORD "ApoX"
#define BOT_TOKEN "TOKEN_BOT"

const unsigned long BOT_MTBS = 1000; // время обновления
WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);
unsigned long bot_lasttime;
//вставка

MPU6050 mpu1 = 0x68;              // адрес датчика вверху спины
MPU6050 mpu2 = 0x69;              // адрес датчика снизу спины

uint8_t devStatus;                // возвращаяет статус устройства после каждого действия (0 = success, !0 = error)
float ypr_one[3];                 //массив хранения текущих показаний вверху спины
float ypr_two[3];                 //массив хранения показний внизу спины
float normaly1[3];                // массив хвранения настроенных углов углов внизу спины
float normaly2[3];                // массив хранения настроенных углов вверху спины
uint8_t fifoBuffer1[45];          // буфер1
uint8_t fifoBuffer2[45];          // буфер2
bool knopka;                      // кнопка
bool flag_motor; 
int16_t ax, ay, az;
int16_t gx, gy, gz;

void setup() {
  Serial.begin(115200);

//вставка
 Serial.print("Connecting to Wifi SSID ");
  Serial.print(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  secured_client.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.print("\nWiFi connected. IP address: ");
  Serial.println(WiFi.localIP());
//вставка
  
  Wire.begin();
  Wire.setClock(1000000UL);       // разгоняем шину на максимум
  pinMode(10, OUTPUT);            // порт вибро
  pinMode(2, INPUT);              // порт кнопки
  pinMode(5, OUTPUT);             // порт бузера
  initialize_mpu1 ();             // инициализация DMP_0x68
  initialize_mpu2 ();             // инициализация DMP_0x69


     if ((text == "/calibration of sensors") || (text == "calibration of sensors"))
    {
       //----------Калибровка датчиков----------------------
      Serial.println("Start kalibrovka");
      delay(1000);
      while (1){
        knopka = digitalRead(2);
        if (knopka){
          kalib_mpu1();
          tone (5, 1000, 300);
          delay(300);
          kalib_mpu2();
          tone (5, 1000, 300);
          delay(300);
          break;
      }
    }
      bot.sendMessage(chat_id, "Калибровка датчиков завершена", "");
    }
//вставка
     if ((text == "/calibration for humans") || (text == "calibration for humans"))
    {
 //----------Калибровка под человека----------------------
      Serial.println("Primite ydobnoe pologenie");
      delay(1000);
      Serial.println("Nagmite na knopky, esli gotovi");
      while (1){
        knopka = digitalRead(2);
        get_normaly1();              // получение идеальных углов вверху спины
        get_normaly2();              // получение идеальных углов внизу спины
        if (knopka){
           break;
      }
    }
      bot.sendMessage(chat_id, "Калибровка под человека завершена", "");
    }
    
    tone (5, 1000, 300);
    Serial.println("Pologenie vibrano");
    bot.sendMessage(chat_id, "Положение выбрано", "");
    vivod_norm1();
    vivod_norm2();
    tone (5, 1000, 300);
    delay(1000);
}
void loop() {
  static uint32_t tmr1;
  static uint32_t tmr2;
  static uint32_t tmr3_motor;
  if (millis() - bot_lasttime > BOT_MTBS)
  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while (numNewMessages)
    {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }

    bot_lasttime = millis();
  }
  if (millis() - tmr1 >= 11) {      // таймер на 11 мс (на всякий случай)
     getdata_mpu1();
     tmr1 = millis();               // сброс таймера 
  }
  if (millis() - tmr2 >= 11) {      // таймер на 11 мс (на всякий случай)
     getdata_mpu2();
     tmr2 = millis();               // сброс таймера 
  }
  //abs(degrees(normaly1[0]) - degrees(ypr_one[0])) > 10 || ДАТЧИК 0x68 ПО Х ДРЕЙФУЕТ, УБРАНО ИЗ УСЛОВИЯ НИЖЕ
  //---верхний датчик---------------  
  if (abs(degrees(normaly1[1]) - degrees(ypr_one[1])) > 10 || abs(degrees(normaly1[2]) - degrees(ypr_one[2])) > 10) {
    //Serial.println ("vipra                                                                                                                                                     mis");  
    tone (5, 1000); 
    bot.sendMessage(chat_id, "Выпрямите спину", "");
  }
  else {
    //Serial.println ("ok");  
    noTone (5); 
}
//abs(degrees(normaly2[0]) - degrees(ypr_two[0])) > 10 ||
  //------нижний датчик-------------------
  if ( abs(degrees(normaly2[1]) - degrees(ypr_two[1])) > 12 || abs(degrees(normaly2[2]) - degrees(ypr_two[2])) > 12){
     if (!flag_motor){
      digitalWrite(10,HIGH);
      bot.sendMessage(chat_id, "Выпрямите спину", "");
      flag_motor = true;
      tmr3_motor = millis();
     }
     if (millis() -  tmr3_motor > 2000  && flag_motor) {
      digitalWrite(10,LOW );
     }    
}
  else {
     digitalWrite(10,LOW );
     flag_motor = false;  
  }
  
}


//**************Функции**************

//иниациализация mpu1 вверху спины
void initialize_mpu1 () {
  // инициализация DMP_0x68
  mpu1.initialize();
  Serial.println(mpu1.testConnection() ? F("MPU6050_0x68 connection successful") : F("MPU6050_0x68 connection failed"));
  Serial.println(F("Initializing DMP_0x68..."));
  devStatus = mpu1.dmpInitialize();
  // make sure it worked (returns 0 if so)
  if (devStatus == 0) {
    // turn on the DMP, now that it's ready
    Serial.println(F("Enabling DMP_0x68..."));
    mpu1.setDMPEnabled(true);
    Serial.println(F("DMP_0x68 ready! Waiting for first interrupt..."));
  } else {
    // ERROR!
    // 1 = initial memory load failed
    // 2 = DMP configuration updates failed
    Serial.print(F("DMP_0x68  Initialization failed (code "));
    Serial.print(devStatus);
    Serial.println(F(")"));
  }
}

//иниациализация mpu2 внизу спины
void initialize_mpu2 () {
  // инициализация DMP_0x69
  mpu2.initialize();
  Serial.println(mpu2.testConnection() ? F("MPU6050_0x69 connection successful") : F("MPU6050_0x69 connection failed"));
  Serial.println(F("Initializing DMP_0x69..."));
  devStatus = mpu2.dmpInitialize();
  // make sure it worked (returns 0 if so)
  if (devStatus == 0) {
    // turn on the DMP, now that it's ready
    Serial.println(F("Enabling DMP_0x69..."));
    mpu2.setDMPEnabled(true);
    Serial.println(F("DMP_0x69 ready! Waiting for first interrupt..."));
  } else {
    // ERROR!
    // 1 = initial memory load failed
    // 2 = DMP configuration updates failed
    Serial.print(F("DMP_0x69 Initialization failed (code "));
    Serial.print(devStatus);
    Serial.println(F(")"));
  }
}

// Получение идеальных углов вверху спины
void get_normaly1() {
  static uint32_t tmr1;
  if (millis() - tmr1 >= 11) {
    if (mpu1.dmpGetCurrentFIFOPacket(fifoBuffer1)) {
      // переменные для расчёта (ypr можно вынести в глобал)
      Quaternion q_1;
      VectorFloat gravity_1;
      // расчёты
      mpu1.dmpGetQuaternion(&q_1, fifoBuffer1);
      mpu1.dmpGetGravity(&gravity_1, &q_1);
      mpu1.dmpGetYawPitchRoll(normaly1, &q_1, &gravity_1);
      tmr1 = millis();  // сброс таймера
      }
  }
}

// Получение идеальных углов внизу спины
void get_normaly2() {
  static uint32_t tmr2;
  if (millis() - tmr2 >= 11) {
    if (mpu2.dmpGetCurrentFIFOPacket(fifoBuffer2)) {
      // переменные для расчёта (ypr можно вынести в глобал)
      Quaternion q_2;
      VectorFloat gravity_2;
      // расчёты
      mpu2.dmpGetQuaternion(&q_2, fifoBuffer2);
      mpu2.dmpGetGravity(&gravity_2, &q_2);
      mpu2.dmpGetYawPitchRoll(normaly2, &q_2, &gravity_2);
      tmr2 = millis();  // сброс таймера
      }
  }
}

//Получение показаний углов mpu1 0x68 в динамике
void getdata_mpu1() {
     if (mpu1.dmpGetCurrentFIFOPacket(fifoBuffer1)){
      // переменные для расчёта датчикка 1
      Quaternion q_1;
      VectorFloat gravity_1;
      // расчёты для первого датчика с адресом 0x68 
      mpu1.dmpGetQuaternion(&q_1, fifoBuffer1);
      mpu1.dmpGetGravity(&gravity_1, &q_1);
      mpu1.dmpGetYawPitchRoll(ypr_one, &q_1, &gravity_1);
      vivod_one(ypr_one);
    }
}

//Получение показаний углов mpu2 0x69 в динамике
void getdata_mpu2() {
      if (mpu2.dmpGetCurrentFIFOPacket(fifoBuffer2)){
      // переменные для расчёта датчикка 2
      Quaternion q_2;
      VectorFloat gravity_2;
      // расчёты для второго датчика с адресом 0x69 
      mpu2.dmpGetQuaternion(&q_2, fifoBuffer2);
      mpu2.dmpGetGravity(&gravity_2, &q_2);
      mpu2.dmpGetYawPitchRoll(ypr_two, &q_2, &gravity_2);
      vivod_two(ypr_two);
    }
}
//Вывод показаний в порт с датчика №1 вверху спины
void vivod_one (float mp_one[]){  
  Serial.print("mpu1: ");
  Serial.print(degrees(normaly1[0]) - degrees(mp_one[0])); // вокруг оси Z
  Serial.print(',');
  Serial.print(degrees(normaly1[1]) - degrees(mp_one[1])); // вокруг оси Y
  Serial.print(',');
  Serial.print(degrees(normaly1[2]) - degrees(mp_one[2])); // вокруг оси X
  Serial.println();
}

//Вывод показаний в порт с датчика №2 низу спины
void vivod_two (float mp_two[]){
  Serial.print("mpu2: ");
  Serial.print(degrees(normaly2[0]) - degrees(mp_two[0])); // вокруг оси Z
  Serial.print(',');
  Serial.print(degrees(normaly2[1]) - degrees(mp_two[1])); // вокруг оси Y
  Serial.print(',');
  Serial.print(degrees(normaly2[2]) - degrees(mp_two[2])); // вокруг оси X
  Serial.println();   
}
void vivod_norm1() {
    Serial.print("mpu1: ");
  Serial.print(degrees(normaly1[0])); // вокруг оси Z
  Serial.print(',');
  Serial.print(degrees(normaly1[1])); // вокруг оси Y
  Serial.print(',');
  Serial.print(degrees(normaly1[2])); // вокруг оси X
  Serial.println();
}
void vivod_norm2() {
    Serial.print("mpu2: ");
  Serial.print(degrees(normaly2[0])); // вокруг оси Z
  Serial.print(',');
  Serial.print(degrees(normaly2[1])); // вокруг оси Y
  Serial.print(',');
  Serial.print(degrees(normaly2[2])); // вокруг оси X
  Serial.println();
}
void kalib_mpu1 () {
  mpu1.setXAccelOffset(0);
  mpu1.setYAccelOffset(0);
  mpu1.setZAccelOffset(0);
  mpu1.setXGyroOffset(0);
  mpu1.setYGyroOffset(0);
  mpu1.setZGyroOffset(0);
  // выводим начальные значения
  mpu1.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  Serial.print(ax); Serial.print(" ");
  Serial.print(ay); Serial.print(" ");
  Serial.print(az); Serial.print(" ");
  Serial.print(gx); Serial.print(" ");
  Serial.print(gy); Serial.print(" ");
  Serial.println(gz);
  calibration_1();
  // выводим значения после калибровки
  mpu1.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  Serial.print(ax); Serial.print(" ");
  Serial.print(ay); Serial.print(" ");
  Serial.print(az); Serial.print(" ");
  Serial.print(gx); Serial.print(" ");
  Serial.print(gy); Serial.print(" ");
  Serial.println(gz);
}

void kalib_mpu2 () {
  mpu2.setXAccelOffset(0);
  mpu2.setYAccelOffset(0);
  mpu2.setZAccelOffset(0);
  mpu2.setXGyroOffset(0);
  mpu2.setYGyroOffset(0);
  mpu2.setZGyroOffset(0);
  // выводим начальные значения
  mpu2.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  Serial.print(ax); Serial.print(" ");
  Serial.print(ay); Serial.print(" ");
  Serial.print(az); Serial.print(" ");
  Serial.print(gx); Serial.print(" ");
  Serial.print(gy); Serial.print(" ");
  Serial.println(gz);
  calibration_2();
  // выводим значения после калибровки
  mpu2.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  Serial.print(ax); Serial.print(" ");
  Serial.print(ay); Serial.print(" ");
  Serial.print(az); Serial.print(" ");
  Serial.print(gx); Serial.print(" ");
  Serial.print(gy); Serial.print(" ");
  Serial.println(gz);
}

void calibration_1() {
  long offsets[6];
  long offsetsOld[6];
  int16_t mpuGet[6];
  // используем стандартную точность
  mpu1.setFullScaleAccelRange(MPU6050_ACCEL_FS_2);
  mpu1.setFullScaleGyroRange(MPU6050_GYRO_FS_250);
  // обнуляем оффсеты
  mpu1.setXAccelOffset(0);
  mpu1.setYAccelOffset(0);
  mpu1.setZAccelOffset(0);
  mpu1.setXGyroOffset(0);
  mpu1.setYGyroOffset(0);
  mpu1.setZGyroOffset(0);
  delay(1000);
  Serial.println("Calibration_1 start. It will take about 5 seconds");
  for (byte n = 0; n < 10; n++) {     // 10 итераций калибровки
    for (byte j = 0; j < 6; j++) {    // обнуляем калибровочный массив
      offsets[j] = 0;
    }
    for (byte i = 0; i < 100 + BUFFER_SIZE; i++) { // делаем BUFFER_SIZE измерений для усреднения
      mpu1.getMotion6(&mpuGet[0], &mpuGet[1], &mpuGet[2], &mpuGet[3], &mpuGet[4], &mpuGet[5]);
      if (i >= 99) {                         // пропускаем первые 99 измерений
        for (byte j = 0; j < 6; j++) {
          offsets[j] += (long)mpuGet[j];   // записываем в калибровочный массив
        }
      }
    }
    for (byte i = 0; i < 6; i++) {
      offsets[i] = offsetsOld[i] - ((long)offsets[i] / BUFFER_SIZE); // учитываем предыдущую калибровку
      if (i == 2) offsets[i] += 16384;                               // если ось Z, калибруем в 16384
      offsetsOld[i] = offsets[i];
    }
    // ставим новые оффсеты
    mpu1.setXAccelOffset(offsets[0] / 8);
    mpu1.setYAccelOffset(offsets[1] / 8);
    mpu1.setZAccelOffset(offsets[2] / 8);
    mpu1.setXGyroOffset(offsets[3] / 4);
    mpu1.setYGyroOffset(offsets[4] / 4);
    mpu1.setZGyroOffset(offsets[5] / 4);
    delay(2);
  }
    // выводим в порт
    Serial.println("Calibration_1 end. Your offsets:");
    Serial.println("accX accY accZ gyrX gyrY gyrZ");
    Serial.print(mpu1.getXAccelOffset()); Serial.print(", ");
    Serial.print(mpu1.getYAccelOffset()); Serial.print(", ");
    Serial.print(mpu1.getZAccelOffset()); Serial.print(", ");
    Serial.print(mpu1.getXGyroOffset()); Serial.print(", ");
    Serial.print(mpu1.getYGyroOffset()); Serial.print(", ");
    Serial.print(mpu1.getZGyroOffset()); Serial.println(" ");
    Serial.println(" ");
}

void calibration_2 () {
  long offsets[6];
  long offsetsOld[6];
  int16_t mpuGet[6];
  // используем стандартную точность
  mpu2.setFullScaleAccelRange(MPU6050_ACCEL_FS_2);
  mpu2.setFullScaleGyroRange(MPU6050_GYRO_FS_250);
  // обнуляем оффсеты
  mpu2.setXAccelOffset(0);
  mpu2.setYAccelOffset(0);
  mpu2.setZAccelOffset(0);
  mpu2.setXGyroOffset(0);
  mpu2.setYGyroOffset(0);
  mpu2.setZGyroOffset(0);
  delay(1000);
  Serial.println("Calibration_2 start. It will take about 5 seconds");
  for (byte n = 0; n < 10; n++) {     // 10 итераций калибровки
    for (byte j = 0; j < 6; j++) {    // обнуляем калибровочный массив
      offsets[j] = 0;
    }
    for (byte i = 0; i < 100 + BUFFER_SIZE; i++) { // делаем BUFFER_SIZE измерений для усреднения
      mpu2.getMotion6(&mpuGet[0], &mpuGet[1], &mpuGet[2], &mpuGet[3], &mpuGet[4], &mpuGet[5]);
      if (i >= 99) {                         // пропускаем первые 99 измерений
        for (byte j = 0; j < 6; j++) {
          offsets[j] += (long)mpuGet[j];   // записываем в калибровочный массив
        }
      }
    }
    for (byte i = 0; i < 6; i++) {
      offsets[i] = offsetsOld[i] - ((long)offsets[i] / BUFFER_SIZE); // учитываем предыдущую калибровку
      if (i == 2) offsets[i] += 16384;                               // если ось Z, калибруем в 16384
      offsetsOld[i] = offsets[i];
    }
    // ставим новые оффсеты
    mpu2.setXAccelOffset(offsets[0] / 8);
    mpu2.setYAccelOffset(offsets[1] / 8);
    mpu2.setZAccelOffset(offsets[2] / 8);
    mpu2.setXGyroOffset(offsets[3] / 4);
    mpu2.setYGyroOffset(offsets[4] / 4);
    mpu2.setZGyroOffset(offsets[5] / 4);
    delay(2);
  }
    // выводим в порт
    Serial.println("Calibration_2 end. Your offsets:");
    Serial.println("accX accY accZ gyrX gyrY gyrZ");
    Serial.print(mpu2.getXAccelOffset()); Serial.print(", ");
    Serial.print(mpu2.getYAccelOffset()); Serial.print(", ");
    Serial.print(mpu2.getZAccelOffset()); Serial.print(", ");
    Serial.print(mpu2.getXGyroOffset()); Serial.print(", ");
    Serial.print(mpu2.getYGyroOffset()); Serial.print(", ");
    Serial.print(mpu2.getZGyroOffset()); Serial.println(" ");
    Serial.println(" ");
}
void handleNewMessages(int numNewMessages)
{
  Serial.print("handleNewMessages ");
  Serial.println(numNewMessages);

  for (int i = 0; i < numNewMessages; i++)
  {
    String chat_id = bot.messages[i].chat_id;
    String text = bot.messages[i].text;
    text.toLowerCase();
    String from_name = bot.messages[i].from_name;
    if (from_name == "")
      from_name = "Guest";
    if ((text == "/ledon") || (text == "ledon"))
    {
      digitalWrite(18, HIGH);
      bot.sendMessage(chat_id, "Светодиод включен", "");
    }
    if ((text == "/ledoff") || (text == "ledoff"))
    {
      digitalWrite(18, LOW);
      bot.sendMessage(chat_id, "Светодиод выключен", "");
    }
   if ((text == "/start") || (text == "start") || (text == "/help") || (text == "help"))
    {
      bot.sendMessage(chat_id, "Привет, " + from_name + "!", "");
      bot.sendMessage(chat_id, "Я контроллер Йотик 32. Я принимаю следующие команды:", "");
      bot.sendMessage(chat_id, "/ledon или /ledoff - включить/выключить светодиод", "");
      bot.sendMessage(chat_id, "/hall - считать значение с датчика Холла", "");
      bot.sendMessage(chat_id, "Команды можно писать без слэша слева и буквами любого регистра", "");
    }
  }
}
