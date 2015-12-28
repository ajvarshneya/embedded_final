/*
 * A.J. Varshneya
 * ajv4dg@virginia.edu
 * 10.26.15
 * encoder.c
 */

#include "encoder.h"
#include "debounce.h"
#include "init.h"
#include "usci_spi.h"
#include "tx.h"
#include "parity.h"

// Sends a byte to the display
void SendByteToDisplay(unsigned int number) {
    #ifdef TX_ENABLE
    // Update t_data
    my_data.data_buffer = number;
    my_data.data_buffer &= DATA_MASK;
    my_data.data_buffer |= (parity(my_data.data_buffer) << 31);
    #endif

    // Get most/least significant digit
    unsigned int msb = number / 16;
    unsigned int lsb = number % 16;

    // Send associated opcodes
    SendDigitToDisplay(lsb);
    SendDigitToDisplay(msb);

    // Latch
    P2OUT |= XLAT;
    P2OUT &= ~XLAT;
}

void SendDigitToDisplay(unsigned int digit) {
    // Select what opcode to send according to input
    switch(digit) {
        case 0:
            SPISendByte(0x3f);
            break;
        case 1:
            SPISendByte(0x06);
            break;
        case 2:
            SPISendByte(0x5b);
            break;
        case 3:
            SPISendByte(0x4f);
            break;
        case 4:
            SPISendByte(0x66);
            break;
        case 5:
            SPISendByte(0x6d);
            break;
        case 6:
            SPISendByte(0x7d);
            break;
        case 7:
            SPISendByte(0x07);
            break;
        case 8:
            SPISendByte(0x7f);
            break;
        case 9:
            SPISendByte(0x67);
            break;
        case 10:
            SPISendByte(0x77);
            break;
        case 11:
            SPISendByte(0x7c);
            break;
        case 12:
            SPISendByte(0x39);
            break;
        case 13:
            SPISendByte(0x5e);
            break;
        case 14:
            SPISendByte(0x79);
            break;
        case 15:
            SPISendByte(0x71);
            break;
    }
}

// Maintains state machine for rotary encoder
EncoderState RefreshRotorState(EncoderDef * myEncoder) {
    // Debounce the switches
    Debounce(&(myEncoder->switchA));
    Debounce(&(myEncoder->switchB));
    Debounce(&(myEncoder->reset));

    switch(myEncoder->state) {
        case INIT:
            // Go to any of the states depending on current switch values
            if(GetSwitch(&(myEncoder->switchA)) == LOW && GetSwitch(&(myEncoder->switchB)) == LOW) {
                myEncoder->state = ALOW_BLOW;
            }
            if(GetSwitch(&(myEncoder->switchA)) == LOW && GetSwitch(&(myEncoder->switchB)) == HIGH) {
                myEncoder->state = ALOW_BHIGH;
            }
            if(GetSwitch(&(myEncoder->switchA)) == HIGH && GetSwitch(&(myEncoder->switchB)) == LOW) {
                myEncoder->state = AHIGH_BLOW;
            }
            if(GetSwitch(&(myEncoder->switchA)) == HIGH && GetSwitch(&(myEncoder->switchB)) == HIGH) {
                myEncoder->state = AHIGH_BHIGH;
            }
            break;

        case ALOW_BLOW:
            // Go to ALOW_BHIGH if B is HIGH (CCW)
            if(GetSwitch(&(myEncoder->switchB)) == HIGH) {
                myEncoder->state = ALOW_BHIGH;
                countCCW(&(myEncoder->count));
                break;
            }
            // Go to AHIGH_BLOW if A is HIGH (CW)
            if(GetSwitch(&(myEncoder->switchA)) == HIGH) {
                myEncoder->state = AHIGH_BLOW;
                countCW(&(myEncoder->count));
            }
            break;

        case AHIGH_BLOW:
            // Go to ALOW_BLOW if A is LOW (CCW)
            if(GetSwitch(&(myEncoder->switchA)) == LOW) {
                myEncoder->state = ALOW_BLOW;
                countCCW(&(myEncoder->count));
                break;
            }
            // Go to AHIGH_BHIGH if B is HIGH (CW)
            if(GetSwitch(&(myEncoder->switchB)) == HIGH) {
                myEncoder->state = AHIGH_BHIGH;
                countCW(&(myEncoder->count));
            }
            break;

        case AHIGH_BHIGH:
            // Go to AHIGH_BLOW if B is LOW (CCW)
            if(GetSwitch(&(myEncoder->switchB)) == LOW) {
                myEncoder->state = AHIGH_BLOW;
                countCCW(&(myEncoder->count));
                break;
            }
            // Go to ALOW_BHIGH if A is LOW (CW)
            if(GetSwitch(&(myEncoder->switchA)) == LOW) {
                myEncoder->state = ALOW_BHIGH;
                countCW(&(myEncoder->count));
            }
            break;

        case ALOW_BHIGH:
            // Go to AHIGH_BHIGH if A is HIGH
            if(GetSwitch(&(myEncoder->switchA)) == HIGH) {
                myEncoder->state = AHIGH_BHIGH;
                countCCW(&(myEncoder->count));
                break;
            }
            // Go to ALOW_BLOW if B is LOW
            if(GetSwitch(&(myEncoder->switchB)) == LOW) {
                myEncoder->state = ALOW_BLOW;
                countCW(&(myEncoder->count));
            }
            break;

        default:
            myEncoder->state = INIT;
    }

    // Reset, go to initialize
    if (GetSwitch(&(myEncoder->reset)) == LOW) {
        myEncoder->state = INIT;
        myEncoder->count = 128;
    }

    // Update the display with current count
    SendByteToDisplay(myEncoder->count);

    // Update PWM duty cycle with current count
    TACCR1 = (int) ((myEncoder->count) << 2);

    return myEncoder->state;
}

void countCW(unsigned int * count) {
// Counts clockwise, handles upper limit
    ++(*count);
    if (*count > 250) {
        *count = 0;
    }
}

// Counts counterclockwise, handles lower limit
void countCCW(unsigned int * count) {
    --(*count);
    if (*count > 250) {
        *count = 250;
    }
}
