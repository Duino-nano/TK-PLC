#include <Arduino.h>
#include "timer.hpp"
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
#define CYLINDER_SENSOR_IN (!digitalRead(IN_3))
#define SET_BOTTOM (!digitalRead(IN_6))
// #define NG_IN           digitalRead(IN_3)
#define CAMERA_OUT(x) digitalWrite(OUT_1, x)
#define CYLINDER_OUT(x) digitalWrite(OUT_2, x)
#define LED_OUT(x) digitalWrite(OUT_5, x)
// #define ALERT_OUT(x)    digitalWrite(OUT_2,x)
// #define SENSOR_OUT(x)    digitalWrite(OUT_4,x)
// #define Pushe_OUT(x)    digitalWrite(OUT_5,x)
void cameraJudgeMain();
void cylinderMain();
IntervalTimer timer(memory_data.waitTime[0]);
KeyInput setKey(IN_6, 100, 1000);
bool setFlag = false;

typedef struct {
  uint8_t cameraCount = 0;
  int differenceCount = 0;
  bool Buff[5]        = {false};
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
  Serial.begin(9600);
  init_EEPROM();
  // SENSOR_OUT(1);
  // put your setup code here, to run once:
}

void loop() {
  cameraJudgeMain();
  cylinderMain();
}

void cameraJudgeMain() {
  static int sq = 0;
  static IntervalTimer timer(100);
  bool flag = false;
  switch (sq) {
    case 0:  // カメラセンサーが検知されたら
      if (CAMERA_SENSOR_IN) {
        sq++;
        timer.setTime(memory_data.waitTime[0]);
      }
      break;

    case 1:  // カメラジャッジ開始
      CAMERA_OUT(1);
      if (timer.isWait()) {
        CAMERA_OUT(0);
        timer.setTime(memory_data.waitTime[1]);
        sq++;
      }

      break;

    case 2:  // カメラNGの場合は
      flag = CAMERA_JUDGE_IN;
      if (flag || timer.isWait()) {                  // 　カメラジャッジまたは時間経過で次へ
        flagData.Buff[flagData.cameraCount] = flag;  // カメラジャッジ結果をバッファに格納
        flagData.cameraCount++;
        flagData.differenceCount++;
        if (flagData.cameraCount >= sizeof(flagData.Buff)) {
          flagData.cameraCount = 0;
        }
        if (flagData.differenceCount >= sizeof(flagData.Buff)) {
          Serial.println("DifferenceCount Warning");
        }
        sq = 0;
      }
      break;

    default:
      sq = 0;
      CAMERA_OUT(0);
      break;
  }
}

void cylinderMain() {
  static int sq = 0;
  static IntervalTimer timer(100);
  switch (sq) {
    case 0:  // カメラセンサーが検知されたら
      if (CYLINDER_SENSOR_IN) {
        if (flagData.Buff[(flagData.cameraCount - ((flagData.differenceCount) % sizeof(flagData.Buff)))]) {  // カメラジャッジ結果をバッファから取り出し,NGの場合は次へ
          sq++;
        }
        flagData.differenceCount--;  // 次のカメラジャッジ結果を取り出すためにカウントを減らす
        if (flagData.differenceCount < 0) {
          flagData.differenceCount = 0;
          Serial.println("DifferenceCount Error");
        }
      }
      break;

    case 1:  // シリンダーで押し出し
      CYLINDER_OUT(1);
      timer.setTime(memory_data.waitTime[2]);
      sq++;
      break;

    case 2:  // シリンダーが動作するまで待機
      if (timer.isWait()) {
        sq = 0;
        timer.init();
      }
      break;

    default:
      sq = 0;
      CYLINDER_OUT(0);
      break;
  }
}