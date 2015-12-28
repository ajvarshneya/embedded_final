/*
 * A.J. Varshneya
 * ajv4dg@virginia.edu
 * 11.17.15
 */

/*
 * Manchester phase encoder transceiver
 * May transmit any number of bits up to 32
 * Protocol details:
 * - Separate start bit sent with rising edge at mid-bit (?)
 * - Normal transmission is sent
 * - Rising edge at mid-bit --> 1
 * - Falling edge at mid-bit --> 0
 * - 1 ms/bit transmission rate
 * - Inter-transmission time (period) of 50 ms
 * - 500 us interrupt required
 */

/*
 * - To run, comment/uncomment the TX_ENABLE and RX_ENABLE
 * macros in 'init.h' to transmit/receive respectively.
 * - Change the DATA macro as desired to send different messages.
 */

#include <msp430.h>
#include "init.h"
#include "debounce.h"
#include "encoder.h"

#ifdef TX_ENABLE
#include "tx.h"
#endif

#ifdef RX_ENABLE
#include "rx.h"
#endif

void main(void) {
    // Initialization
    init();

    // Main loop
    while (1) {
        #ifdef TX_ENABLE
        RefreshRotorState(&gEncoder);
        #endif

        ManageSoftwareTimers();

        #ifdef RX_ENABLE
            rcv(); // Call receiver
        #endif
    }
}

/*
 * Timer0_A0 interrupt
 */
#pragma vector=TIMER0_A0_VECTOR
__interrupt void periodic_interrupt(void){

    #ifdef TX_ENABLE
        txTimeout++;
        gTimeout++;
        transmit(&my_data);
    #endif

    // ~ Receiver Enabled ~
    #ifdef RX_ENABLE
    #endif
}

/*
 *  Timer1_A0 interrupt
 */
#pragma vector=TIMER1_A0_VECTOR
__interrupt void rising_edge_interrupt(void){

    #ifdef RX_ENABLE
        risingedge();
    #endif
}

/*
 *  Timer1_A1 interrupt
 */
#pragma vector=TIMER1_A1_VECTOR
__interrupt void falling_edge_interrupt(void){
    // Efficient switch
    switch (__even_in_range(TA1IV, TA1IV_TAIFG))
    {
        case TA1IV_TACCR1:
            #ifdef RX_ENABLE
                fallingedge();
            #endif
            break;

        case TA1IV_TACCR2:
            break;

        case TA1IV_TAIFG:
            break;
    }
}
