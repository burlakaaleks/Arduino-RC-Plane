#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include <Servo.h>

#define ENG_PIN 3
#define AIL_PIN 9
#define ELE_PIN 5
#define RUD_PIN 10

Servo engine;
Servo ailerons;
Servo elevator;
Servo rudder;

RF24 radio(4,7); //В папку Libraries нужно положить библиотеку с включенным SOFT_SPI
int data[4];

byte address[][6] = {"1Node","2Node","3Node","4Node","5Node","6Node"};  //возможные номера труб

void setup(){
  Serial.begin(9600); //открываем порт для связи с ПК
  radio.begin(); //активировать модуль
  radio.setAutoAck(1);         //режим подтверждения приёма, 1 вкл 0 выкл
  radio.setRetries(0,15);     //(время между попыткой достучаться, число попыток)
  radio.enableAckPayload();    //разрешить отсылку данных в ответ на входящий сигнал
  radio.setPayloadSize(32);     //размер пакета, в байтах

  radio.openReadingPipe(1,address[0]);      //хотим слушать трубу 0
  radio.setChannel(0x60);  //выбираем канал (в котором нет шумов!)

  radio.setPALevel (RF24_PA_MAX); //уровень мощности передатчика. На выбор RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX
  radio.setDataRate (RF24_250KBPS); //скорость обмена. На выбор RF24_2MBPS, RF24_1MBPS, RF24_250KBPS
  //должна быть одинакова на приёмнике и передатчике!
  //при самой низкой скорости имеем самую высокую чувствительность и дальность!!
  
  radio.powerUp(); //начать работу
  radio.startListening();  //начинаем слушать эфир, мы приёмный модуль

  engine.attach(ENG_PIN);
  ailerons.attach(AIL_PIN);
  elevator.attach(ELE_PIN);
  rudder.attach(RUD_PIN);

  ailerons.write(85);
  elevator.write(40);
  engine.writeMicroseconds(2300);
  delay(2000);
  engine.writeMicroseconds(800); 
  delay(6000);
}

void loop() {
    byte pipeNo;                          
    while( radio.available(&pipeNo)){    // слушаем эфир со всех труб
      radio.read( &data, sizeof(data) );         // чиатем входящий сигнал

      int eng_val = map(data[0], 880, 140, 800, 2300);
      eng_val = constrain(eng_val, 810, 2300);
      engine.writeMicroseconds(eng_val);

      int rud_val = map(data[1], 200, 800, 0, 100);
      rud_val = constrain(rud_val, 0, 100);
      rudder.write(rud_val);

      int ail_val = map(data[2], 200, 800, 50, 130);
      ail_val = constrain(ail_val, 40, 130);
      ailerons.write(ail_val);
 
      int ele_val = map(data[3], 200, 800, 0, 90);
      ele_val = constrain(ele_val, 0, 80);
      elevator.write(ele_val);

//      Serial.print("thrust: "); Serial.print(data[0]);
//      Serial.print("rudder: "); Serial.print(data[1]);
//      Serial.print("ail: "); Serial.print(data[2]);
//      Serial.print("rv: "); Serial.println(data[3]);
   }
}
