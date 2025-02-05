/**
 *
 *	@file		key.cpp
 *
 *	@brief		キー入力処理
 *
 *	@author		山形
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
KeyInput::KeyInput(int pin, unsigned long pushTime, unsigned long longPushTime){
	keyPin = pin;
	pinMode(keyPin, INPUT);
	pushTimer = new IntervalTimer(pushTime);
	longPushTimer = new IntervalTimer(longPushTime);
}

/**
 *	@brief		デスイントラクタ
 *	@param		なし
 *	@retval		なし
 */
KeyInput::~KeyInput(void){
	delete pushTimer;
	delete longPushTimer;
}

/**
 *	@brief		押下時間経過確認
 *	@param		なし
 *	@retval		0：時間未経過　1：時間経過
 */
int KeyInput::isPush(void){
	int result = 0;
	if(!digitalRead(keyPin)){
		if(pushTimer->isWait()){
			toggle = 1;
		}
	}else{
		if(toggle){
			toggle = 0;
			result = 1;
		}
		pushTimer->init();
	}
	return result;
}

/**
 *	@brief		長押し時間経過確認
 *	@param		なし
 *	@retval		0：時間未経過　1：時間経過
 */
int KeyInput::isLongPush(void){
	int result = 0;
	if(!digitalRead(keyPin)){
		if(longPushTimer->isWait()){
			toggle = 0;
			result = 1;
		}
	}else{
		longPushTimer->init();
	}
	return result;
}

/**
 *	@brief		時間設定
 *	@param		t:設定時間
 *	@retval		なし
 */
void KeyInput::setTime(unsigned long t){
	pushTimer->setTime(t);
}

/**
 *	@brief		初期化
 *	@param		t:設定時間
 *	@retval		なし
 */
void KeyInput::init(void){
	pushTimer->init();
}