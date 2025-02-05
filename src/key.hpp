/**
 *
 *	@file		key.hpp
 *
 *	@brief		キー入力処理
 *
 *	@author		山形
 *
 *	@date		create : 2024/01/16
 *
 */
#ifndef  __KEY_HPP__
#define __KEY_HPP__
#include "timer.hpp"
/*==========================================================================================
	定数定義
==========================================================================================*/


/*==========================================================================================
	変数定義
==========================================================================================*/
/**
 * @struct	
 * @brief	
 */
/**
 * @class 	KeyInput
 * @brief	キー入力処理
 */
class KeyInput{
	private:
		int keyPin;
		int toggle;
		IntervalTimer *pushTimer;
		IntervalTimer *longPushTimer;

	public:
		KeyInput(int, unsigned long pushTime = 50, unsigned long longPushTime = 1000);    //インストラクタ
		~KeyInput(void);				//デスインタラクタ
		int isPush(void);      	//キー入力確認
		int isLongPush(void);      	//長押し入力確認
		void setTime(unsigned long);     	//更新時間設定
		void init(void);     	//データ初期化
};
/*==========================================================================================
	関数定義
==========================================================================================*/


#endif