#ifndef API_CLIENT_H
#define API_CLIENT_H

#include <Arduino.h>
#include "types.h"

TransactionResult apiProcessTransaction(const String& cardUid, unsigned long amount);
bool apiCheckHealth();
bool apiSendHeartbeat(unsigned long uptimeSec, int rssi, size_t freeHeap, int txnToday);
bool apiGetCardInfo(const String& cardUid, String& outName, long& outBalance, bool& outActive);

#endif // API_CLIENT_H
