#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include <Arduino.h>

enum class TxState {
    IDLE,
    INPUT_NOMINAL,
    WAIT_RFID,
    PROCESSING,
    SHOW_SUCCESS,
    SHOW_ERROR,
    OFFLINE
};

void fsmInit();
void fsmTick();
TxState fsmGetState();

#endif // STATE_MACHINE_H
