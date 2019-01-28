#include <SPI.h>          // библиотека для работы с шиной SPI
#include "nRF24L01.h"     // библиотека радиомодуля
#include "RF24.h"         // ещё библиотека радиомодуля

#define thrustPIN 2
#define rudderPIN 0
#define aileronPIN 1
#define elevatorPIN 3

RF24 radio(9, 10); // "создать" модуль на пинах 9 и 10 Для Уно

int data[4];

byte address[][6] = {"1Node", "2Node", "3Node", "4Node", "5Node", "6Node"}; //возможные номера труб

byte counter;

void setup() {
  Serial.begin(9600); //открываем порт для связи с ПК

  radio.begin(); //активировать модуль
  radio.setAutoAck(1);         //режим подтверждения приёма, 1 вкл 0 выкл
  radio.setRetries(0, 15);    //(время между попыткой достучаться, число попыток)
  radio.enableAckPayload();    //разрешить отсылку данных в ответ на входящий сигнал
  radio.setPayloadSize(32);     //размер пакета, в байтах

  radio.openWritingPipe(address[0]);   //мы - труба 0, открываем канал для передачи данных
  radio.setChannel(0x60);  //выбираем канал (в котором нет шумов!)

  radio.setPALevel (RF24_PA_MAX); //уровень мощности передатчика. На выбор RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX
  radio.setDataRate (RF24_250KBPS); //скорость обмена. На выбор RF24_2MBPS, RF24_1MBPS, RF24_250KBPS
  //должна быть одинакова на приёмнике и передатчике!
  //при самой низкой скорости имеем самую высокую чувствительность и дальность!!

  radio.powerUp(); //начать работу
  radio.stopListening();  //не слушаем радиоэфир, мы передатчик
}

void loop() {
  data[0] = analogRead(thrustPIN);
  data[1] = analogRead(rudderPIN);
  data[2] = analogRead(aileronPIN);
  data[3] = analogRead(elevatorPIN);
  
//  Serial.print("thrust: "); Serial.print(data[0]);
//  Serial.print("rudder: "); Serial.print(data[1]);
//  Serial.print("ail: "); Serial.print(data[2]);
//  Serial.print("elevator: "); Serial.println(data[3]);
  
  radio.write(&data, sizeof(data));
  delay(50);
}
