#include <Arduino.h>
#include "timer.hpp"
#include "setWeb.hpp"
#include "eeprom.hpp"
#include "key.hpp"
#define OUT_1 4
#define OUT_2 16
#define OUT_3 17
#define OUT_4 18
#define OUT_5 19

#define IN_1 26
#define IN_2 25
#define IN_3 33
#define IN_4 32
#define IN_5 35
#define IN_6 34

#define CAMERA_SENSOR_IN (!digitalRead(IN_1))
#define CAMERA_JUDGE_IN (!digitalRead(IN_2))
#define SENSOR2_IN (!digitalRead(IN_4))
#define SET_BOTTOM (!digitalRead(IN_6))
// #define NG_IN           digitalRead(IN_3)
#define CAMERA_OUT(x) digitalWrite(OUT_1, x)
#define CYLINDER_OUT(x) digitalWrite(OUT_2, x)
#define LED_OUT(x) digitalWrite(OUT_5, x)
// #define ALERT_OUT(x)    digitalWrite(OUT_2,x)
// #define SENSOR_OUT(x)    digitalWrite(OUT_4,x)
// #define Pushe_OUT(x)    digitalWrite(OUT_5,x)
IntervalTimer timer(memory_data.waitTime[0]);
KeyInput setKey(IN_6, 100, 1000);
bool setFlag = false;
void setup() {
  pinMode(OUT_1, OUTPUT);
  pinMode(OUT_2, OUTPUT);
  pinMode(OUT_3, OUTPUT);
  pinMode(OUT_4, OUTPUT);
  pinMode(OUT_5, OUTPUT);

  pinMode(IN_1, INPUT);
  pinMode(IN_2, INPUT);
  pinMode(IN_3, INPUT);
  pinMode(IN_4, INPUT);
  pinMode(IN_5, INPUT);
  Serial.begin(9600);
  init_EEPROM();
  // SENSOR_OUT(1);
  // put your setup code here, to run once:
}

void loop() {
  static int sq = 0;
  static IntervalTimer timer(100);
  LED_OUT(setWebMain(setKey));

  switch (sq) {
    case 0:  // カメラセンサーが検知されたら
      if (CAMERA_SENSOR_IN) {
        sq++;
        timer.setTime(memory_data.waitTime[0]);
      }
      break;

    case 1:  // カメラジャッジ開始

      CAMERA_OUT(1);
      sq++;
      break;

    case 2:  // カメラNGの場合は
      if (CAMERA_JUDGE_IN) {
        sq++;
      }
      break;

    case 3:  // シリンダーで押し出し
      CYLINDER_OUT(1);
      timer.init();
      sq++;
      break;

    case 4:  // シリンダーが動作するまで待機
      if (timer.isWait()) {
        CYLINDER_OUT(0);
        sq++;
        timer.init();
      }
      break;

    case 5:  // シリンダーで押し出したか確認
      if (SENSOR2_IN) {
        sq = 0;
        timer.init();
      }
      break;

    default:
      break;
  }
}
