#include "wifi_manager.h"
#include <time.h>

static unsigned long lastReconnectAttempt = 0;

static void onWiFiEvent(WiFiEvent_t event) {
    switch (event) {
        case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
            Serial.println(F("[WIFI] Disconnected! Will retry..."));
            break;
        case ARDUINO_EVENT_WIFI_STA_GOT_IP:
            Serial.print(F("[WIFI] Connected! IP: "));
            Serial.println(WiFi.localIP());
            wifiInitNTP();
            Serial.println(F("[WIFI] NTP time sync triggered on GOT_IP"));
            break;
        default: break;
    }
}

bool wifiConnectBlocking() {
    WiFi.onEvent(onWiFiEvent);
    WiFi.mode(WIFI_STA);
    WiFi.setAutoReconnect(true);
    WiFi.persistent(false);  // Cegah flash wear (credentials di-supply via begin())

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
    // Auto-reconnect dihandle oleh WiFi.setAutoReconnect(true)
    // Fallback: kalau auto-reconnect gagal setelah interval, coba manual (tanpa disconnect)
    if (WiFi.status() != WL_CONNECTED) {
        unsigned long now = millis();
        if (now - lastReconnectAttempt >= WIFI_RECONNECT_INTERVAL_MS) {
            lastReconnectAttempt = now;
            Serial.println(F("[WIFI] Auto-reconnect belum sukses, coba manual..."));
            WiFi.reconnect();  // Tidak pakai disconnect() agar tidak ganggu auto-reconnect
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

void wifiInitNTP() {
    // Sync jam via NTP server (WIB = GMT+7)
    configTime(NTP_TZ_OFFSET_SEC, 0, NTP_SERVER_1, NTP_SERVER_2);
    Serial.println(F("[WIFI] NTP time sync initialized (WIB GMT+7)"));
}

bool wifiIsTimeSynced() {
    // true kalau NTP sudah sync (time > ~Nov 2023)
    return time(nullptr) > (time_t)NTP_MIN_VALID_TIME;
}
