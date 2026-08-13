#ifndef FEEDBACK_H
#define FEEDBACK_H

#include <Arduino.h>
#include "config.h"

void feedbackInit();
void feedbackBeepSuccess();
void feedbackBeepError();
void feedbackLedGreen(bool on);
void feedbackLedRed(bool on);
void feedbackUpdate();

#endif // FEEDBACK_H
