#include <Arduino.h>
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

#define SENSOR_IN       digitalRead(IN_1)
#define OK_IN           digitalRead(IN_2)
#define NG_IN           digitalRead(IN_3)
#define BELT_OUT(x)     digitalWrite(OUT_1,x)
#define ALERT_OUT(x)    digitalWrite(OUT_2,x)
#define SENSOR_OUT(x)    digitalWrite(OUT_4,x)
#define Pushe_OUT(x)    digitalWrite(OUT_5,x)

#define STRING_N  15
// %YES%
// %NG%
/************************************************
 * シーケンスNo
 * 
 ************************************************/
typedef enum {
  BELT_START,
  SENSOR_WAIT,
  SENSOR_HIT,
  BELT_STOP,
  BELT_STOP_WAIT,
  CAMERA_JUDGE,
  CAMERA_JUDGE_WAIT,
  ALERT,
  PUSHER_TRAVEL,
  PUSHER_PLL,
  PUSHER_PUSH,
}eMAIN_SQ;

typedef enum{
  CAM_NULL,
  CAM_OK,
  CAM_NG,
}eCAM_LOG;
// %YES%

const char cameraOK[STRING_N] = "%YES%\n";
const char cameraNG[STRING_N]  = "%NO%\n";

int sec_timer(unsigned long time, eMAIN_SQ cq);
eCAM_LOG cameraCheck(void);
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

  SENSOR_OUT(1);
  // put your setup code here, to run once:
}

void loop() {
  
  static eMAIN_SQ sq = BELT_START;
  eCAM_LOG dataBuf = CAM_NULL;
  static int ledBuf = 0;
  switch (sq)
  {
    /*ベルトコンベア動作開始*/
  case BELT_START:
    BELT_OUT(1);    
    if(sec_timer(1000,BELT_START)){  //一秒経過後次の処理へ
      sq = SENSOR_WAIT;
    }
    break;

    /*通過センサー監視*/
  case SENSOR_WAIT:
    if(!SENSOR_IN){
      sq = SENSOR_HIT;
    }
    break;

  /*通過センサー監視*/
  case SENSOR_HIT:
    // if(sec_timer(200,SENSOR_HIT)){  //一秒経過後次の処理へ
    //   sq = BELT_STOP;
    // }
    sq = BELT_STOP;

    break;

  /*ベルト停止*/
  case BELT_STOP:
    BELT_OUT(0);    
    if(sec_timer(1000,BELT_STOP)){  //一秒経過後次の処理へ
    
      sq = CAMERA_JUDGE;
    }
    
    break;

  /*カメラ判定開始*/
  case CAMERA_JUDGE:
    Serial.println("%takepic%");
    sq = CAMERA_JUDGE_WAIT;
    break;

  /*カメラ判定*/
  case CAMERA_JUDGE_WAIT:
    dataBuf = cameraCheck();
    if((dataBuf == CAM_OK)||(!OK_IN)){    //カメラOKの場合はベルト再スタート
      sq = BELT_START;        
    }else if((dataBuf == CAM_NG)||(!NG_IN)){  //NGの場合はプッシャーで押し出し
      // sq = ALERT; //プッシャー使用しない場合
      sq = PUSHER_TRAVEL; //プッシャー処理へ
    }

    if(sec_timer(10000,CAMERA_JUDGE_WAIT)){  //10秒待機しても結果が帰って来ない場合はアラーム
      sq = ALERT;
    }
    break;

  /*カメラ判定*/
  case PUSHER_TRAVEL:
    BELT_OUT(1);    
    if(sec_timer(1200,PUSHER_TRAVEL)){ //プッシャーまで移動させる
      BELT_OUT(0);
      sq = PUSHER_PUSH;
    }
  break;

  /*プッシャー押出*/
  case PUSHER_PUSH:
    Pushe_OUT(1);
    if(sec_timer(1000,PUSHER_PUSH)){  //押出まで待機
      sq = PUSHER_PLL;
    }
  break;

  /*プッシャー引き戻し*/
  case PUSHER_PLL:
    Pushe_OUT(0);
    if(sec_timer(1000,PUSHER_PLL)){ //引き戻しまで待機
      sq = BELT_START;
    }
  break;

  /*アラート処理*/
  case ALERT:
    if(!SENSOR_IN){   //センサーに検知している場合はアラートを出す。
      if(sec_timer(200,ALERT)){
        sec_timer(0,ALERT);
        ledBuf = 1-ledBuf;
        ALERT_OUT(ledBuf);
      }
    }else{      //検体が取り除かれたら再開
      ALERT_OUT(0);
      Pushe_OUT(0);
      sq = BELT_START;
    }
    
    break;
  
  default:
    break;
  }
  
}


/********************************************************************************
 * @brief   1秒カウントタイマー
 * @param   time 0:リセット, 1~:ms経過時間, mainSequence:変更時にリセット
 * @return  0:通常　1:時間経過
 * ******************************************************************************/
int sec_timer(unsigned long time, eMAIN_SQ cq){
    static eMAIN_SQ backCq = BELT_START;
    static ulong time_buffer = 0;
    if(cq == backCq){
      if((millis()-time_buffer) >= time){   //一秒間のカウント
        time_buffer = millis();
        return 1;   
      }
    }else{
      time_buffer = millis();
      backCq = cq;
    }
  
  return 0;
}

/**********************************************************
 * @brief		  カメラ返答内容確認
 * @param     なし
 * @return    eCAM_LOG	
 **********************************************************/
eCAM_LOG cameraCheck(void){
  char data[STRING_N];

  if (Serial.available() > 0 ) {
    // シリアルデータの受信 (改行まで)
    for(int i=0; i<STRING_N; i++){
      data[i] = Serial.read();     //一文字取得   
      if(data[i] == 255){
        data[i] = 0;
      }
    }
    // Serial.println((data));
    if(data[0] == cameraNG[0]){
      for(int i=1; i<STRING_N; i++){
        // Serial.println(int(data[i]));
        // Serial.println(int(cameraOK[i]));
        // Serial.println(".");
        if(data[i] != cameraNG[i]){
          break;
        }else{
          if(data[i] == '\n'){
            // Serial.println("NG");
            return CAM_NG;
          }
        }
      }
    }
    if(data[0] == cameraOK[0]){
      for(int i=1; i<STRING_N; i++){
        // Serial.println(int(data[i]));
        // Serial.println(int(cameraOK[i]));
        // Serial.println(".");
        if(data[i] != cameraOK[i]){
          break;
        }else{
          if(data[i] == '\n'){
            // Serial.println("OK");
            return CAM_OK;
          }
        }
      }
    }
    
  }
  return CAM_NULL;
}