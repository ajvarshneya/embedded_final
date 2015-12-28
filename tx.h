/*
 * A.J. Varshneya
 * ajv4dg@virginia.edu
 * 11.17.15
 */

#ifndef TX_H
#define TX_H

// How long to wait between transmissions in milliseconds
#define INTERWORD_DELAY 20
#define TX_DATA BIT5
#define MSB_MASK 0x80000000
#define TX_HIGH P2OUT |= TX_DATA;
#define TX_LOW P2OUT &= ~TX_DATA;

/*
 * Type definitions
 */
enum TransmitStates{START_BIT, TRANSMIT, INTERWORD};
enum TransmitClockPhase{FULLBIT, MIDBIT};

typedef struct {
    // The data to be transmitted
    unsigned long data;

    // Update data with this value when you want to transmit
    // Change this at any time to update the next value in data without affecting current routine
    unsigned long data_buffer;

    // Number of bits remaining in current transmission
    unsigned int bits_remaining;

    // Updated every 500 us (half-bite period) [but what does it do?]
    enum TransmitClockPhase clock_phase;

    // State of the transmitter
    enum TransmitStates state;
} TransmitData;

TransmitData my_data;

/*
 * Function prototypes
 */
void transmit(TransmitData * t_data) ; //This routine is called every 500 uS by an interrupt handler.

#endif /* TX_H */
