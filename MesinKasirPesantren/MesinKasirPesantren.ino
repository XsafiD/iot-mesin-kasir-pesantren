/*
 * ==========================================================
 *  MESIN KASIR PESANTREN - Main Sketch
 *  Firmware v1.0.0
 *
 *  Hardware: ESP32 DevKit v1 + LCD 20x4 I2C + MFRC522 RFID
 *            + Keypad 4x4 + Buzzer + LED
 *
 *  Lihat: config.h untuk semua pengaturan
 * ==========================================================
 */

#include <Arduino.h>
#include <esp_task_wdt.h>
#include "config.h"
#include "types.h"
#include "display.h"
#include "rfid_reader.h"
#include "keypad_input.h"
#include "feedback.h"
#include "wifi_manager.h"
#include "api_client.h"
#include "state_machine.h"

void setup() {
    Serial.begin(115200);
    delay(200);

    // Watchdog timer (30 detik) — reset otomatis kalau firmware hang
    esp_task_wdt_init(30, true);
    esp_task_wdt_add(NULL);

    Serial.println();
    Serial.println(F("================================"));
    Serial.println(F(" Mesin Kasir Pesantren v1.0.0"));
    Serial.println(F("================================"));

    // 1. Init hardware drivers
    Serial.println(F("[SETUP] Init LCD..."));
    displayInit();
    lcd.setCursor(0, 0); lcd.print("=== KASIR PESANTREN ===");
    lcd.setCursor(0, 1); lcd.print("Booting...           ");

    Serial.println(F("[SETUP] Init RFID..."));
    rfidInit();

    Serial.println(F("[SETUP] Init Keypad..."));
    keypadInit();

    Serial.println(F("[SETUP] Init Feedback (buzzer/LED)..."));
    feedbackInit();

    // 2. Connect WiFi
    Serial.println(F("[SETUP] Connect WiFi..."));
    lcd.setCursor(0, 2); lcd.print("Connect WiFi...      ");
    int dots = 0;
    bool connected = false;
    while (!(connected = wifiConnectBlocking())) {
        displayShowWiFiConnecting(++dots);
        delay(500);
        if (dots > 60) {
            Serial.println(F("[SETUP] WiFi gagal, lanjut mode offline"));
            break;
        }
    }
    if (connected) {
        displayShowWiFiConnected(wifiGetIP());
        Serial.print(F("[SETUP] WiFi OK. IP="));
        Serial.println(wifiGetIP());
    }

    // 3. Health check server
    lcd.setCursor(0, 3); lcd.print("Cek server...        ");
    bool serverOk = apiCheckHealth();
    Serial.print(F("[SETUP] Server health: "));
    Serial.println(serverOk ? "OK" : "FAIL");

    // 4. Start FSM
    fsmInit();

    Serial.println(F("[SETUP] Selesai. Memasuki loop utama."));
    if (connected && serverOk) {
        feedbackBeepSuccess();
    } else {
        feedbackBeepError();
    }
}

void loop() {
    esp_task_wdt_reset();
    fsmTick();
}
