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
#include <Arduino.h>
#include <ESPmDNS.h>
#include "measurelinks.hpp"
#include <ESPAsyncWebServer.h>
#include <FS.h>
#include "SPIFFS.h"
#include <ArduinoJson.h>
#include "setingWeb.hpp"
#include <stdio.h>
#include "eeprom.hpp"
#include "display.hpp"
#include "key.hpp"
#include "gas.hpp"
#include "fg_rs232c.hpp"
#include "firmwareUpdate.hpp"
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
bool setWebserverFlag = false;
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
AsyncWebServer webServer(80);

ConfigData config;

/*==========================================================================================
	関数
==========================================================================================*/
void getData(char *data, AsyncWebServerRequest *request);
void handleRestart(AsyncWebServerRequest *request);
/**
 *	@brief		設定WEBアプリ処理（非同期処理）
 *	@param		なし
 *	@retval		なし
 */
void startWebserver() {
	static bool stertWebServerFlag = false;
	if(!stertWebServerFlag){	//すでにサーバーが立ってる場合は処理しない。
		stertWebServerFlag = true;
		// フラッシュメモリのデータを取得
		if (!SPIFFS.begin(true)) {
			Serial.println("An Error has occurred while mounting SPIFFS");
			return;
		}
		
		// Web server
		webServer.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
			if(setWebserverFlag){
				request->send(SPIFFS, "/index.html", "text/html");
			}else{
      	request->send(503, "text/plain", "Server unavailable"); // WEBサーバー無効の場合はエラー応答
    	}
		});

		// アイコンを渡す
		webServer.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *request){
			if(setWebserverFlag){
				request->send(SPIFFS, "/favicon.ico", "image/x-icon");
			}else{
      	request->send(503, "text/plain", "Server unavailable"); // WEBサーバー無効の場合はエラー応答
    	}		
		});
		
			// CSSファイルを提供
		webServer.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
			if(setWebserverFlag){
				request->send(SPIFFS, "/style.css", "text/css");
			}else{
      	request->send(503, "text/plain", "Server unavailable"); // WEBサーバー無効の場合はエラー応答
    	}
		});

		// JavaScriptファイルを提供
		webServer.on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request){
			if(setWebserverFlag) {
				request->send(SPIFFS, "/script.js", "application/javascript");
			}else{
      	request->send(503, "text/plain", "Server unavailable"); // WEBサーバー無効の場合はエラー応答
    	}
		});

		webServer.on("/getData", HTTP_GET, [](AsyncWebServerRequest *request){
			if(setWebserverFlag){
				DynamicJsonDocument jsonDoc(1024);
				jsonDoc["Ver"] = config.version;
				jsonDoc["ssid"] = config.wifi.ssid;
				// jsonDoc["ps"] = config.wifi.ps;
				jsonDoc["gasId"] = config.gas.devId;
				jsonDoc["palletWeight"] = config.palletWeight;
				//タイプを文字列へ変換
				switch (config.nfcType){
					case eUSER:
						jsonDoc["nfcType"] = "userOnly";
					break;

					case eFARM:
						jsonDoc["nfcType"] = "farmOnly";
					break;

					case eBOTH:
						jsonDoc["nfcType"] = "both";
					break;

					default:
						jsonDoc["nfcType"] = "userOnly";
					break;

				}
				// JSONオブジェクトを文字列に変換
				String jsonResponse;
				serializeJson(jsonDoc, jsonResponse);
				Serial.println(jsonResponse);
				// レスポンスを送信
				request->send(200, "application/json", jsonResponse);
			}else{
      	request->send(503, "text/plain", "Server unavailable"); // WEBサーバー無効の場合はエラー応答
    	}
		});

		//現在の重量を渡す
		webServer.on("/getWeight", HTTP_GET, [](AsyncWebServerRequest *request){
			if(setWebserverFlag){
				DynamicJsonDocument jsonDoc(1024);
				char buffer[10];
				dtostrf(fgScale.data, 6, 2, buffer); 
				jsonDoc["weight"] = buffer;
				jsonDoc["stable"] = fgScale.status;
				// JSONオブジェクトを文字列に変換
				String jsonResponse;
				serializeJson(jsonDoc, jsonResponse);
				// Serial.println(jsonResponse);
				// レスポンスを送信
				request->send(200, "application/json", jsonResponse);
			}else{
      	request->send(503, "text/plain", "Server unavailable"); // WEBサーバー無効の場合はエラー応答
    	}
		});


		webServer.on("/ssidUpdate", HTTP_POST, [](AsyncWebServerRequest *request){
			Serial.print("ssid:");
			getData(config.wifi.ssid, request);
		});

		webServer.on("/psUpdate", HTTP_POST, [](AsyncWebServerRequest *request){
			Serial.print("ps:");
			getData(config.wifi.ps, request);
		});

		webServer.on("/gasIdUpdate", HTTP_POST, [](AsyncWebServerRequest *request){
			Serial.print("gasId:");
			getData(config.gas.devId, request);
		});

		webServer.on("/restart", HTTP_POST, [](AsyncWebServerRequest *request){
			handleRestart(request);
		});

		webServer.on("/nfcTypeUpdate", HTTP_POST, [](AsyncWebServerRequest *request){
			if(setWebserverFlag){
				char type[16];
				Serial.print("nfcType:");
				getData(type, request);
				//文字列からフラグに変換
				if(!strcmp(type,"userOnly")){
					config.nfcType = eUSER;
				}else if(!strcmp(type,"farmOnly")){
					config.nfcType = eFARM;
				}else if(!strcmp(type,"both")){
					config.nfcType = eBOTH;
				}
				request->send(200);
				eepromSave();
			}else{
      	request->send(503, "text/plain", "Server unavailable"); // WEBサーバー無効の場合はエラー応答
    	}
		});

		webServer.on("/setWeight", HTTP_POST, [](AsyncWebServerRequest *request){
			if(setWebserverFlag){
				Serial.print("palletWeight:");
				config.palletWeight = request->arg("newData").toFloat();
				Serial.println(config.palletWeight);
				request->send(200);
				eepromSave();
			}else{
      	request->send(503, "text/plain", "Server unavailable"); // WEBサーバー無効の場合はエラー応答
    	}
		});

		webServer.on("/checkFirmware", HTTP_GET, [](AsyncWebServerRequest *request){
			if(setWebserverFlag){
				request->send(200, "text/plain", String(checkAndUpdateFirmware()));
			}else{
      	request->send(503, "text/plain", "Server unavailable"); // WEBサーバー無効の場合はエラー応答
    	}
		});

		webServer.on("/firmwareUpdate", HTTP_GET, [](AsyncWebServerRequest *request){
			// ファームウェア更新タスクを開始
			// xTaskCreate(firmwareUpdate, "FirmwareUpdateTask", 8192, NULL, 0, NULL);
			if(setWebserverFlag){
				updateFlag = 1;
				request->send(200, "text/plain", "Firmware update started.");
			}else{
      	request->send(503, "text/plain", "Server unavailable"); // WEBサーバー無効の場合はエラー応答
    	}
		});

		webServer.begin();
	}
}

/**
 *	@brief		設定サーバー動作切替
 *	@param		なし
 *	@retval		なし
 */
void webserverEnable(bool en){
	setWebserverFlag = en;
}

/**
 *	@brief		データ取得処理
 *	@param		data:書き込み先アドレス、request:rリクエストデータアドレス
 *	@retval		なし
 */
void getData(char *data, AsyncWebServerRequest *request){
	if(setWebserverFlag){
		String newData = request->arg("newData");
		strcpy(data, newData.c_str());
		Serial.println(data);
		request->send(200);
		eepromSave();
	}else{
		request->send(503, "text/plain", "Server unavailable"); // WEBサーバー無効の場合はエラー応答
	}
}



/**
 *	@brief		再起動処理
 *	@param		request:rリクエストデータアドレス
 *	@retval		なし
 */
void handleRestart(AsyncWebServerRequest *request) {
	if(setWebserverFlag){
		Serial.print("restart");
		config.flag = eCOMPLETE;
		eepromSave();
		request->send(200);
		endLodingDisp();
		setStrDisp("    ");
		delay(1000);
		ESP.restart();   
	}else{
		request->send(503, "text/plain", "Server unavailable"); // WEBサーバー無効の場合はエラー応答
	}
}

/**
 *	@brief		設定モード移管処理
 *	@param		なし
 *	@retval		なし
 */
void setWifiMode(){
	char led;
	if(updateFlag){
		firmwareUpdate();
		Serial.println("updateEND");
		updateFlag = 0;
	}
	//設定ボタン長押しの場合
	if(setKey.isLongPush()){
		setStrDisp("SET");
		startLodingDisp(1, 100);
		setLed(0b0000);	//LED消灯
		webserverEnable(true);
		WiFi.disconnect();
		delay(500);
		// Start SPIFFS
		// Set up Access Point
		WiFi.softAP(PRODUCT, "m2-labo00");
		Serial.println(WiFi.softAPIP());
		if (!MDNS.begin(PRODUCT)) {
			Serial.println("Error setting up MDNS responder!");
		}
		delay(100);
		startWebserver();
		scaleMain();	//重量データ取得処理
		while(1){
			scaleMain();	//重量データ取得処理
			//端末接続を監視
			if(WiFi.softAPgetStationNum()){
				led = getLed();
				led |= 0b0001;	
				setLed(led);	//接続LEDを点灯
			}else{
				led = getLed();	//接続led消灯
				led &= 0b1110;
				setLed(led);
			} 
			//ゼロキー
			if(zeroKey.isPush()){
				Serial2.print("Z\r\n");
			}
			//セットキー長押しで終了する。
			if(setKey.isLongPush()){
				config.flag = eCOMPLETE;
				endLodingDisp();
				setStrDisp("    ");
				WiFi.softAPdisconnect(true);
				setStrDisp("Con ");
				startLodingDisp(1, 100);
				wifiConect();
				endLodingDisp();
				break;
			}
		}
	}
}
