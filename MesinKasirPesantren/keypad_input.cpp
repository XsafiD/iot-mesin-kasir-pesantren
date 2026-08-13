#include "keypad_input.h"
#include <Keypad.h>

static const byte ROWS = 4, COLS = 4;
static const char keys[ROWS][COLS] = {
    {'1','2','3','A'},
    {'4','5','6','B'},
    {'7','8','9','C'},
    {'*','0','#','D'}
};
static const byte rowPins[ROWS] = {KP_R1, KP_R2, KP_R3, KP_R4};
static const byte colPins[COLS] = {KP_C1, KP_C2, KP_C3, KP_C4};

static Keypad keypad = Keypad(makeKeymap(keys), (byte*)rowPins, (byte*)colPins, ROWS, COLS);

void keypadInit() {
    keypad.setDebounceTime(50);
    Serial.println(F("[KEYPAD] initialized"));
}

char keypadGetKey() {
    return keypad.getKey();
}
