/*
 * A.J. Varshneya
 * ajv4dg@virginia.edu
 * 11.17.15
 */

#include <msp430.h>
#include "init.h"
#include "tx.h"
#include "parity.h"

// Manages transmission, called every 500 us by periodic interrupt
void transmit(TransmitData* t_data) {

    // LOW is midbit, HIGH is not
    t_data->clock_phase = (t_data->clock_phase == MIDBIT) ? FULLBIT : MIDBIT;
    enum TransmitClockPhase phase = t_data->clock_phase;

    // Indicates whether to transition to TRANSMIT
    static unsigned int transition_flag = 0;

    // State machine to manage manchester coding
    switch(t_data->state) {

        // Start sequence (1 ms)
        case START_BIT:
            if (phase == FULLBIT) {
                if (transition_flag) {
                    // 1 ms elapsed
                    transition_flag = 0;
                    t_data->data = t_data->data_buffer;
                    t_data->state = TRANSMIT;
                    TX_LOW
                } else {
                    // 0 ms elapsed
                    transition_flag = 1;
                    TX_HIGH
                }
            }
            break;

        // Transmit data
        case TRANSMIT:
            switch(phase) {
                // Midbit
                case MIDBIT:
                    // Set LOW to prepare to send 1, HIGH for 0
                    if (t_data->data & MSB_MASK) {
                        TX_LOW
                    } else {
                        TX_HIGH
                    }
                    break;

                // Fullbit
                case FULLBIT:
                    if (t_data->bits_remaining > 0) {
                        // Set HIGH to send 1, LOW for 0
                        if (t_data->data & MSB_MASK) {
                            TX_HIGH
                        } else {
                            TX_LOW
                        }

                        // Use next bit
                        t_data->data <<= 1;
                        --(t_data->bits_remaining);
                    }
                    // Transmitted all bits, go to INTERWORD
                    else {
                        t_data->bits_remaining = TRANSMISSION_SIZE;
                        t_data->state = INTERWORD;
                        txTimeout = 0;

                        TX_LOW
                    }
                    break;
            }
            break;

        // Interword delay 10 ms
        case INTERWORD:
            if (txTimeout >= INTERWORD_DELAY) {
                txTimeout = 0;
                t_data->state = START_BIT;
            }

            break;
        default:
            t_data->state = START_BIT;
            // Other initialization (?)
            break;

    }
}

