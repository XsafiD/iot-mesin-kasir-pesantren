#ifndef CONFIG_H
#define CONFIG_H

// =====================================================
// WIFI CONFIG
// =====================================================
#define WIFI_SSID           "ASA-SERVER-BLK"
#define WIFI_PASSWORD       "serverblk21"
#define WIFI_CONNECT_TIMEOUT_MS  15000   // 15 seconds
#define WIFI_RECONNECT_INTERVAL_MS 30000 // 30 seconds

// =====================================================
// API SERVER CONFIG
// =====================================================
#define API_BASE_URL        "http://127.0.0.1:8080"
#define API_KEY             "kasir-pesantren-secret-key-2026"
#define DEVICE_ID           "KASIR-PESANTREN-01"
#define API_TIMEOUT_MS      5000
#define API_MAX_RETRIES     2

// Endpoint paths
#define ENDPOINT_TRANSACTION  "/api/v1/transactions"
#define ENDPOINT_HEALTH       "/api/v1/health"
#define ENDPOINT_HEARTBEAT    "/api/v1/devices/heartbeat"
#define ENDPOINT_CARD_INFO    "/api/v1/cards/"

// =====================================================
// PIN CONFIGURATION
// =====================================================
// LCD I2C
#define I2C_SDA             21
#define I2C_SCL             22
#define LCD_ADDR            0x27
#define LCD_COLS            20
#define LCD_ROWS            4

// RFID MFRC522 (SPI)
#define RFID_SS             5
#define RFID_RST            4
// SPI default: SCK=18, MISO=19, MOSI=23

// Keypad 4x4 (TRANSPOSE LAYOUT - KEYPAD UNLABELED)
// Keypad ini menggunakan layout TRANSPOSE (row ↔ col swapped)
// Final working configuration dari KeypadAutoDetect & KeypadDiagnostic
// Updated: 2026-08-13 - ALL 16 BUTTONS TESTED WORKING!
#define KP_R1               26  // Col pin 1 → Row 1 (transpose)
#define KP_R2               27  // Col pin 2 → Row 2 (transpose)
#define KP_R3               14  // Col pin 3 → Row 3 (transpose)
#define KP_R4               13  // Col pin 4 → Row 4 (transpose)
#define KP_C1               4   // Row pin 1 → Col 1 (transpose)
#define KP_C2               32  // Row pin 2 → Col 2 (transpose)
#define KP_C3               33  // Row pin 3 → Col 3 (transpose)
#define KP_C4               25  // Row pin 4 → Col 4 (transpose)

// Feedback
#define BUZZER_PIN          15
#define LED_GREEN_PIN       2    // Built-in LED
#define LED_RED_PIN         33

// =====================================================
// BEHAVIORAL CONSTANTS
// =====================================================
#define MAX_NOMINAL_DIGITS    7      // Max Rp 9.999.999
#define INPUT_TIMEOUT_MS      30000  // 30 seconds
#define WAIT_RFID_TIMEOUT_MS  30000
#define HTTP_TIMEOUT_MS       10000
#define RESULT_DISPLAY_MS     3000   // 3 seconds
#define BUZZER_SUCCESS_MS     150
#define BUZZER_ERROR_MS       800
#define HEARTBEAT_INTERVAL_MS 60000  // 1 minute

// =====================================================
// FIRMWARE VERSION
// =====================================================
#define FIRMWARE_VERSION    "1.0.0"

#endif // CONFIG_H
