#include "state_machine.h"
#include "config.h"
#include "types.h"
#include "display.h"
#include "rfid_reader.h"
#include "keypad_input.h"
#include "feedback.h"
#include "wifi_manager.h"
#include "api_client.h"
#include <time.h>

static TxState state = TxState::IDLE;
static unsigned long stateEnteredAt = 0;

static unsigned long currentNominal = 0;
static String        nominalBuffer  = "";
static String        lastCardUid    = "";
static TransactionResult lastResult;

static unsigned long lastHeartbeat = 0;
static unsigned int  txnCounterToday = 0;
static int           lastTxnDay = -1;  // Untuk reset counter harian via NTP

static unsigned long lastBlink = 0;
static bool          blinkOn = true;

static void changeState(TxState newState) {
    Serial.print(F("[FSM] "));
    Serial.print((int)state);
    Serial.print(F(" -> "));
    Serial.println((int)newState);
    state = newState;
    stateEnteredAt = millis();
}

static bool stateTimedOut(unsigned long timeoutMs) {
    return (millis() - stateEnteredAt) >= timeoutMs;
}

static void handleIdle() {
    char key = keypadGetKey();
    if (key >= '1' && key <= '9') {  // Cegah '0' sebagai digit pertama (Rp 0 stuck)
        nominalBuffer = String(key);
        currentNominal = key - '0';
        changeState(TxState::INPUT_NOMINAL);
        blinkOn = true;
        lastBlink = millis();
        displayShowInput(currentNominal, blinkOn);
    }
}

static void handleInputNominal() {
    char key = keypadGetKey();
    if (key) {
        if (key >= '0' && key <= '9') {
            if (nominalBuffer.length() < MAX_NOMINAL_DIGITS) {
                nominalBuffer += key;
                currentNominal = nominalBuffer.toInt();
                blinkOn = true;
                lastBlink = millis();
                displayShowInput(currentNominal, blinkOn);
            }
        } else if (key == '#') {
            if (currentNominal > 0) {
                rfidReset();
                changeState(TxState::WAIT_RFID);
                displayShowWaitRFID(currentNominal);
                return;  // Cegah blink menimpa display
            } else {
                feedbackBeepError();  // Nominal kosong, kasih feedback ke user
            }
        } else if (key == '*') {
            nominalBuffer = "";
            currentNominal = 0;
            displayShowIdle();
            changeState(TxState::IDLE);
            return;  // Cegah blink menimpa display
        }
    }

    if (millis() - lastBlink >= 500) {
        lastBlink = millis();
        blinkOn = !blinkOn;
        displayShowInput(currentNominal, blinkOn);
    }

    if (stateTimedOut(INPUT_TIMEOUT_MS)) {
        nominalBuffer = "";
        currentNominal = 0;
        displayShowIdle();
        changeState(TxState::IDLE);
    }
}

static void handleWaitRfid() {
    char key = keypadGetKey();
    if (key == '*') {
        nominalBuffer = "";
        currentNominal = 0;
        displayShowIdle();
        changeState(TxState::IDLE);
        return;
    }

    String uid;
    if (rfidCheckCard(uid)) {
        lastCardUid = uid;
        Serial.print(F("[FSM] Card: "));
        Serial.println(uid);
        rfidHalt();
        displayShowProcessing(currentNominal);
        changeState(TxState::PROCESSING);
    }

    if (stateTimedOut(WAIT_RFID_TIMEOUT_MS)) {
        feedbackBeepError();
        displayShowError("Timeout", "Tap kartu lagi");
        nominalBuffer = "";
        currentNominal = 0;
        changeState(TxState::SHOW_ERROR);
    }
}

static void handleProcessing() {
    if (!wifiIsConnected()) {
        lastResult.status = TxnStatus::NETWORK_ERROR;
        lastResult.error_message = "WiFi terputus";
        displayShowError("Offline", "WiFi terputus");
        changeState(TxState::SHOW_ERROR);
        return;
    }

    lastResult = apiProcessTransaction(lastCardUid, currentNominal);

    if (lastResult.status == TxnStatus::SUCCESS) {
        displayShowSuccess((unsigned long)lastResult.amount,
                           lastResult.new_balance,
                           lastResult.card_holder);
        feedbackBeepSuccess();
        txnCounterToday++;
        changeState(TxState::SHOW_SUCCESS);
    } else {
        String title = txnStatusToString(lastResult.status);
        String detail;
        if (lastResult.status == TxnStatus::INSUFFICIENT_BALANCE) {
            detail = "Saldo: Rp " + String(lastResult.previous_balance);
        } else {
            detail = lastResult.error_message;
        }
        displayShowError(title, detail);
        feedbackBeepError();
        changeState(TxState::SHOW_ERROR);
    }
}

static void handleShowSuccess() {
    char key = keypadGetKey();
    if (key == '#' || stateTimedOut(RESULT_DISPLAY_MS)) {  // Skip dengan # atau auto 3s
        nominalBuffer = "";
        currentNominal = 0;
        displayShowIdle();
        feedbackLedGreen(false);
        changeState(TxState::IDLE);
    }
}

static void handleShowError() {
    char key = keypadGetKey();
    if (key == '#' || stateTimedOut(RESULT_DISPLAY_MS)) {  // Skip dengan # atau auto 3s
        nominalBuffer = "";
        currentNominal = 0;
        displayShowIdle();
        feedbackLedRed(false);
        changeState(TxState::IDLE);
    }
}

static void handleOffline() {
    if (wifiIsConnected()) {
        displayShowIdle();
        changeState(TxState::IDLE);
    } else if (stateTimedOut(5000)) {
        displayShowOffline();
        stateEnteredAt = millis();
    }
}

void fsmInit() {
    state = TxState::IDLE;
    stateEnteredAt = millis();
    nominalBuffer = "";
    currentNominal = 0;
    txnCounterToday = 0;
    lastTxnDay = -1;
    lastHeartbeat = 0;
    displayShowIdle();
}

void fsmTick() {
    feedbackUpdate();
    wifiLoop();

    // Reset counter transaksi harian via NTP (deteksi ganti hari)
    if (wifiIsConnected()) {
        time_t now = time(nullptr);
        if (now > (time_t)NTP_MIN_VALID_TIME) {
            struct tm timeinfo;
            localtime_r(&now, &timeinfo);
            if (lastTxnDay != -1 && timeinfo.tm_mday != lastTxnDay) {
                txnCounterToday = 0;
                Serial.println(F("[FSM] Hari berganti, reset txn counter harian"));
            }
            lastTxnDay = timeinfo.tm_mday;
        }
    }

    // Heartbeat hanya saat IDLE agar tidak mengganggu responsivitas keypad
    if (state == TxState::IDLE && wifiIsConnected() &&
        millis() - lastHeartbeat >= HEARTBEAT_INTERVAL_MS) {
        lastHeartbeat = millis();
        Serial.println(F("[FSM] Sending heartbeat..."));
        apiSendHeartbeat(millis() / 1000, wifiGetRssi(), ESP.getFreeHeap(), txnCounterToday);
    }

    if (!wifiIsConnected() &&
        state != TxState::PROCESSING &&
        state != TxState::SHOW_SUCCESS &&
        state != TxState::SHOW_ERROR) {
        if (state != TxState::OFFLINE) {
            changeState(TxState::OFFLINE);
            displayShowOffline();
        }
    } else if (state == TxState::OFFLINE && wifiIsConnected()) {
        changeState(TxState::IDLE);
        displayShowIdle();
    }

    switch (state) {
        case TxState::IDLE:           handleIdle();           break;
        case TxState::INPUT_NOMINAL:  handleInputNominal();   break;
        case TxState::WAIT_RFID:      handleWaitRfid();       break;
        case TxState::PROCESSING:     handleProcessing();     break;
        case TxState::SHOW_SUCCESS:   handleShowSuccess();    break;
        case TxState::SHOW_ERROR:     handleShowError();      break;
        case TxState::OFFLINE:        handleOffline();        break;
    }
}

TxState fsmGetState() { return state; }
