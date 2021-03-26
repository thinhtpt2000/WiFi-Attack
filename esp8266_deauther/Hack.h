/* This software is licensed under the MIT License: https://github.com/spacehuhntech/esp8266_deauther */

#pragma once

#include "Arduino.h"
#include <ESP8266WiFi.h>
extern "C" {
  #include "user_interface.h"
}
#include "src/ArduinoJson-v5.13.5/ArduinoJson.h"
#include "SimpleList.h"
#include "language.h"
#include "Accesspoints.h"
#include "Scan.h"
#include "Attack.h"

#define HACK_LIST_SIZE 25

extern Accesspoints accesspoints;
extern Scan scan;
extern Attack attack;

extern void checkFile(String path, String data);
extern JsonVariant parseJSONFile(String path, DynamicJsonBuffer& jsonBuffer);
extern bool writeFile(String path, String& buf);
extern bool appendFile(String path, String& buf);
extern String b2s(bool input);
extern String fixUtf8(String str);

class Hack {
    public:
        Hack();

        void start();
        void stop();
        void update();
        void load();
        void verifyPassword(String password);
        bool isRunning();
        void remove(int num);

    private:
        bool running = false;
        String FILE_PATH = "/password.json";
        int CONNECTION_TIMEOUT = 10 * 1000; // 10s

        struct HackTarget {
            String mac;
            String ssid;
            String password;
            bool verified;
        };

        SimpleList<HackTarget>* list;
        HackTarget ct;
        
        void save();
        int count();
        int internal_checkExist(String mac, String ssid);
        void internal_add(String mac, String ssid, String password, bool verified);
//        void internal_update(int idx, String mac, String ssid, String password, uint32_t lastUpdate, bool verified);
        void internal_update(int idx, String password, bool verified);
        void internal_remove(int num);
        void internal_removeAll();
        bool check(int num);
        String getMac(int num);
        String getSsid(int num);
        String getPassword(int num);
        bool getVerified(int num);
};
