/**
 *
 *	@file		eeprom.cpp
 *
 *	@brief		EEPROM処理
 *
 *	@author		山形
 *
 *	@date		create : 2024/03/16
 *
 */
#include <Arduino.h>
#include <EEPROM.h>
#include <CRC32.h>
#include "eeprom.hpp"
/*==========================================================================================
        外部関数定義
==========================================================================================*/

/*==========================================================================================
        内部定数定義
==========================================================================================*/

/*==========================================================================================
        内部関数定義
==========================================================================================*/
void eepromSave(void);
/*==========================================================================================
        外部変数定義
==========================================================================================*/
MEMORY_DATA memory_data;
MEMORY_DATA memory_data_buckup;
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
 *	@brief		EEPROM初期設定
 *	@param		なし
 *	@retval		なし
 */
void init_EEPROM() {
  CRC32 crc;
  uint32_t crcData;
  EEPROM.begin(sizeof(memory_data) + sizeof(crcData));
  EEPROM.get(0, memory_data);
  EEPROM.get(sizeof(memory_data), crcData);
  crc.update((uint8_t *)&memory_data, sizeof(memory_data));
  Serial.println("EEPROM");

  Serial.println();
  Serial.println(memory_data.waitTime[0]);
  Serial.println(memory_data.waitTime[1]);
  Serial.println(memory_data.waitTime[2]);

  Serial.println(crcData);
  Serial.println(crc.finalize());
  // データの整合性を確認
  if (crc.finalize() != crcData) {
    // 保存データが無い場合デフォルトを設定
    memory_data.waitTime[0] = 1000;   // <- データ保存時間
    memory_data.waitTime[1] = 50;    // <- エアー出力時間下限
    memory_data.waitTime[2] = 1000;  // <- エアーの出力時間上限
    eepromSave();
  }
}

/**
 *	@brief		EEPROM保存処理
 *	@param		なし
 *	@retval		なし
 */
void eepromSave(void) {
  CRC32 crc;
  if (memcmp(&memory_data, &memory_data_buckup, sizeof(memory_data))) {  // 設定データが変更された場合
    Serial.print("Serv");
    crc.update((uint8_t *)&memory_data, sizeof(memory_data));  // CRCデータ生成
    EEPROM.put(0, memory_data);                                // 設定データ保存
    EEPROM.put(sizeof(memory_data), crc.finalize());           // CRCデータを設定データの後に保存
    EEPROM.commit();
    memcpy(&memory_data_buckup, &memory_data, sizeof(memory_data));
  }
}