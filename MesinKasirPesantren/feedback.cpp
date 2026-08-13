#include "feedback.h"

static unsigned long buzzerOffAt = 0;
static bool buzzerActive = false;

void feedbackInit() {
    pinMode(BUZZER_PIN, OUTPUT);
    pinMode(LED_GREEN_PIN, OUTPUT);
    pinMode(LED_RED_PIN, OUTPUT);
    digitalWrite(BUZZER_PIN, LOW);
    digitalWrite(LED_GREEN_PIN, LOW);
    digitalWrite(LED_RED_PIN, LOW);
}

void feedbackBeepSuccess() {
    digitalWrite(BUZZER_PIN, HIGH);
    digitalWrite(LED_GREEN_PIN, HIGH);
    digitalWrite(LED_RED_PIN, LOW);
    buzzerActive = true;
    buzzerOffAt = millis() + BUZZER_SUCCESS_MS;
}

void feedbackBeepError() {
    digitalWrite(BUZZER_PIN, HIGH);
    digitalWrite(LED_RED_PIN, HIGH);
    digitalWrite(LED_GREEN_PIN, LOW);
    buzzerActive = true;
    buzzerOffAt = millis() + BUZZER_ERROR_MS;
}

void feedbackLedGreen(bool on) { digitalWrite(LED_GREEN_PIN, on ? HIGH : LOW); }
void feedbackLedRed(bool on)   { digitalWrite(LED_RED_PIN,   on ? HIGH : LOW); }

void feedbackUpdate() {
    if (buzzerActive && millis() >= buzzerOffAt) {
        digitalWrite(BUZZER_PIN, LOW);
        buzzerActive = false;
    }
}
