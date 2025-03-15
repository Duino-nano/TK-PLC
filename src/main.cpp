#include <Arduino.h>
#include "timer.hpp"
#include "eeprom.hpp"
#include "key.hpp"
#define OUT_1 4
#define OUT_2 16
#define OUT_3 17
#define OUT_4 18
#define OUT_5 19

#define IN_1 26  // OK
#define IN_2 25  // NG
#define IN_3 33  // 近接センサー
#define IN_4 32
#define IN_5 35
#define IN_6 34

#define CAMERA_JUDGE_OK (!digitalRead(IN_1))
#define CAMERA_JUDGE_NG (!digitalRead(IN_2))
#define CYLINDER_SENSOR_IN (!digitalRead(IN_3))
#define SET_BOTTOM (!digitalRead(IN_6))
// #define NG_IN           digitalRead(IN_3)
// #define CAMERA_OUT(x) digitalWrite(OUT_1, x)
#define CYLINDER_OUT(x) digitalWrite(OUT_1, x)
#define LED_OUT(x) digitalWrite(OUT_5, x)
// #define ALERT_OUT(x)    digitalWrite(OUT_2,x)
// #define SENSOR_OUT(x)    digitalWrite(OUT_4,x)
// #define Pushe_OUT(x)    digitalWrite(OUT_5,x)
void cameraJudgeMain();
void cylinderMain();
void timeUpFlag();
void cylinderTimeOutMain();
uint8_t getFlagDataCount();
IntervalTimer cylinderTimer(memory_data.waitTime[1]);
// KeyInput setKey(IN_6);
KeyInput cylinder(IN_3, "", 1, memory_data.waitTime[2], 1);
bool setFlag = false;

typedef struct {
  uint8_t start            = 0;
  uint8_t end              = 0;
  IntervalTimer timeup[10] = {IntervalTimer(memory_data.waitTime[0]), IntervalTimer(memory_data.waitTime[0]), IntervalTimer(memory_data.waitTime[0]), IntervalTimer(memory_data.waitTime[0]), IntervalTimer(memory_data.waitTime[0]),
                              IntervalTimer(memory_data.waitTime[0]), IntervalTimer(memory_data.waitTime[0]), IntervalTimer(memory_data.waitTime[0]), IntervalTimer(memory_data.waitTime[0]), IntervalTimer(memory_data.waitTime[0])};
  bool Buff[10]            = {false};
} FlagData;
FlagData flagData;

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
  pinMode(IN_6, INPUT);
  Serial.begin(115200);
  delay(1000);
  Serial.println("Start");
  init_EEPROM();
  // SENSOR_OUT(1);
  // put your setup code here, to run once:
}

void loop() {
  cameraJudgeMain();
  cylinderMain();
  timeUpFlag();
  cylinderTimeOutMain();
  if (SET_BOTTOM) {
    CYLINDER_OUT(1);
    cylinderTimer.setTime(memory_data.waitTime[1]);
  }
}

void timeUpFlag() {
  if (getFlagDataCount()) {
    if (flagData.timeup[flagData.end].isWait()) {
      Serial.println("flagData.end" + String(flagData.end));
      Serial.println("TimeUp");
      flagData.end++;
      if (flagData.end >= (sizeof(flagData.Buff) / sizeof(flagData.Buff[0]))) {
        flagData.end = 0;
      }
    }
  }
}

void cameraJudgeMain() {
  static uint8_t sq    = 0;
  static uint8_t oldSq = 0;
  static IntervalTimer timer(100);
  bool flag = false;
  switch (sq) {
    case 0:  // カメラセンサーからの信号があったら
      if (CAMERA_JUDGE_OK || CAMERA_JUDGE_NG) {
        sq++;
      }
      break;

    case 1:  // カメラジャッジ開始
      if (CAMERA_JUDGE_OK) {
        flag = true;
      } else if (CAMERA_JUDGE_NG) {
        flag = false;
      }
      flagData.Buff[flagData.start] = flag;  // カメラジャッジ結果をバッファに格納
      flagData.timeup[flagData.start].setTime(memory_data.waitTime[0]);
      Serial.println("flagData.start" + String(flagData.start));
      flagData.start++;
      if (flagData.start >= (sizeof(flagData.Buff) / sizeof(flagData.Buff[0]))) {
        flagData.start = 0;
      }
      if (flagData.start == flagData.end) {
        Serial.println("DifferenceCount Warning");
      }
      sq++;
      break;

    case 2:  // 信号がなくなるまで待機
      if (!CAMERA_JUDGE_OK && !CAMERA_JUDGE_NG) {
        sq = 0;
      }
      break;

    default:
      sq = 0;
      // CAMERA_OUT(0);
      break;
  }

  // if (sq != oldSq) {
  //   Serial.println("cameraJudgeMain" + String(sq));
  //   oldSq = sq;
  // }
}

void cylinderMain() {
  static uint8_t sq    = 0;
  static uint8_t oldSq = 0;
  static IntervalTimer timer(100);
  switch (sq) {
    case 0:
      if (cylinder.isRelease()) {
        cylinder.setLongPushTime(memory_data.waitTime[2]);
        sq++;
      }
      break;
    case 1:
      if (cylinder.isLongPush()) {
        if (getFlagDataCount()) {
          if (!flagData.Buff[flagData.end]) {  // カメラジャッジ結果をバッファから取り出し,NGの場合は次へ
            sq++;
          } else {
            sq = 0;
          }
          flagData.end++;  // 次のカメラジャッジ結果を取り出すためにカウント
          if (flagData.end >= (sizeof(flagData.Buff) / sizeof(flagData.Buff[0]))) {
            flagData.end = 0;
          }
        } else {
          Serial.println("NoData");
          sq++;
        }
      }

      break;

    case 2:  // シリンダーで押し出し
      CYLINDER_OUT(1);
      cylinderTimer.setTime(memory_data.waitTime[1]);
      // timer.setTime(memory_data.waitTime[2]);
      // cylinder.setReleaseTime(memory_data.waitTime[1]);
      sq = 0;
      break;

    default:
      sq = 0;
      CYLINDER_OUT(0);
      break;
  }

  // if (sq != oldSq) {
  //   Serial.println("cylinderMain" + String(sq));
  //   oldSq = sq;
  // }
}

// シリンダー出力処理
void cylinderTimeOutMain() {
  if (cylinderTimer.isWait()) {
    CYLINDER_OUT(0);
  }
}
// FlagDataのカウント数を取得
uint8_t getFlagDataCount() {
  uint8_t size = sizeof(flagData.Buff) / sizeof(flagData.Buff[0]);
  if (flagData.start >= flagData.end) {
    return flagData.start - flagData.end;
  } else {
    return size - (flagData.end - flagData.start);
  }
}
//