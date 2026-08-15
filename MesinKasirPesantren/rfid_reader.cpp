#include "rfid_reader.h"
#include <SPI.h>
#include <MFRC522v2.h>
#include <MFRC522DriverSPI.h>
#include <MFRC522DriverPinSimple.h>

static MFRC522DriverPinSimple ss_pin(RFID_SS);
static MFRC522DriverSPI driver(ss_pin);
static MFRC522 mfrc522(driver);

void rfidInit() {
    SPI.begin();
    mfrc522.PCD_Init();
    Serial.println(F("[RFID] MFRC522 initialized"));
}

bool rfidCheckCard(String& outUid) {
    if (!mfrc522.PICC_IsNewCardPresent()) return false;
    if (!mfrc522.PICC_ReadCardSerial())   return false;

    outUid = "";
    for (byte i = 0; i < mfrc522.uid.size; i++) {
        if (mfrc522.uid.uidByte[i] < 0x10) outUid += "0";
        outUid += String(mfrc522.uid.uidByte[i], HEX);
    }
    outUid.toUpperCase();
    return true;
}

void rfidHalt() {
    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();
}

void rfidReset() {
    rfidHalt();
    mfrc522.PCD_Init();
}
