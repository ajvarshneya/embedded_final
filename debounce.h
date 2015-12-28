/*
 * A.J. Varshneya
 * ajv4dg@virginia.edu
 * 10.26.15
 * debounce.h
 */

#ifndef DEBOUNCE_H_
#define DEBOUNCE_H_

#include <msp430.h>

/* Function Prototypes */

// Debouncing state
typedef enum {
    DB_LOW, DB_VAL_HIGH, DB_HIGH, DB_VAL_LOW
} DbState;

// Switch state
typedef enum {
    HIGH, LOW
} SwitchState;

// Switch definition
typedef struct {
    DbState controlState;
    SwitchState currentSwitchReading; // Instantaneous state (for GetSwitch)
    SwitchState currentValidState;    // Debounced state (for state machine)
    char* switchPort;
    char switchPin;

    // Timestamp of last switch
    unsigned int timestamp;
} SwitchDef;

// This should deal with software timers off of your 1 ms global counter.
void ManageSoftwareTimers(void);

// Returns state of selected switch
SwitchState GetSwitch(SwitchDef *mySwitch);

// Debounces switch
SwitchState Debounce(SwitchDef *mySwitch);

#endif /* DEBOUNCE_H_ */
