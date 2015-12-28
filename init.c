/*
 * A.J. Varshneya
 * ajv4dg@virginia.edu
 * 11.17.15
 */

#include <msp430.h>
#include "init.h"
#include "parity.h"
#include "usci_spi.h"

#ifdef TX_ENABLE
#include "tx.h"
#endif

#ifdef RX_ENABLE
#include "rx.h"
#endif

/*
 * Initialize all the things
 */
void init(void) {
    // Stop watchdog timer
    WDTCTL = WDTHOLD + WDTPW;

    init_hardware();
    SPI_Init();
    init_variables();

    // Enable global interrupts
    _EINT();
}

/*
 * Initialize ports, clock, timers
 */
void init_hardware(void) {
    // Initialize ports

    init_ports();

    // Clock/timer initialization
    init_basic_clock_system();
    init_timer0_a3();
    init_timer1_a3();
}

void init_ports(void) {
    P1DIR |= CNTRL1;

    #ifdef TX_ENABLE
    init_transmit_ports();
    #endif

    #ifdef RX_ENABLE
    init_receive_ports();
    #endif

    init_encoder_ports();
    init_display_ports();
}

#ifdef TX_ENABLE
void init_transmit_ports(void) {
    //P1DIR |= TX_DATA;
    P2DIR |= TX_DATA;
    P2SEL &= ~TX_DATA;
    P2SEL2 &= ~TX_DATA;

    P1OUT &= ~CNTRL1; // LOW --> Transmit
}
#endif

#ifdef RX_ENABLE
void init_receive_ports(void) {
    P2DIR &= ~RX_DATA;
    P1OUT |= CNTRL1; // HIGH --> Receive
    P2SEL |= RX_DATA;
    P2SEL2 &= ~RX_DATA;
}
#endif

void init_encoder_ports(void) {
    // Make LEDs outputs
    P1DIR |= RedLED;
//    P1DIR |= GreenLED;

    // Reset button
    P1DIR &= ~RESET; // Set button to active low
    P1REN |= RESET; // Set button pullup/pulldown resistor
    P1OUT |= RESET; // Set pullup/pulldown resistor to pull up

    // Switches
    P2DIR &= ~SWITCH_A;
    P2REN |= SWITCH_A;
    P2OUT |= SWITCH_A;

    P2DIR &= ~SWITCH_B;
    P2REN |= SWITCH_B;
    P2OUT |= SWITCH_B;
}

void init_display_ports(void) {
    // Latch/Blank setup
    P2DIR |= XLAT;
    P2SEL2 &= ~XLAT;
    P2SEL &= ~XLAT;

    P1DIR |= BLANK;
    P1OUT &= ~BLANK;
}

/*
 *  Initializes MSP430 Basic Clock System
 */
void init_basic_clock_system(void)
{
    /*
     * Basic Clock System Control 1
     *
     * XT2OFF       : Disable XT2CLK
     * ~XTS         : Low Frequency
     * DIVA_0       : Divide by 1
     */
    BCSCTL1 |= XT2OFF + DIVA_0;

    /*
     * Basic Clock System Control 2
     *
     * SELM_0       : DCOCLK
     * DIVM_0       : Divide by 1
     * ~SELS        : DCOCLK
     * DIVS_0       : Divide by 1
     * ~DCOR        : DCO uses internal resistor
     */
    BCSCTL2 = SELM_0 + DIVM_0 + DIVS_0;

    // Not sure what this does
    if (CALBC1_8MHZ != 0xFF) {

        // Adjust according to VCC rise time
        __delay_cycles(100000);

        // Clear all DCOx and MODx bits, apply RSELx values, apply new DCOx and MODx values.
        DCOCTL = 0x00;

        // Set DCO to 8 MHz
        BCSCTL1 = CALBC1_8MHZ;
        DCOCTL = CALDCO_8MHZ;
    }

    /*
     * Basic Clock System Control 3
     *
     * XT2S_0       : 0.4 - 1 MHz
     * LFXT1S_2     : If XTS = 0, XT1 = VLOCLK ; If XTS = 1, XT1 = 3 - 16-MHz crystal or resonator
     * XCAP_1       : ~6 pF
     */
    BCSCTL3 = XT2S_0 + LFXT1S_2 + XCAP_1;
}

/*
 *  Initialize MSP430 Timer0_A3
 */
void init_timer0_a3(void)
{
     /*
     * TA0CCTL0, Capture/Compare Control Register 0
     *
     * CM_0         : No Capture
     * CCIS_0       : CCIxA
     * ~SCS         : Asynchronous Capture
     * ~SCCI        : Latched capture signal (read)
     * ~CAP         : Compare mode
     * OUTMOD_0     : PWM output mode: 0 - OUT bit value
     */
    TA0CCTL0 = CM_0 + CCIS_0 + OUTMOD_0 + CCIE;

    /* TA0CCR0, Timer0_A3 Capture/Compare Register 0 */
    TA0CCR0 = 499;

    /*
     * TA0CTL, Timer0_A3 Control Register
     *
     * TASSEL_2     : SMCLK
     * ID_3         : Divider - /8
     * MC_1         : Up Mode
     */
    TA0CTL = TASSEL_2 + ID_3 + MC_1;
}

/*
 *  Initialize MSP430 Timer1_A3
 */
void init_timer1_a3(void)
{
    /*
     * TA1CCTL0, Capture/Compare Control Register 0
     *
     * CM_1         : Rising Edge
     * CCIS_0       : CCIxA
     * SCS          : Sychronous Capture
     * ~SCCI        : Latched capture signal (read)
     * CAP          : Capture mode
     * OUTMOD_0     : PWM output mode: 0 - OUT bit value
     */
    TA1CCTL0 = CM_1 + CCIS_0 + SCS + CAP + OUTMOD_0 + CCIE;

    /*
     * TA1CCTL1, Capture/Compare Control Register 1
     *
     * CM_2         : Falling Edge
     * CCIS_0       : CCIxA
     * SCS          : Sychronous Capture
     * ~SCCI        : Latched capture signal (read)
     * CAP          : Capture mode
     * OUTMOD_0     : PWM output mode: 0 - OUT bit value
     */
    TA1CCTL1 = CM_2 + CCIS_0 + SCS + CAP + OUTMOD_0 + CCIE;

    /*
     * TA1CCTL2, Capture/Compare Control Register 2
     *
     * CM_2         : Falling Edge
     * CCIS_0       : CCIxA
     * SCS          : Sychronous Capture
     * ~SCCI        : Latched capture signal (read)
     * CAP          : Capture mode
     * OUTMOD_0     : PWM output mode: 0 - OUT bit value
     */
    TA1CCTL2 = CM_2 + CCIS_0 + SCS + CAP + OUTMOD_0;

    /*
     * TA1CTL, Timer1_A3 Control Register
     *
     * TASSEL_2     : SMCLK
     * ID_3         : Divider - /8
     * MC_2         : Continuous Mode
     */
    TA1CTL = TASSEL_2 + ID_3 + MC_2;
}

/*
 * Initialize transmit/receive structures
 */
void init_variables(void){
    init_encoder_variables();

    #ifdef TX_ENABLE
    init_transmit_variables();
    #endif

    #ifdef RX_ENABLE
    init_receive_variables();
    #endif
}

/*
 * Encoder variables helper method
 */
void init_encoder_variables(void) {
    // Switch A initialization
    gEncoder.switchA.controlState = DB_HIGH;
    gEncoder.switchA.currentValidState = HIGH;
    gEncoder.switchA.switchPort = (char*) &(P2IN);
    gEncoder.switchA.switchPin = BIT4;
    gEncoder.switchA.timestamp = 0;

    // Switch B initialization
    gEncoder.switchB.controlState = DB_HIGH;
    gEncoder.switchB.currentValidState = HIGH;
    gEncoder.switchB.switchPort = (char*) &(P2IN);
    gEncoder.switchB.switchPin = BIT3;
    gEncoder.switchB.timestamp = 0;

    // Reset (push button) initialization
    gEncoder.reset.controlState = DB_HIGH;
    gEncoder.reset.currentValidState = HIGH;
    gEncoder.reset.switchPort = (char*) &(P1IN);
    gEncoder.reset.switchPin = BIT3;
    gEncoder.reset.timestamp = 0;

    // Encoder state/count initialization
    gEncoder.state = INIT;
    gEncoder.count = 128;

    // Other variables (timer) initialization
    gTimeout = 0;
    gTimer = 0;
    g500msTimer = 0;
    gButtonPresses = 0;
}

/*
 * Transmit variables helper method
 */
#ifdef TX_ENABLE
void init_transmit_variables(void) {
    my_data.bits_remaining = TRANSMISSION_SIZE;

    // Set data/buffer
    my_data.data_buffer = DATA & DATA_MASK;
    my_data.data = DATA & DATA_MASK;

    // Set parity bit
    my_data.data |= (parity(DATA & DATA_MASK) << 31);
    my_data.data_buffer |= (parity(DATA & DATA_MASK) << 31);

    my_data.clock_phase = MIDBIT;
    my_data.state = START_BIT;

    // Timer stuff
    txTimeout = 0;
}
#endif

/*
* Receive variables helper method
*/
#ifdef RX_ENABLE
void init_receive_variables(void) {
    Receiver_Events.Get_Index = 0;
    Receiver_Events.Put_index = 0;
    Receiver_Events.QueSize   = 0;

    Rcv1.CurrentRcvState = Initial_Expect_Rising;
    Rcv1.CurrentRecvdData = 0;
    Rcv1.FallingEdgeTimeStamp = 0;
    Rcv1.RisingEdgeTimeStamp = 0;
    Rcv1.MidBitTimeStamp = 0;
    Rcv1.PulseWidth = 0;
    Rcv1.CurrentRecvdData = 0;
    Rcv1.LastValidReceived = 0;
}
#endif
