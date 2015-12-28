/*
 * A.J. Varshneya
 * ajv4dg@virginia.edu
 * 11.17.15
 */

#ifndef INIT_H
#define INIT_H

#include "debounce.h"
#include "encoder.h"

/*
 * Enable / disable transmit/receive
 */
//#define TX_ENABLE
#define RX_ENABLE

/*
 * Constants
 */

// Set the data you want to send here
#define DATA 0x1DEDBEEF

// Ensures that we don't make mistakes with parity bit
#define DATA_MASK 0x7FFFFFFF

#define CNTRL1 BIT6
#define TRANSMISSION_SIZE 32

// Encoder constants

// Definitions for debounce time
#define MAKEDEBOUNCETIME 5
#define RELEASEBOUNCETIME 5

// LED definitions
#define RedLED BIT0
#define GreenLED BIT6
#define RESET BIT3
#define NUMSWITCHES 1

// LED Display Definitions
#define SIN BIT7
#define SCLK BIT5
#define XLAT BIT2
#define BLANK BIT4
#define SWITCH_A BIT3
#define SWITCH_B BIT4

/*
 * Global variables
 */

unsigned int txTimeout;

// Incremented by interrupt handler
// Decremented by software call in main loop
unsigned int gTimeout;

// Incremented in ManageSoftwareTimers()
unsigned int gTimer;
unsigned int g500msTimer;

// Number of button presses from all switches
unsigned int gButtonPresses;

SwitchDef gPushButton;

EncoderDef gEncoder;

/*
 * Function prototypes
 */
void init(void);
void init_hardware(void);

void init_ports(void);
void init_transmit_ports(void);
void init_receive_ports(void);
void init_encoder_ports(void);
void init_display_ports(void);

void init_variables(void);
void init_encoder_variables(void);
void init_transmit_variables(void);
void init_receive_variables(void);

void init_basic_clock_system(void);
void init_timer0_a3(void);
void init_timer1_a3(void);

#endif /* INIT_H_ */
