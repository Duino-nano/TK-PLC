/**
 *
 *	@file		key.cpp
 *
 *	@brief		キー入力処理
 *
 *	@date		create : 2024/01/16
 *
 */
#include <Arduino.h>
#include "timer.hpp"
#include "key.hpp"
/*==========================================================================================
        外部関数定義
==========================================================================================*/

/*==========================================================================================
        内部定数定義
==========================================================================================*/

/*==========================================================================================
        内部関数定義
==========================================================================================*/
/*==========================================================================================
        外部変数定義
==========================================================================================*/
/*==========================================================================================
        内部変数定義
==========================================================================================*/
/**
 *	@var		ssPins
 *	@brief		RFIDチャンネル
 */

/**
 *	@var		mfrc522
 *	@brief		各チャンネルインスタンス
 */

/*==========================================================================================
        関数
==========================================================================================*/

/**
 *	@brief		インタラクタ
 *	@param		t:設定時間
 *	@retval		なし
 */
KeyInput::KeyInput(int pin, const char* n, unsigned long pushTime, unsigned long longPushTime, unsigned long releaseTime) {
  char setName[32];
  keyPin      = pin;
  name        = n;
  releaseFlag = false;
  pinMode(keyPin, INPUT_PULLUP);
  // 名称作成
  strcpy(setName, "push:");
  strcat(setName, name);
  pushTimer = new IntervalTimer(pushTime, setName);
  strcpy(setName, "longPush:");
  strcat(setName, name);
  longPushTimer = new IntervalTimer(longPushTime, setName);
  strcpy(setName, "release:");
  strcat(setName, name);
  releaseTimer = new IntervalTimer(releaseTime, setName);
}

/**
 *	@brief		デスイントラクタ
 *	@param		なし
 *	@retval		なし
 */
KeyInput::~KeyInput(void) {
  delete pushTimer;
  delete longPushTimer;
  delete releaseTimer;
}

/**
 *	@brief		押下時間経過確認
 *	@param		なし
 *	@retval		0：時間未経過　1：時間経過
 */
bool KeyInput::isPush(void) {
  bool result = false;
  if (!digitalRead(keyPin)) {
    releaseTimer->init();
    releaseFlag = false;
    if (pushTimer->isWait()) {
      if (toggle == 0) {
        toggle = 1;
      }
    }
  } else {
    if (toggle == 1) {
      result = true;
    }
    toggle = 0;
    pushTimer->init();
    longPushTimer->init();
  }
  return result;
}

/**
 *	@brief		長押し時間経過確認
 *	@param		なし
 *	@retval		0：時間未経過　1：時間経過
 */
bool KeyInput::isLongPush(void) {
  bool result = false;
  if (!digitalRead(keyPin)) {
    releaseTimer->init();
    releaseFlag = false;
    if (longPushTimer->isWait() || toggle == 2) {
      if (toggle <= 2) {
        toggle = 2;
        result = true;
        pushTimer->init();
        longPushTimer->init();
      }
    }
  } else {
    if (toggle >= 2) {
      Serial.println("Longinit");
      toggle = 0;
    }

    pushTimer->init();
    longPushTimer->init();
  }
  return result;
}

/**
 *	@brief		離し確認
 *	@param		なし
 *	@retval		0：押下中　1：離し
 */
bool KeyInput::isRelease(void) {
  if (digitalRead(keyPin)) {
    if (releaseTimer->isWait()) {
      releaseFlag = true;
    }
  } else {
    releaseTimer->init();
    releaseFlag = false;
  }

  return releaseFlag;
}

/**
 *	@brief		押下時間設定
 *	@param		t:設定時間
 *	@retval		なし
 */
void KeyInput::setPushTime(unsigned long t) {
  pushTimer->setTime(t);
}

/**
 *	@brief		長押し時間設定
 *	@param		t:設定時間
 *	@retval		なし
 */
void KeyInput::setLongPushTime(unsigned long t) {
  longPushTimer->setTime(t);
}

/**
 *	@brief		離し時間設定
 *	@param		t:設定時間
 *	@retval		なし
 */

void KeyInput::setReleaseTime(unsigned long t) {
  releaseTimer->setTime(t);
  releaseFlag = false;
}

/**
 *	@brief		初期化
 *	@param		t:設定時間
 *	@retval		なし
 */
void KeyInput::init(void) {
  pushTimer->init();
  longPushTimer->init();
  releaseTimer->init();
  releaseFlag = false;
  if(toggle == 2) {
    Serial.println("init");
    toggle = 3;
  }
}

/**
 * @brief		名称登録
 * @param		name:名称
 * @retval		なし
 */
void KeyInput::setName(const char* n) {
  name = n;
}