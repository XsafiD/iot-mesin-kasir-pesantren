#include "api_client.h"
#include "config.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

static String buildTransactionBody(const String& cardUid, unsigned long amount, const String& clientTxnId) {
    JsonDocument doc;
    doc["card_uid"]       = cardUid;
    doc["amount"]         = (long)amount;
    doc["device_id"]      = DEVICE_ID;
    doc["client_txn_id"]  = clientTxnId;
    doc["timestamp"]      = (long)time(nullptr);

    String body;
    serializeJson(doc, body);
    return body;
}

static String makeClientTxnId() {
    static unsigned int counter = 0;
    counter++;
    return String(DEVICE_ID) + "-" + String(millis()) + "-" + String(counter);
}

static TransactionResult parseResponse(int httpCode, const String& body) {
    TransactionResult r;
    r.http_code = httpCode;
    r.amount    = 0;

    JsonDocument doc;
    DeserializationError err = deserializeJson(doc, body);
    if (err) {
        r.status       = TxnStatus::UNKNOWN_ERROR;
        r.error_message = "Parse JSON gagal";
        return r;
    }

    const char* status = doc["status"] | "";

    if (strcmp(status, "success") == 0) {
        r.status           = TxnStatus::SUCCESS;
        r.transaction_id   = doc["transaction_id"] | "";
        r.card_holder      = doc["card_holder"] | "";
        r.previous_balance = doc["previous_balance"] | 0;
        r.amount           = doc["amount"] | 0;
        r.new_balance      = doc["new_balance"] | 0;
    } else {
        const char* code = doc["error_code"] | "UNKNOWN";
        const char* msg  = doc["message"]    | "Error tidak diketahui";
        r.error_message  = msg;
        r.card_holder    = doc["card_holder"]     | "";
        r.previous_balance = doc["current_balance"] | 0;

        if      (strcmp(code, "INSUFFICIENT_BALANCE") == 0) r.status = TxnStatus::INSUFFICIENT_BALANCE;
        else if (strcmp(code, "CARD_NOT_FOUND")       == 0) r.status = TxnStatus::CARD_NOT_FOUND;
        else if (strcmp(code, "CARD_INACTIVE")        == 0) r.status = TxnStatus::CARD_INACTIVE;
        else if (strcmp(code, "INVALID_REQUEST")      == 0) r.status = TxnStatus::INVALID_REQUEST;
        else if (strcmp(code, "UNAUTHORIZED_DEVICE")  == 0) r.status = TxnStatus::UNAUTHORIZED;
        else if (strcmp(code, "DUPLICATE_TRANSACTION")== 0) r.status = TxnStatus::DUPLICATE_TRANSACTION;
        else if (strcmp(code, "MAINTENANCE_MODE")     == 0) r.status = TxnStatus::MAINTENANCE_MODE;
        else                                                 r.status = TxnStatus::UNKNOWN_ERROR;
    }
    return r;
}

static void setCommonHeaders(HTTPClient& http) {
    http.setTimeout(API_TIMEOUT_MS);
    http.setConnectTimeout(API_TIMEOUT_MS);
    http.addHeader(F("Content-Type"), F("application/json"));
    http.addHeader(F("X-API-Key"),    F(API_KEY));
    http.addHeader(F("X-Device-Id"),  F(DEVICE_ID));
}

TransactionResult apiProcessTransaction(const String& cardUid, unsigned long amount) {
    TransactionResult result;
    result.status = TxnStatus::NETWORK_ERROR;
    result.amount = (long)amount;

    String url = String(API_BASE_URL) + ENDPOINT_TRANSACTION;
    String clientTxnId = makeClientTxnId();
    String body = buildTransactionBody(cardUid, amount, clientTxnId);

    Serial.print(F("[API] POST "));
    Serial.println(url);
    Serial.print(F("[API] Body: "));
    Serial.println(body);

    WiFiClient client;
    HTTPClient http;

    for (int attempt = 0; attempt <= API_MAX_RETRIES; attempt++) {
        if (!http.begin(client, url)) {
            Serial.println(F("[API] begin() failed"));
            continue;
        }
        setCommonHeaders(http);

        int httpCode = http.POST(body);

        if (httpCode > 0) {
            String response = http.getString();
            Serial.print(F("[API] HTTP "));
            Serial.print(httpCode);
            Serial.print(F(" Response: "));
            Serial.println(response);

            result = parseResponse(httpCode, response);
            result.amount = (long)amount;
            http.end();
            return result;
        } else {
            Serial.print(F("[API] Error code: "));
            Serial.println(httpCode);
        }
        http.end();
        delay(100);
    }

    result.status = TxnStatus::NETWORK_ERROR;
    result.error_message = "Server tidak merespons";
    return result;
}

bool apiCheckHealth() {
    String url = String(API_BASE_URL) + ENDPOINT_HEALTH;

    WiFiClient client;
    HTTPClient http;
    if (!http.begin(client, url)) return false;

    http.setTimeout(3000);
    http.addHeader(F("X-API-Key"), F(API_KEY));

    int code = http.GET();
    http.end();

    return (code == HTTP_CODE_OK);
}

bool apiSendHeartbeat(unsigned long uptimeSec, int rssi, size_t freeHeap, int txnToday) {
    String url = String(API_BASE_URL) + ENDPOINT_HEARTBEAT;

    JsonDocument doc;
    doc["device_id"]           = DEVICE_ID;
    doc["uptime_seconds"]      = (long)uptimeSec;
    doc["wifi_rssi"]           = rssi;
    doc["free_heap_bytes"]     = (long)freeHeap;
    doc["firmware_version"]    = FIRMWARE_VERSION;
    doc["transactions_today"]  = txnToday;

    String body;
    serializeJson(doc, body);

    WiFiClient client;
    HTTPClient http;
    if (!http.begin(client, url)) return false;
    setCommonHeaders(http);

    int code = http.POST(body);
    http.end();
    return (code == HTTP_CODE_OK);
}

bool apiGetCardInfo(const String& cardUid, String& outName, long& outBalance, bool& outActive) {
    String url = String(API_BASE_URL) + ENDPOINT_CARD_INFO + cardUid;

    WiFiClient client;
    HTTPClient http;
    if (!http.begin(client, url)) return false;
    http.setTimeout(3000);
    http.addHeader(F("X-API-Key"), F(API_KEY));

    int code = http.GET();
    if (code != HTTP_CODE_OK) { http.end(); return false; }

    String body = http.getString();
    http.end();

    JsonDocument doc;
    if (deserializeJson(doc, body)) return false;

    outName    = doc["holder_name"] | "";
    outBalance = doc["balance"]     | 0;
    outActive  = doc["is_active"]   | false;
    return true;
}
