#ifndef TYPES_H
#define TYPES_H

#include <Arduino.h>

enum class TxnStatus {
    SUCCESS,
    INSUFFICIENT_BALANCE,
    CARD_NOT_FOUND,
    CARD_INACTIVE,
    INVALID_REQUEST,
    UNAUTHORIZED,
    DUPLICATE_TRANSACTION,
    MAINTENANCE_MODE,
    NETWORK_ERROR,
    UNKNOWN_ERROR
};

struct TransactionResult {
    TxnStatus status;
    String    transaction_id;
    String    card_holder;
    long      previous_balance;
    long      amount;
    long      new_balance;
    String    error_message;
    int       http_code;
};

String txnStatusToString(TxnStatus s);

#endif // TYPES_H
