/**
 *
 *	@file		eeprom.cpp
 *
 *	@brief		eeprom処理
 *
 *	@author		株式会社エムスクエア・ラボ　山形
 *
 *	@date		create : 2024/01/15
 *
 */
#include <Arduino.h>
#include <EEPROM.h>
#include "setingWeb.hpp"
#include <CRC32.h>
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
/*==========================================================================================
	内部変数定義
==========================================================================================*/

/*==========================================================================================
	関数
==========================================================================================*/
/**
 *	@brief		EEPROM初期設定
 *	@param		なし
 *	@retval		なし
 */
void eepromInit(void){
	uint32_t crcData;
	CRC32 crc;
	EEPROM.begin(sizeof(config)+sizeof(crcData));

	EEPROM.get(0,config);//設定データ取得
	EEPROM.get(sizeof(config),crcData);//CRCデータ取得
	Serial.print( "config.wifi.ssid: ");
	Serial.println(config.wifi.ssid);
	Serial.print( "config.wifi.ps: ");
	Serial.println(config.wifi.ps);
	Serial.print( "config.gas.devId: ");
	Serial.println(config.gas.devId);
	Serial.print( "config.nfcType: ");
	Serial.println(config.nfcType);
	Serial.print( "config.flag: ");
	Serial.println(config.flag);
	Serial.print( "config.pallet_n: ");
	Serial.println(config.pallet_n);
	Serial.print( "config.palletWeight: ");
	Serial.println(config.palletWeight);
	Serial.print( "config.version: ");
	Serial.println(config.version);
	Serial.print( "CRC: ");
	Serial.println(crcData);
	
	crc.update((uint8_t *)&config, sizeof(config));	//CRCデータ生成
	//CRC不一致または初期状態の場合は初期設定を書き込む
	if((config.flag == eINITIAL)||(crc.finalize() != crcData)){	
		strcpy(config.version, "0.0.1\n");
		Serial.println("configReset");
		strcpy(config.wifi.ssid, "SSID");
		strcpy(config.wifi.ps  , "PS");
		strcpy(config.gas.devId, "GASID");
		config.nfcType = eUSER;
		config.palletWeight = 0;
		config.pallet_n = 0;
		config.flag = eSETING;
		eepromSave();
	}
}

/**
 *	@brief		EEPROM保存処理
 *	@param		なし
 *	@retval		なし
 */
void eepromSave(void){
	CRC32 crc;
	Serial.println("Serv");
	crc.update((uint8_t *)&config, sizeof(config));	//CRCデータ生成
	EEPROM.put(0, config);	//設定データ保存
	EEPROM.put(sizeof(config), crc.finalize());	//CRCデータを設定データの後に保存
	EEPROM.commit();
}