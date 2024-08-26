#include <Arduino.h>
#include "timer.hpp"
#include "setWeb.hpp"
#include "eeprom.hpp"
#include "key.hpp"
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
#define SET_BOTTOM      (!digitalRead(IN_6))
#define HOPPER_IN           (!digitalRead(IN_2))
// #define NG_IN           digitalRead(IN_3)
#define BELT_OUT(x)     digitalWrite(OUT_1,x)
#define LED_OUT(x)      digitalWrite(OUT_3,x)
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
  LED_OUT(setWebMain(setKey));
  switch(sq){
    case 0:
      if((SENSOR_IN)&&(HOPPER_IN)){
        timer.setTime(memory_data.waitTime[0]);
        sq++;
      }
      BELT_OUT(0);
    break;

    case 1:
      if(timer.isWait()){
        BELT_OUT(1);
        sq++;
      }
    break;

    case 2:
      if((!SENSOR_IN)||(!HOPPER_IN)){
        timer.setTime(memory_data.waitTime[1]);
        sq++;
      }
      BELT_OUT(1);
    break;

    case 3:
      if(timer.isWait()){
        BELT_OUT(0);
        sq = 0;
      }
    break;

    default:
    break;
  }
  
}

