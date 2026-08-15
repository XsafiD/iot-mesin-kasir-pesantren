#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>
#include "config.h"
#include <WiFi.h>

bool wifiConnectBlocking();
void wifiLoop();
bool wifiIsConnected();
String wifiGetIP();
int wifiGetRssi();
void wifiInitNTP();
bool wifiIsTimeSynced();

#endif // WIFI_MANAGER_H
