# Pemetaan Pin Modul - Mesin Kasir Pesantren

**Project:** MesinKasirPesantren v1.0.0
**Board:** ESP32 DevKit V1 (30-pin) di atas Expansion Board (S-V-G)
**Tanggal:** 2026-08-15
**Status Wiring:** ✅ RFID, keypad, LCD teruji (2026-08-15, sketch `01_PROJECTS/TestKeypadRfidLcd`)
**Sumber config:** `MesinKasirPesantren/config.h`

---

## 🗺️ Peta Alokasi Pin Master (Semua GPIO)

| GPIO | Kolom Exp. Board | Modul | Fungsi | Status |
|------|------------------|-------|--------|--------|
| 2  | D2  | RFID RC522 | RST (hardware, idle HIGH) | ⚠️ khusus RFID — jangan dipakai lain* |
| 4  | D4  | Keypad | colPins[0] (transpose) | ✅ |
| 5  | D5  | RFID RC522 | SDA / SS (SPI) | ✅ |
| 13 | D13 | Keypad | rowPins[3] (transpose) | ✅ |
| 14 | D14 | Keypad | rowPins[2] (transpose) | ✅ |
| 15 | D15 | Buzzer | Output aktif-HIGH | ⚠️ strapping pin (boot log) |
| 16 | D16 | LED Hijau | Output | ✅ (dipindah dari GPIO2) |
| 17 | D17 | LED Merah | Output | ✅ (dipindah dari GPIO33) |
| 18 | D18 | RFID RC522 | SCK (VSPI CLK) | ✅ |
| 19 | D19 | RFID RC522 | MISO (VSPI MISO) | ✅ |
| 21 | D21 | LCD I2C | SDA | ✅ |
| 22 | D22 | LCD I2C | SCL | ✅ |
| 23 | D23 | RFID RC522 | MOSI (VSPI MOSI) | ✅ |
| 25 | D25 | Keypad | colPins[3] (transpose) | ✅ |
| 26 | D26 | Keypad | rowPins[0] (transpose) | ✅ |
| 27 | D27 | Keypad | rowPins[1] (transpose) | ✅ |
| 32 | D32 | Keypad | colPins[1] (transpose) | ✅ |
| 33 | D33 | Keypad | colPins[2] (transpose) | ✅ |

\* GPIO2 = strapping pin + LED onboard ESP32. LED onboard akan menyala redup saat RST RC522 idle HIGH — ini normal, justru jadi indikator RFID powered.

### Pin Bebas (Cadangan Pengembangan)

| GPIO | Kolom | Catatan |
|------|-------|---------|
| 34, 35 | D34, D35 | Input-only (tanpa pull-up/down internal) — cocok sensor analog |
| 36 (VP), 39 (VN) | VP, VN | Input-only — sensor analog |
| 1 (TX0), 3 (RX0) | TX0, RX0 | ⚠️ Jangan dipakai — dipakai USB serial (upload/debug) |

### Pin yang Dilarang Dipakai

- **GPIO 6–11**: terhubung flash internal → ESP32 crash/fail boot.
- **GPIO 0** (BOOT): strapping pin, tidak tersedia di kolom expansion board.
- **GPIO 12**: strapping pin (MTDO) — bisa menggagalkan boot jika HIGH saat reset.

---

## 1️⃣ RFID RC522 (Mode SPI) — ✅ TERUJI

VCC wajib **3.3V** (dari terminal 3.3V blok daya kiri expansion board, BUKAN baris V).

| Pin RC522 | Fungsi | → GPIO | → Kolom Exp. Board (pin S) |
|-----------|--------|--------|---------------------------|
| SDA | SS (Slave Select) | GPIO5 | **D5** |
| SCK | SPI Clock | GPIO18 | **D18** |
| MOSI | Master Out Slave In | GPIO23 | **D23** |
| MISO | Master In Slave Out | GPIO19 | **D19** |
| RST | Reset (idle HIGH) | GPIO2 | **D2** |
| IRQ | Interrupt | ❌ tidak dicolok | - |
| 3.3V | VCC | - | Terminal **3.3V** |
| GND | Ground | - | Baris **G** / blok GND |

Config di kode: `RFID_SS=5`, `RFID_RST=2` (RST tidak dipakai library MFRC522v2 — murni fisik/dokumentasi).

Urutan header fisik RC522 dari tepi: `3V3 · RST · GND · IRQ · MISO · MOSI · SCK · SDA`.

---

## 2️⃣ Keypad 4x4 No-Label (TRANSPOSE) — ✅ TERUJI

Keypad punya 10 pin male header; **pin 1 tidak dipakai**, pin 2–9 adalah 8 pin aktif.
Layout-nya transpose: row fisik ↔ col tertukar, jadi pin 2–5 jadi **colPins** di kode, pin 6–9 jadi **rowPins**.

| Pin Keypad (fisik) | Peran di Kode | GPIO | → Kolom Exp. Board (pin S) |
|--------------------|---------------|------|---------------------------|
| 2 | colPins[0] (`KP_C1`) | GPIO4 | **D4** |
| 3 | colPins[1] (`KP_C2`) | GPIO32 | **D32** |
| 4 | colPins[2] (`KP_C3`) | GPIO33 | **D33** |
| 5 | colPins[3] (`KP_C4`) | GPIO25 | **D25** |
| 6 | rowPins[0] (`KP_R1`) | GPIO26 | **D26** |
| 7 | rowPins[1] (`KP_R2`) | GPIO27 | **D27** |
| 8 | rowPins[2] (`KP_R3`) | GPIO14 | **D14** |
| 9 | rowPins[3] (`KP_R4`) | GPIO13 | **D13** |

Keymap (standar, tidak berubah karena transpose ditangani tukar row/col pins):

```
1 2 3 A
4 5 6 B
7 8 9 C
* 0 # D
```

Referensi lengkap: `04_ARCHIVES/2026-08-13 - Keypad/KEYPAD_TRANSPOSE_GUIDE.md`.

---

## 3️⃣ LCD 20x4 I2C — ✅ TERUJI

Address I2C: `0x27` (fallback umum: `0x3F`).

| Pin LCD | Fungsi | → GPIO | → Kolom Exp. Board |
|---------|--------|--------|--------------------|
| SDA | I2C Data | GPIO21 | **D21** (atau header I2C 4-pin pojok) |
| SCL | I2C Clock | GPIO22 | **D22** (atau header I2C 4-pin pojok) |
| VCC | Power 5V | - | Baris **V** (pastikan JUMP V = 5V) / terminal 5V |
| GND | Ground | - | Baris **G** / blok GND |

> ⚠️ Jangan tertukar: **SDA/SCL LCD ≠ SDA RC522**. SDA RC522 itu SPI Slave Select → D5.

---

## 4️⃣ Buzzer & LED (Feedback)

| Komponen | Pin | → GPIO | → Kolom Exp. Board (pin S) | Perilaku |
|----------|-----|--------|---------------------------|----------|
| Buzzer aktif 5V | + | GPIO15 | **D15** | HIGH = bunyi (150ms sukses / 800ms error) |
| Buzzer aktif 5V | - | - | Baris **G** | - |
| LED Hijau | Anoda (+ via resistor 220Ω–330Ω) | GPIO16 | **D16** | Transaksi sukses / indikator WiFi-server OK |
| LED Merah | Anoda (+ via resistor 220Ω–330Ω) | GPIO17 | **D17** | Transaksi gagal / error |
| Katoda kedua LED | - | - | Baris **G** | - |

Riwayat perpindahan pin (penting untuk dokumentasi):
- `LED_GREEN_PIN`: GPIO2 → **GPIO16** (konflik dengan RST RFID di D2, 2026-08-15)
- `LED_RED_PIN`: GPIO33 → **GPIO17** (konflik dengan KP_C3 keypad)
- `RFID_RST`: GPIO4 → GPIO16 → **GPIO2** (kini sesuai wiring fisik teruji)

---

## 5️⃣ ESP32 DevKit V1 + Expansion Board

**Expansion board** menampilkan setiap GPIO ESP32 ke 3 baris header per kolom:

- **S (kuning)** = sinyal GPIO → **semua kabel modul colok ke sini**
- **V (merah)** = tegangan (5V atau 3.3V, tergantung **JUMP V**)
- **G (hitam)** = ground

Layout kolom (urut dari atas):

```
SISI KIRI (S-V-G)          SISI KANAN (G-V-S)
┌──────────────┐           ┌──────────────┐
│ EN  VP  VN   │           │ D23 ← RFID MOSI
│ D34 D35      │           │ D22 ← LCD SCL
│ D32 ← KP_C2  │           │ TX0 RX0 (jangan dipakai)
│ D33 ← KP_C3  │           │ D21 ← LCD SDA
│ D25 ← KP_C4  │           │ D19 ← RFID MISO
│ D26 ← KP_R1  │           │ D18 ← RFID SCK
│ D27 ← KP_R2  │           │ D5  ← RFID SS/SDA
│ D14 ← KP_R3  │           │ D17 ← LED Merah
│ D12 (larang) │           │ D16 ← LED Hijau
│ D13 ← KP_R4  │           │ D4  ← KP_C1
│              │           │ D2  ← RFID RST
│              │           │ D15 ← Buzzer
└──────────────┘           └──────────────┘
```

Blok daya kiri bawah: terminal **3×3.3V, 3×5V, 3×GND** — RFID ambil 3.3V dari sini; LCD & buzzer 5V.

---

## 🔌 Checklist Wiring Cepat (Assembly Ulang)

1. [ ] ESP32 nempel di expansion board, label GPIO sejajar kolom
2. [ ] RFID: SDA→D5, SCK→D18, MOSI→D23, MISO→D19, RST→D2, IRQ lepas, VCC→3.3V, GND→G
3. [ ] Keypad pin 2→D4, 3→D32, 4→D33, 5→D25, 6→D26, 7→D27, 8→D14, 9→D13
4. [ ] LCD: SDA→D21, SCL→D22, VCC→5V, GND→G
5. [ ] Buzzer: +→D15, -→G
6. [ ] LED hijau (via resistor)→D16, LED merah (via resistor)→D17, katoda→G
7. [ ] JUMP V disesuaikan (LCD butuh 5V — aman ambil V dari terminal 5V langsung)
8. [ ] Upload → cek Serial: `RC522 firmware 0x91/0x92` = wiring RFID benar

---

## 📎 Referensi

- `docs/2026-08-15 - Tabel-Pinout-RFID-RC522.md` (root IoT)
- `docs/2026-08-15 - Tabel-Pinout-ESP32-Expansion-Board.md` (root IoT)
- `docs/2026-08-13 - Tabel-Pinout-ESP32-DevKit-V1.md` (root IoT)
- `04_ARCHIVES/2026-08-13 - Keypad/KEYPAD_TRANSPOSE_GUIDE.md`
- `01_PROJECTS/TestKeypadRfidLcd/2026-08-15 - Pemetaan-Pinout-RC522-ke-Expansion-Board.md`
- `MesinKasirPesantren/config.h` (sumber kebenaran pin di firmware)
