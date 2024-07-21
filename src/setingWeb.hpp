/**
 *
 *	@file		setingWeb.cpp
 *
 *	@brief		設定用webアプリ
 *
 *	@author		株式会社エムスクエア・ラボ　山形
 *
 *	@date		create : 2023/11/09
 *
 */
#ifndef  __SETINGWEBHPP__
#define __SETINGWEBHPP__

#include <ESPAsyncWebServer.h>
#include "key.hpp"



/*==========================================================================================
	定数定義
==========================================================================================*/
#define PALLET_n 99


/*==========================================================================================
	変数定義
==========================================================================================*/
/**
 * @enum	NfcType
 * @brief	設定状態フラグ
 */
typedef enum nfctype{ 
    eUSER,      //0:ユーザーIDで登録
    eFARM,      //1:農場IDで登録
    eBOTH       //2:ユーザー,IDで登録
}NfcType;

/**
 * @enum	Setflag
 * @brief	設定状態フラグ
 */
typedef enum setflag{ 
    eINITIAL = -1,
    eSETING,
    eCOMPLETE
}Setflag;

/**
 * @struct	GasConfig
 * @brief	GAS設定データ
 */
typedef struct gasconfig{
    char devId[128];
}GasConfig;

/**
 * @struct	WifiConfig
 * @brief	wifi設定データ
 */
typedef struct wificonfig{ 
    char ssid[128];
    char ps[128];
}WifiConfig;

/**
 * @struct	ConfigData
 * @brief	設定データ
 */
typedef struct configdata{
    GasConfig gas;
    WifiConfig wifi;
    NfcType nfcType;
    float palletWeight;
    int pallet_n;
    Setflag flag = eINITIAL;
    char version[16];
}ConfigData;



extern ConfigData config;
/*==========================================================================================
	関数定義
==========================================================================================*/
extern void startWebserver();
extern void setWifiMode();
extern void webserverEnable(bool en);
#endif