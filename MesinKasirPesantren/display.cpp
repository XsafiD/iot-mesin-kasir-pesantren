#include "display.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(LCD_ADDR, LCD_COLS, LCD_ROWS);

// Helper function untuk padding string ke 20 karakter
static String padCenter(String s, int totalLength = 20) {
    int len = s.length();
    if (len >= totalLength) return s.substring(0, totalLength);

    int padding = (totalLength - len) / 2;
    String result = "";
    for (int i = 0; i < padding; i++) result += " ";
    result += s;
    for (int i = 0; i < totalLength - len - padding; i++) result += " ";
    return result;
}

// Helper function untuk pad string ke 20 karakter
static String padRight(String s, int totalLength = 20) {
    while (s.length() < totalLength) s += " ";
    return s.substring(0, totalLength);
}

// Helper function untuk memotong string ke 20 karakter
static String truncate(String s, int maxLength = 20) {
    if (s.length() <= maxLength) return s;
    return s.substring(0, maxLength);
}

static String formatRupiah(unsigned long amount) {
    String s = String(amount);
    String out = "";
    int count = 0;
    for (int i = s.length() - 1; i >= 0; i--) {
        if (count == 3) { out = "." + out; count = 0; }
        out = String(s[i]) + out;
        count++;
    }
    String result = "Rp " + out;

    // Truncate jika lebih dari 20 karakter
    if (result.length() > 20) {
        // Potong agar "Rp X.XXX.XXX" format
        result = result.substring(0, 20);
    }
    return result;
}

void displayInit() {
    Wire.begin(I2C_SDA, I2C_SCL);
    lcd.init();
    lcd.backlight();
    lcd.clear();
}

void displayClear() {
    lcd.clear();
}

void displayShowIdle() {
    lcd.clear();
    lcd.setCursor(0, 0); lcd.print("=== KASIR PESANTREN ===");
    lcd.setCursor(0, 1); lcd.print(padCenter("Sistem Siap"));
    lcd.setCursor(0, 2); lcd.print(padCenter("Input nominal belanja"));
    lcd.setCursor(0, 3); lcd.print(padCenter("Tekan # untuk lanjut"));
}

void displayShowInput(unsigned long nominal, bool blink) {
    lcd.setCursor(0, 0); lcd.print(padRight("Input Nominal:"));
    lcd.setCursor(0, 1);
    if (blink) {
        String rupiah = formatRupiah(nominal);
        // Center the rupiah string
        lcd.print(padCenter(rupiah));
    } else {
        lcd.print("                    ");  // 20 spaces
    }
    lcd.setCursor(0, 2); lcd.print(padCenter("# = OK  * = Hapus"));
    lcd.setCursor(0, 3); lcd.print(padCenter("Tekan nominal dulu"));
}

void displayShowWaitRFID(unsigned long nominal) {
    lcd.clear();
    lcd.setCursor(0, 0); lcd.print(padRight("Total: " +formatRupiah(nominal)));
    lcd.setCursor(0, 1); lcd.print("                    ");  // 20 spaces
    lcd.setCursor(0, 2); lcd.print(padCenter("Tap kartu siswa"));
    lcd.setCursor(0, 3); lcd.print(padCenter("* = Batal"));
}

void displayShowProcessing(unsigned long nominal) {
    lcd.clear();
    lcd.setCursor(0, 0); lcd.print(padCenter("Memproses transaksi"));
    lcd.setCursor(0, 1); lcd.print(padRight(formatRupiah(nominal)));
    lcd.setCursor(0, 2); lcd.print(padCenter("Mohon tunggu..."));
    lcd.setCursor(0, 3); lcd.print("                    ");  // 20 spaces
}

void displayShowSuccess(unsigned long amount, long newBalance, const String& name) {
    lcd.clear();
    lcd.setCursor(0, 0); lcd.print(padCenter("*** BERHASIL ***"));
    lcd.setCursor(0, 1); lcd.print(padRight(formatRupiah(amount)));
    lcd.setCursor(0, 2); lcd.print(padRight("Sisa: " + formatRupiah(newBalance)));
    lcd.setCursor(0, 3); lcd.print(padRight(truncate("Siswa: " + name)));
}

void displayShowError(const String& title, const String& detail) {
    lcd.clear();
    lcd.setCursor(0, 0); lcd.print(padCenter("! GAGAL !"));
    lcd.setCursor(0, 1); lcd.print(padCenter(truncate(title)));
    lcd.setCursor(0, 2); lcd.print(padCenter(truncate(detail)));
    lcd.setCursor(0, 3); lcd.print(padCenter("# = lanjut / auto 3s"));
}

void displayShowOffline() {
    lcd.clear();
    lcd.setCursor(0, 0); lcd.print(padCenter("! OFFLINE !"));
    lcd.setCursor(0, 1); lcd.print(padCenter("Server tidak terjangkau"));
    lcd.setCursor(0, 2); lcd.print(padCenter("Coba beberapa saat..."));
    lcd.setCursor(0, 3); lcd.print("                    ");  // 20 spaces
}

void displayShowWiFiConnecting(int dots) {
    lcd.setCursor(0, 3);
    String s = "WiFi";
    for (int i = 0; i < dots && i < 8; i++) s += ".";  // Max 8 dots
    lcd.print(padRight(s));
}

void displayShowWiFiConnected(const String& ip) {
    lcd.setCursor(0, 3);
    String s = "IP: " + ip;
    lcd.print(padRight(s));
}
