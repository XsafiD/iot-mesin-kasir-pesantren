#include "wifi_manager.h"

static unsigned long lastReconnectAttempt = 0;

static void onWiFiEvent(WiFiEvent_t event) {
    switch (event) {
        case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
            Serial.println(F("[WIFI] Disconnected! Will retry..."));
            break;
        case ARDUINO_EVENT_WIFI_STA_GOT_IP:
            Serial.print(F("[WIFI] Connected! IP: "));
            Serial.println(WiFi.localIP());
            break;
        default: break;
    }
}

bool wifiConnectBlocking() {
    WiFi.onEvent(onWiFiEvent);
    WiFi.mode(WIFI_STA);
    WiFi.setAutoReconnect(true);
    WiFi.persistent(true);

    Serial.print(F("[WIFI] Connecting to "));
    Serial.print(WIFI_SSID);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    unsigned long startAttempt = millis();
    while (WiFi.status() != WL_CONNECTED &&
           millis() - startAttempt < WIFI_CONNECT_TIMEOUT_MS) {
        delay(100);
        Serial.print(".");
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println();
        Serial.print(F("[WIFI] Connected. IP="));
        Serial.println(WiFi.localIP());
        Serial.print(F("[WIFI] RSSI="));
        Serial.println(WiFi.RSSI());
        return true;
    }
    Serial.println();
    Serial.println(F("[WIFI] FAILED to connect within timeout"));
    return false;
}

void wifiLoop() {
    if (WiFi.status() != WL_CONNECTED) {
        unsigned long now = millis();
        if (now - lastReconnectAttempt >= WIFI_RECONNECT_INTERVAL_MS) {
            lastReconnectAttempt = now;
            Serial.println(F("[WIFI] Reconnecting..."));
            WiFi.disconnect();
            WiFi.reconnect();
        }
    }
}

bool wifiIsConnected() {
    return WiFi.status() == WL_CONNECTED;
}

String wifiGetIP() {
    return WiFi.localIP().toString();
}

int wifiGetRssi() {
    return WiFi.RSSI();
}
