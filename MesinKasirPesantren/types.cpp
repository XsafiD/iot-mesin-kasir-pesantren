#include "types.h"

String txnStatusToString(TxnStatus s) {
    switch (s) {
        case TxnStatus::SUCCESS:                return F("Berhasil");
        case TxnStatus::INSUFFICIENT_BALANCE:   return F("Saldo kurang");
        case TxnStatus::CARD_NOT_FOUND:         return F("Kartu tak dikenal");
        case TxnStatus::CARD_INACTIVE:          return F("Kartu nonaktif");
        case TxnStatus::INVALID_REQUEST:        return F("Input salah");
        case TxnStatus::UNAUTHORIZED:           return F("Device tak diizinkan");
        case TxnStatus::DUPLICATE_TRANSACTION:  return F("Transaksi dobel");
        case TxnStatus::MAINTENANCE_MODE:       return F("Maintenance");
        case TxnStatus::NETWORK_ERROR:          return F("Server down");
        default:                                return F("Error");
    }
}
