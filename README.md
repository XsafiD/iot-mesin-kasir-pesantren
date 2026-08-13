# 🛒 Mesin Kasir Pesantren - ESP32 Firmware

> Sistem mesin kasir berbasis ESP32 untuk pondok pesantren dengan keypad, RFID, dan koneksi ke backend server.

**Version**: 1.0.0
**Status**: ✅ Production Ready
**Board**: ESP32 DEVKIT V1
**Last Updated**: 2026-08-13

---

## 📋 Table of Contents

- [Features](#features)
- [Hardware Requirements](#hardware-requirements)
- [Quick Start](#quick-start)
- [⚙️ Configuration Guide](#️-configuration-guide) ← **START HERE!**
- [Building & Uploading](#building--uploading)
- [File Structure](#file-structure)
- [API Endpoints](#api-endpoints)
- [Troubleshooting](#troubleshooting)
- [Testing Guide](#testing-guide)

---

## ✨ Features

- 💳 **RFID Card Reader** - Scan kartu siswa dengan MFRC522
- ⌨️ **Keypad Input** - Input nominal belanja via keypad 4x4
- 📺 **LCD Display** - Feedback visual di LCD 20x4 I2C
- 🔔 **Audio/Visual Feedback** - Buzzer dan LED untuk notifikasi
- 📡 **WiFi Connectivity** - Koneksi ke backend server
- 🔒 **Secure API** - HTTP client dengan API key authentication
- 🔄 **Auto Reconnect** - WiFi auto-reconnect jika terputus
- 📊 **Heartbeat System** - Device monitoring ke server
- 🧠 **State Machine** - Non-blocking transaction processing
- 🌐 **Offline Mode** - Tetap berjalan walau server down

---

## 🛠️ Hardware Requirements

### Components Needed:

| Component | Specs | Notes |
|-----------|-------|-------|
| **ESP32 Board** | DEVKIT V1 | 4MB flash, dual-core |
| **LCD Display** | 20x4 with I2C | Address 0x27 (or 0x3F) |
| **RFID Reader** | MFRC522 | SPI interface |
| **Keypad** | 4x4 matrix | Avoid GPIO 12 (strapping pin) |
| **Buzzer** | 5V active buzzer | For audio feedback |
| **LEDs** | Green + Red | For visual feedback |
| **Power Supply** | 5V 2A minimum | ESP32 butuh ample power |

### Wiring Diagram:

```
┌─────────────────────────────────────────────────────────┐
│                    ESP32 DEVKIT V1                       │
│  ┌──────┐  ┌──────┐  ┌──────┐  ┌──────┐  ┌──────┐     │
│  │ LCD  │  │ RFID │  │ KEYPAD│  │BUZZER│  │ LEDS │     │
│  │ I2C  │  │ SPI  │  │ 4x4  │  │      │  │      │     │
│  └──┬───┘  └──┬───┘  └──┬───┘  └──┬───┘  └──┬───┘     │
│     │         │         │         │         │           │
│  SDA│21     SS│5        R1│13      │15       G│2 (LED)   │
│  SCL│22     RST│4        R2│14               R│33        │
│                      R3│16                          │
│                      R4│17                          │
│                      C1│25                          │
│                      C2│26                          │
│                      C3│27                          │
│                      C4│32                          │
└─────────────────────────────────────────────────────────┘
```

---

## 🚀 Quick Start

### Prerequisites:

1. **Arduino CLI** (version 1.5.1+)
   ```bash
   arduino-cli version
   ```

2. **ESP32 Core** installed
   ```bash
   arduino-cli core install esp32:esp32
   ```

3. **Required Libraries**:
   ```bash
   arduino-cli lib install RFID_MFRC522v2 LiquidCrystal_I2C Keypad ArduinoJson
   ```

### Quick Upload (Before Configuration Changes):

```bash
# 1. Navigate to project
cd /home/xsafi0/Documents/ASACYBER/IoT/01_PROJECTS

# 2. Upload with default config
arduino-cli compile --upload --fqbn esp32:esp32:esp32doit-devkit-v1 -p /dev/ttyUSB0 mesin-kasir-pesantren

# 3. Monitor serial
arduino-cli monitor -p /dev/ttyUSB0 --config baudrate=115200
```

⚠️ **Note**: Default configuration uses placeholder WiFi credentials. Configure first for production use!

---

## ⚙️ Configuration Guide

### 📝 Overview

Configuration files function like `.env` files - you edit them with your environment-specific settings, then recompile and upload.

**Main Configuration File**: `config.h` (all settings in one place!)

---

### 🔧 Step 1: Edit Configuration

Open `config.h` with your preferred editor:

```bash
# Using nano
nano config.h

# Using vim
vim config.h

# Using VS Code
code config.h
```

---

### 📶 Step 2: WiFi Configuration

**Location**: Lines 7-10 in `config.h`

```cpp
// =====================================================
// WIFI CONFIG
// =====================================================
#define WIFI_SSID           "PESANTREN-WIFI"      // ← GANTI INI!
#define WIFI_PASSWORD       "rahasia123"           // ← GANTI INI!
#define WIFI_CONNECT_TIMEOUT_MS  15000              // 15 seconds (optional)
#define WIFI_RECONNECT_INTERVAL_MS 30000            // 30 seconds (optional)
```

**What to Change**:

| Setting | Description | Example |
|---------|-------------|---------|
| `WIFI_SSID` | Nama WiFi Anda | `"MyHomeWiFi"` |
| `WIFI_PASSWORD` | Password WiFi | `"mypassword123"` |
| `WIFI_CONNECT_TIMEOUT_MS` | Timeout koneksi (ms) | `15000` (15s) |
| `WIFI_RECONNECT_INTERVAL_MS` | Interval reconnect (ms) | `30000` (30s) |

**Example Configuration**:
```cpp
#define WIFI_SSID           "Pesantren_Al-Ikhlas"
#define WIFI_PASSWORD       "santri12345"
```

---

### 🌐 Step 3: API Server Configuration

**Location**: Lines 15-25 in `config.h`

```cpp
// =====================================================
// API SERVER CONFIG
// =====================================================
#define API_BASE_URL        "http://192.168.1.100:8080"  // ← GANTI INI!
#define API_KEY             "kasir-pesantren-secret-key-2026"  // ← GANTI INI!
#define DEVICE_ID           "KASIR-PESANTREN-01"          // ← GANTI INI!
#define API_TIMEOUT_MS      5000
#define API_MAX_RETRIES     2

// Endpoint paths (biasanya tidak perlu diubah)
#define ENDPOINT_TRANSACTION  "/api/v1/transactions"
#define ENDPOINT_HEALTH       "/api/v1/health"
#define ENDPOINT_HEARTBEAT    "/api/v1/devices/heartbeat"
#define ENDPOINT_CARD_INFO    "/api/v1/cards/"
```

**What to Change**:

| Setting | Description | Example |
|---------|-------------|---------|
| `API_BASE_URL` | URL backend server (no trailing slash) | `"http://192.168.1.100:8080"` |
| `API_KEY` | Secret key untuk API authentication | `"my-secret-key-2026"` |
| `DEVICE_ID` | Unique ID untuk setiap device | `"KASIR-PESANTREN-01"` |

**Example Configuration**:
```cpp
#define API_BASE_URL        "http://10.0.0.50:3000"
#define API_KEY             "production-api-key-xyz789"
#define DEVICE_ID           "KASIR-KANTIN-01"
```

**Multiple Devices Example**:
```cpp
// Device 1 (Kantin Utama)
#define DEVICE_ID           "KASIR-KANTIN-01"

// Device 2 (Koperasi)
#define DEVICE_ID           "KASIR-KOP-02"

// Device 3 (Perpustakaan)
#define DEVICE_ID           "KASIR-PERPUS-03"
```

---

### 📍 Step 4: Hardware Pin Configuration (Optional)

**Only change if your wiring is different!**

**Location**: Lines 30-55 in `config.h`

```cpp
// LCD I2C
#define I2C_SDA             21
#define I2C_SCL             22
#define LCD_ADDR            0x27    // Try 0x3F if LCD doesn't work

// RFID MFRC522 (SPI)
#define RFID_SS             5
#define RFID_RST            4

// Keypad 4x4
#define KP_R1               13
#define KP_R2               14
#define KP_R3               16
#define KP_R4               17
#define KP_C1               25
#define KP_C2               26
#define KP_C3               27
#define KP_C4               32

// Feedback
#define BUZZER_PIN          15
#define LED_GREEN_PIN       2       // Built-in LED
#define LED_RED_PIN         33
```

**Common Changes**:

| Scenario | What to Change |
|----------|----------------|
| LCD address 0x3F | Change `LCD_ADDR` from `0x27` to `0x3F` |
| Different RFID wiring | Change `RFID_SS` and `RFID_RST` |
| Custom keypad layout | Change `KP_R1-4` and `KP_C1-4` |
| No buzzer | Comment out buzzer-related code |

---

### ⏱️ Step 5: Behavioral Configuration (Optional)

**Location**: Lines 60-67 in `config.h`

```cpp
#define MAX_NOMINAL_DIGITS    7      // Max Rp 9.999.999
#define INPUT_TIMEOUT_MS      30000  // 30 seconds input timeout
#define WAIT_RFID_TIMEOUT_MS  30000  // 30 seconds RFID scan timeout
#define RESULT_DISPLAY_MS     3000   // 3 seconds display result
#define BUZZER_SUCCESS_MS     150    // 150ms success beep
#define BUZZER_ERROR_MS       800    // 800ms error beep
#define HEARTBEAT_INTERVAL_MS 60000  // 1 minute heartbeat
```

**Common Adjustments**:

| Scenario | Setting to Change |
|----------|-------------------|
| Longer timeout for users | Increase `INPUT_TIMEOUT_MS` to `60000` (60s) |
| Faster transaction flow | Decrease `RESULT_DISPLAY_MS` to `2000` (2s) |
| Different beep patterns | Adjust `BUZZER_SUCCESS_MS` or `BUZZER_ERROR_MS` |
| More frequent heartbeat | Decrease `HEARTBEAT_INTERVAL_MS` to `30000` (30s) |

---

### 🎯 Step 6: Verify Your Changes

After editing, verify your configuration:

```bash
# Check WiFi settings
grep "WIFI_SSID\|WIFI_PASSWORD" config.h

# Check API settings
grep "API_BASE_URL\|API_KEY\|DEVICE_ID" config.h

# Full config review
cat config.h
```

---

### 🔄 Step 7: Apply Changes (Recompile & Upload)

After changing configuration, always recompile and upload:

```bash
cd /home/xsafi0/Documents/ASACYBER/IoT/01_PROJECTS

# Recompile with new config
arduino-cli compile --upload --fqbn esp32:esp32:esp32doit-devkit-v1 -p /dev/ttyUSB0 mesin-kasir-pesantren
```

---

### 📝 Configuration Template

Here's a complete template you can copy and modify:

```cpp
// =====================================================
// WIFI CONFIG - GANTI DENGAN WIFI KAMU
// =====================================================
#define WIFI_SSID           "YOUR_WIFI_NAME_HERE"
#define WIFI_PASSWORD       "YOUR_WIFI_PASSWORD_HERE"

// =====================================================
// API SERVER CONFIG - GANTI DENGAN SERVER KAMU
// =====================================================
#define API_BASE_URL        "http://YOUR_SERVER_IP:PORT"
#define API_KEY             "YOUR_API_KEY_HERE"
#define DEVICE_ID           "KASIR-PESANTREN-01"    // Unique per device

// =====================================================
// LAIN-LAIN - OPSIONAL
// =====================================================
// WiFi timeouts (optional)
#define WIFI_CONNECT_TIMEOUT_MS  15000
#define WIFI_RECONNECT_INTERVAL_MS 30000

// API settings (optional)
#define API_TIMEOUT_MS      5000
#define API_MAX_RETRIES     2

// Hardware (optional - hanya kalau wiring beda)
#define LCD_ADDR            0x27    // Atau 0x3F
```

---

### 🐧 Common Configuration Examples

#### Example 1: Home Network Testing
```cpp
#define WIFI_SSID           "MyHomeWiFi"
#define WIFI_PASSWORD       "password123"
#define API_BASE_URL        "http://192.168.1.100:8080"
#define API_KEY             "test-key-123"
#define DEVICE_ID           "KASIR-DEV-01"
```

#### Example 2: School Network (Production)
```cpp
#define WIFI_SSID           "PESANTREN-STUDENT"
#define WIFI_PASSWORD       "santri@2026!"
#define API_BASE_URL        "http://10.10.1.50:3000"
#define API_KEY             "prod-kasir-key-xyz789"
#define DEVICE_ID           "KASIR-KANTIN-A"
```

#### Example 3: Different Server Port
```cpp
#define API_BASE_URL        "http://backend.pesantren.sch.id:443"
#define API_KEY             "production-key-2026"
#define DEVICE_ID           "KASIR-KOPERASI-01"
```

---

## 🔨 Building & Uploading

### Standard Build & Upload:

```bash
arduino-cli compile --upload --fqbn esp32:esp32:esp32doit-devkit-v1 -p /dev/ttyUSB0 mesin-kasir-pesantren
```

### Build Only (No Upload):

```bash
arduino-cli compile --fqbn esp32:esp32:esp32doit-devkit-v1 mesin-kasir-pesantren
```

### Upload Only (Already Compiled):

```bash
arduino-cli upload -p /dev/ttyUSB0 --fqbn esp32:esp32:esp32doit-devkit-v1 mesin-kasir-pesantren
```

### Verify Board Connection:

```bash
arduino-cli board list
```

Expected output:
```
Port         Protocol Type              Board Name          FQBN
/dev/ttyUSB0 serial   Serial Port      Unknown
```

---

## 📁 File Structure

```
mesin-kasir-pesantren/
├── README.md                  ← This file!
├── PROGRESS.md                ← Implementation progress
├── config.h                   ← ⭐ MAIN CONFIGURATION FILE
├── mesin-kasir-pesantren.ino  ← Main entry point
├── types.h / types.cpp        ← Type definitions
├── display.h / display.cpp    ← LCD driver
├── rfid_reader.h / rfid_reader.cpp  ← RFID driver
├── keypad_input.h / keypad_input.cpp  ← Keypad driver
├── feedback.h / feedback.cpp   ← Buzzer/LED driver
├── wifi_manager.h / wifi_manager.cpp  ← WiFi manager
├── api_client.h / api_client.cpp  ← HTTP API client
└── state_machine.h / state_machine.cpp  ← Transaction FSM
```

**Key Files**:

- **`config.h`** - All configuration settings (WiFi, API, pins, behavior)
- **`mesin-kasir-pesantren.ino`** - Main program entry point
- **`state_machine.cpp`** - Core transaction logic
- **`api_client.cpp`** - Server communication

---

## 🔌 API Endpoints

### Transaction Processing

**Endpoint**: `POST /api/v1/transactions`

**Request Body**:
```json
{
  "card_uid": "A3:5F:22:1B",
  "amount": 15000,
  "device_id": "KASIR-PESANTREN-01",
  "client_txn_id": "KASIR-PESANTREN-01-12345678-1",
  "timestamp": 1723545678
}
```

**Success Response**:
```json
{
  "status": "success",
  "transaction_id": "TXN-20260813-001",
  "card_holder": "Ahmad Fulan",
  "previous_balance": 50000,
  "amount": 15000,
  "new_balance": 35000
}
```

**Error Response**:
```json
{
  "status": "error",
  "error_code": "INSUFFICIENT_BALANCE",
  "message": "Saldo tidak mencukupi",
  "card_holder": "Ahmad Fulan",
  "current_balance": 10000
}
```

### Other Endpoints

| Endpoint | Method | Purpose |
|----------|--------|---------|
| `/api/v1/health` | GET | Server health check |
| `/api/v1/devices/heartbeat` | POST | Device heartbeat |
| `/api/v1/cards/{uid}` | GET | Get card info |

---

## 🧪 Testing Guide

### 1. Hardware Testing

```bash
# Monitor serial output
arduino-cli monitor -p /dev/ttyUSB0 --config baudrate=115200
```

Expected boot sequence:
```
================================
 Mesin Kasir Pesantren v1.0.0
================================
[SETUP] Init LCD...
[RFID] MFRC522 initialized
[KEYPAD] initialized
[SETUP] Connect WiFi...
[WIFI] Connected. IP=192.168.1.105
[SETUP] Server health: OK
[SETUP] Selesai. Memasuki loop utama.
```

### 2. Functional Testing

**Test Scenario 1: Basic Transaction**
1. Input nominal: `1` `5` `0` `0` `0` `#`
2. LCD shows: "Total: Rp 15.000"
3. Tap RFID card
4. LCD shows success/error
5. Check Serial Monitor for API response

**Test Scenario 2: Error Handling**
1. Input nominal: `5` `0` `0` `0` `0` `#`
2. Tap card with insufficient balance
3. LCD shows: "Saldo kurang"
4. Check Serial for error details

**Test Scenario 3: WiFi Reconnect**
1. Turn off WiFi router
2. Wait 30 seconds
3. LCD shows: "! OFFLINE !"
4. Turn on WiFi router
5. Device auto-reconnects
6. LCD returns to idle screen

### 3. Expected LCD Displays

| Screen | Display Content |
|--------|----------------|
| **Idle** | `=== KASIR PESANTREN ===`<br>`Sistem Siap`<br>`Input nominal belanja`<br>`lalu tekan #` |
| **Input** | `Input Nominal:`<br>`Rp 15.000`<br>`# = OK  * = Hapus`<br>`                      ` |
| **Wait RFID** | `Total: Rp 15.000`<br>`                      `<br>`>> Tap kartu siswa <<`<br>`* = batal             ` |
| **Success** | `*** BERHASIL ***     `<br>`Rp 15.000            `<br>`Sisa: Rp 35.000       `<br>`Ahmad Fulan          ` |
| **Error** | `! GAGAL !            `<br>`Saldo kurang          `<br>`Saldo: Rp 10.000     `<br>`                      ` |
| **Offline** | `! OFFLINE !           `<br>`Server tidak         `<br>`terjangkau.          `<br>`Coba beberapa saat... ` |

---

## 🔍 Troubleshooting

### Common Issues & Solutions

#### Issue 1: WiFi Not Connecting

**Symptoms**:
- LCD shows "! OFFLINE !"
- Serial shows: `[WIFI] FAILED to connect within timeout`

**Solutions**:

1. **Check WiFi credentials**:
   ```bash
   grep "WIFI_SSID\|WIFI_PASSWORD" config.h
   ```

2. **Verify WiFi is in range**:
   - ESP32 antenna should have clear line-of-sight
   - Signal strength > -70dBm recommended

3. **Check for typos**:
   ```cpp
   #define WIFI_SSID    "MyWiFi"        // Correct
   #define WIFI_SSID    "My WiFi"       // ❌ Spaces in quotes!
   ```

#### Issue 2: LCD Not Working

**Symptoms**:
- LCD screen is blank or only shows blue blocks
- No text displayed

**Solutions**:

1. **Try different I2C address**:
   ```cpp
   #define LCD_ADDR  0x3F    // Change from 0x27
   ```

2. **Check wiring**:
   - SDA → GPIO 21
   - SCL → GPIO 22
   - VCC → 5V
   - GND → GND

3. **Adjust contrast**:
   - Turn potentiometer on LCD back

#### Issue 3: RFID Not Reading Cards

**Symptoms**:
- Tap card but nothing happens
- Serial doesn't show card UID

**Solutions**:

1. **Check RFID wiring**:
   ```
   SS  → GPIO 5
   RST → GPIO 4
   SCK → GPIO 18 (default)
   MISO→ GPIO 19 (default)
   MOSI→ GPIO 23 (default)
   ```

2. **Verify power**:
   - MFRC522 needs 3.3V power
   - Check 3.3V pin on ESP32

3. **Test card proximity**:
   - Hold card within 2-3cm of reader
   - Wait 1-2 seconds between taps

#### Issue 4: API Server Not Responding

**Symptoms**:
- Transaction fails with "Server down"
- Serial shows: `[API] Error code: -1`

**Solutions**:

1. **Test API from computer**:
   ```bash
   curl http://YOUR_SERVER_IP:PORT/api/v1/health
   ```

2. **Check API_BASE_URL**:
   ```cpp
   #define API_BASE_URL  "http://192.168.1.100:8080"  // No trailing slash!
   ```

3. **Verify server is running**:
   - Check backend server logs
   - Confirm port is correct

#### Issue 5: Compilation Errors

**Symptoms**:
- Build fails with error messages

**Common fixes**:

1. **Missing libraries**:
   ```bash
   arduino-cli lib install RFID_MFRC522v2 LiquidCrystal_I2C Keypad ArduinoJson
   ```

2. **Wrong board selected**:
   ```bash
   arduino-cli compile --fqbn esp32:esp32:esp32doit-devkit-v1 ...
   ```

3. **Syntax errors in config.h**:
   - Check for missing quotes
   - Verify no trailing commas in defines

---

## 📞 Support & Contact

**Project**: Mesin Kasir Pesantren
**Version**: 1.0.0
**Date**: 2026-08-13
**Developer**: xsafi0

**Documentation Files**:
- `README.md` - This file (user guide)
- `PROGRESS.md` - Implementation progress
- `config.h` - Configuration template

---

## 📜 License

This firmware is part of the Mesin Kasir Pesantren project for pondok pesantren use.

---

## 🎯 Quick Reference

### Essential Commands:

```bash
# Edit configuration
nano config.h

# Build and upload
arduino-cli compile --upload --fqbn esp32:esp32:esp32doit-devkit-v1 -p /dev/ttyUSB0 mesin-kasir-pesantren

# Monitor serial
arduino-cli monitor -p /dev/ttyUSB0 --config baudrate=115200

# Check board
arduino-cli board list

# List libraries
arduino-cli lib list
```

### Configuration Checklist:

- [ ] Edit `WIFI_SSID` and `WIFI_PASSWORD`
- [ ] Edit `API_BASE_URL` with server URL
- [ ] Edit `API_KEY` with correct key
- [ ] Edit `DEVICE_ID` with unique identifier
- [ ] Verify board connection at `/dev/ttyUSB0`
- [ ] Recompile and upload
- [ ] Monitor Serial for boot sequence
- [ ] Test with keypad and RFID card

---

**Happy Coding! 🚀**

Untuk pertanyaan atau issues, cek bagian [Troubleshooting](#troubleshooting) di atas.
