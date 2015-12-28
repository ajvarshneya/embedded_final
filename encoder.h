/*
 * A.J. Varshneya
 * ajv4dg@virginia.edu
 * 10.26.15
 * encoder.h
 */

#include "init.h"
#include "debounce.h"

#ifndef ENCODER_H_
#define ENCODER_H_

// Encoder possible states
typedef enum {
    ALOW_BLOW, ALOW_BHIGH, AHIGH_BLOW, AHIGH_BHIGH, INIT
} EncoderState;

// Encoder definition
typedef struct {
    EncoderState state;
    SwitchDef switchA;
    SwitchDef switchB;
    SwitchDef reset;
    unsigned int count;
} EncoderDef;

void SendByteToDisplay(unsigned int number);
void SendDigitToDisplay(unsigned int digit);
EncoderState RefreshRotorState(EncoderDef * myEncoder);
void countCW(unsigned int * count);
void countCCW(unsigned int * count);

#endif /* ENCODER_H_ */
