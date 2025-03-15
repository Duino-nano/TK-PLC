/**
 *
 *	@file		key.hpp
 *
 *	@brief		キー入力処理
 *
 *	@date		create : 2024/01/16
 *
 */
#ifndef __KEY_HPP__
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
class KeyInput {
private:
  int keyPin;
  int toggle;
  IntervalTimer *pushTimer;
  IntervalTimer *longPushTimer;
  IntervalTimer *releaseTimer;
  bool releaseFlag = false;
  const char *name = NULL;

public:
  KeyInput(int, const char *n = "key", unsigned long pushTime = 10, unsigned long longPushTime = 1000, unsigned long releaseTime = 10);  // インストラクタ
  ~KeyInput(void);                                                                                                                       // デスインタラクタ
  bool isPush(void);                                                                                                                     // キー入力確認
  bool isLongPush(void);                                                                                                                 // 長押し入力確認
  bool isRelease(void);                                                                                                                  // 離し確認
  void setPushTime(unsigned long);                                                                                                       // 押下時間設定
  void setLongPushTime(unsigned long);                                                                                                   // 長押し時間設定
  void setReleaseTime(unsigned long);                                                                                                    // 離し時間設定
  void init(void);                                                                                                                       // データ初期化
  void setName(const char *);                                                                                                            // 名称登録
};
/*==========================================================================================
        関数定義
==========================================================================================*/

#endif