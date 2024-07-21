/**
 *
 *	@file		setWeb.cpp
 *
 *	@brief		設定WEB処理
 *
 *	@date		create : 2024/06/09
 *
 */
#include <Arduino.h>
#include "timer.hpp"
#include "setWeb.hpp"
#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include "SPIFFS.h"
#include <LittleFS.h>
#include "SPIFFS.h"
#include <ArduinoJson.h>
#include "eeprom.hpp"
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
void setupServer();
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

WebServer server(80);
/*==========================================================================================
	関数
==========================================================================================*/

/**
 *	@brief		インタラクタ
 *	@param		t:設定時間
 *	@retval		なし
 */

void setWebBegin() {
  if (!SPIFFS.begin(true)) {
    Serial.println("LittleFS mount failed");
  }
  WiFi.softAP(SET_SSID, SET_PAS);
    if (!MDNS.begin("TK-PLC")) {
    Serial.println("Error setting up MDNS responder!");
  } else {
    Serial.println("mDNS responder started");
    MDNS.addService("http", "tcp", 80);
  }
  Serial.println("Access Point Started");
  Serial.println(WiFi.softAPIP()); // IPアドレスを表示
  setupServer();
}

/**
 * @brief		サーバー終了処理
 * @param		なし
 * @retval		なし
 */
void setWebEnd() {
  server.stop();
  WiFi.disconnect();
}


void handleRoot() {
  File file = SPIFFS.open("/index.html", "r");
  if (!file) {
    server.send(404, "text/plain", "File Not Found");
    return;
  }else{
    Serial.println("File Opened");
    server.streamFile(file, "text/html");
    file.close();
  }
}

void handleIcon(){
  File file = SPIFFS.open("/favicon.ico", "r");
  if (!file) {
    server.send(404, "text/plain", "File Not Found");
    return;
  }else{
    Serial.println("File Opened");
    server.streamFile(file, "image/x-icon");
    file.close();
  }

}

void handleCss(){
  File file = SPIFFS.open("/style.css", "r");
  if (!file) {
    server.send(404, "text/plain", "File Not Found");
    return;
  }else{
    Serial.println("File Opened");
    server.streamFile(file, "text/css");
    file.close();
  }
}

void handleScript(){
  File file = SPIFFS.open("/script.js", "r");
  if (!file) {
    server.send(404, "text/plain", "File Not Found");
    return;
  }else{
    Serial.println("File Opened");
    server.streamFile(file, "application/javascript");
    file.close();
  }
}

void handleData() {
  JsonDocument jsonDoc;
  jsonDoc["time1"] = memory_data.waitTime[0];
  jsonDoc["time2"] = memory_data.waitTime[1];
  String jsonResponse;
  serializeJson(jsonDoc, jsonResponse);
  Serial.println(jsonResponse);
  // レスポンスを送信
  server.send(200, "application/json", jsonResponse);
}


void handleGetTime(int data) {
  String newData = server.arg("newData");
  char buf[16];
  newData.toCharArray(buf, sizeof(buf)); // String を char 配列にコピー
  memory_data.waitTime[data] = strtoul(buf, NULL, 10); // 10進数として解析
  Serial.println(memory_data.waitTime[data]);
  server.send(200);
  eepromSave();
}


void handleRestert() {
  Serial.print("restart");
  server.send(200);
  delay(1000);
  ESP.restart();  
}

void setupServer() {
  server.on("/",HTTP_GET, handleRoot);
  server.on("/favicon.ico",HTTP_GET, handleIcon);
  server.on("/style.css",HTTP_GET, handleCss);
  server.on("/script.js",HTTP_GET, handleScript);
  server.on("/getData", HTTP_GET, handleData);
  server.on("/time1Update", HTTP_POST, []() {
    handleGetTime(0);
  });
  server.on("/time2Update", HTTP_POST, []() {
    handleGetTime(1);
  });
  server.on("/restart", HTTP_POST, handleRestert);
  server.begin();
}
bool setWebMain(KeyInput &setKey){
  static bool setFlag = false;
  if(setKey.isLongPush()){
    Serial.println("setWeb");
    setFlag = !setFlag;
    if(setFlag){
      setWebBegin();
    }else{
      setWebEnd();
    }
  }
  if(setFlag){
     server.handleClient();  // クライアントからのリクエストを処理
  }
  return setFlag;
}