#include <Arduino.h>
#include "timer.hpp"
#define OUT_1   4
#define OUT_2   16
#define OUT_3   17
#define OUT_4   18
#define OUT_5   19

#define IN_1    26
#define IN_2    25
#define IN_3    33
#define IN_4    32
#define IN_5    35
#define IN_6    34

#define SENSOR_IN       (!digitalRead(IN_1))

#define OUT1(x)     digitalWrite(OUT_1,x)


/************************************************
 * シーケンスNo
 * 
 ************************************************/
typedef enum {
  eOUTPUT_LOW,
  eOUTPUT_HIGH,
}eMAIN_SQ;


/************************************************
 * セットアップ
 * 
 ************************************************/
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
  Serial.begin(9600);

}

/************************************************
 * メイン処理
 * 
 ************************************************/
void loop() {
  static eMAIN_SQ sq = eOUTPUT_LOW;
  IntervalTimer timer(500);
  if(SENSOR_IN){ //入力を確認
    switch (sq){
      /*消灯処理*/
      case  eOUTPUT_LOW:
        OUT1(LOW);
        if(timer.isWait()){ //一定時間経過後点灯
          timer.init();
          sq = eOUTPUT_HIGH;
        }
      break;

      /*点灯処理*/
      case eOUTPUT_HIGH:
        OUT1(HIGH);
        if(timer.isWait()){ //一定時間経過後消灯
          timer.init();
          sq = eOUTPUT_LOW;
        }
      break;
    default:
        sq = eOUTPUT_LOW;
      break;
    }
  }else{
    //入力があるまで待機
    sq = eOUTPUT_LOW;
    timer.init();
  }
}