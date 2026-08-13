#ifndef RFID_READER_H
#define RFID_READER_H

#include <Arduino.h>
#include "config.h"

void rfidInit();
bool rfidCheckCard(String& outUid);
void rfidHalt();

#endif // RFID_READER_H
