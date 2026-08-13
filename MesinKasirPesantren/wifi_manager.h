#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>
#include <WiFi.h>
#include "config.h"

bool wifiConnectBlocking();
void wifiLoop();
bool wifiIsConnected();
String wifiGetIP();
int wifiGetRssi();

#endif // WIFI_MANAGER_H
