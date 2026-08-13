#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include "config.h"

extern LiquidCrystal_I2C lcd;

void displayInit();
void displayClear();
void displayShowIdle();
void displayShowInput(unsigned long nominal, bool blink);
void displayShowWaitRFID(unsigned long nominal);
void displayShowProcessing(unsigned long nominal);
void displayShowSuccess(unsigned long amount, long newBalance, const String& name);
void displayShowError(const String& title, const String& detail);
void displayShowOffline();
void displayShowWiFiConnecting(int dots);
void displayShowWiFiConnected(const String& ip);

#endif // DISPLAY_H
