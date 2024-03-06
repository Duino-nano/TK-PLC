/**
 *
 *	@file		timer.cpp
 *
 *	@brief		ターマー処理
 *
 *	@date		create : 2023/12/25
 *
 */
#include <Arduino.h>
#include "timer.hpp"
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
IntervalTimer::IntervalTimer(unsigned long t){
    setTime(t);
}

/**
 *	@brief		時間経過確認
 *	@param		なし
 *	@retval		0：時間未経過　1：時間経過
 */
int IntervalTimer::isWait(void){
    if((millis() - backTime) > waitTime){
        backTime = millis();
        return 1;
    }
    return 0;
}

/**
 *	@brief		時間設定
 *	@param		t:設定時間
 *	@retval		なし
 */
void IntervalTimer::setTime(unsigned long t){
    waitTime = t;
    backTime = millis();
}

/**
 *	@brief		初期化
 *	@param		t:設定時間
 *	@retval		なし
 */
void IntervalTimer::init(void){
    backTime = millis();
}