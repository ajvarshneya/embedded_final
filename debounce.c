/*
 * A.J. Varshneya
 * ajv4dg@virginia.edu
 * 10.26.15
 * debounce.c
 */

#include "debounce.h"
#include "init.h"

// Returns High if the switch is high, Low if the switch is low
SwitchState GetSwitch(SwitchDef *mySwitch) {
    mySwitch->currentSwitchReading = (*(mySwitch->switchPort) & mySwitch->switchPin) ? HIGH : LOW;
    return mySwitch->currentSwitchReading;
}

// Contains state machine logic
SwitchState Debounce(SwitchDef *mySwitch) {

    switch (mySwitch->controlState) {
        // LOW state, waiting for HIGH signal
        case DB_LOW:
            // If button HIGH, go to next state
            if (GetSwitch(mySwitch) == HIGH) {
                mySwitch->controlState = DB_VAL_HIGH;
                mySwitch->timestamp = gTimer;
            }
        break;

        case DB_VAL_HIGH:
            if (GetSwitch(mySwitch) == LOW) {
                // If button is LOW, return to previous state
                mySwitch->controlState = DB_LOW;
                mySwitch->timestamp = gTimer;
            } else if ((gTimer - mySwitch->timestamp) > MAKEDEBOUNCETIME) {
                // If button is HIGH and its been long enough, go to next state
                mySwitch->controlState = DB_HIGH;
                gButtonPresses++;

                // Update internal state
                mySwitch->currentValidState = HIGH;
                mySwitch->timestamp = gTimer;
            }
        break;

        case DB_HIGH:
            // If button LOW, go to next state
            if (GetSwitch(mySwitch) == LOW) {
                mySwitch->controlState = DB_VAL_LOW;
                mySwitch->timestamp = gTimer;
            }
        break;

        case DB_VAL_LOW:
            if (GetSwitch(mySwitch) == HIGH) {
                // If button is High, return to previous state
                mySwitch->controlState = DB_HIGH;
                mySwitch->timestamp = gTimer;
            } else if ((gTimer - mySwitch->timestamp) > RELEASEBOUNCETIME) {
                // If button is LOW and its been long enough, go to next state
                mySwitch->controlState = DB_LOW;

                // Update internal state
                mySwitch->currentValidState = LOW;
                mySwitch->timestamp = gTimer;

                // Flip the red LED here
                //P1OUT ^= RedLED;
            }
        break;

       // default: Switch->ControlState = DbExpectHigh;
    }

    return mySwitch->currentValidState;
}

// Manages various timers that I use
void ManageSoftwareTimers(void) {
    // Updates timers with timeout
    if (gTimeout) {
        gTimer += gTimeout;
    }

    // Flip green LED every 500 ms
    if (gTimer > 1000) {
        gTimer = 0;
        g500msTimer++;
        //P1OUT ^= GreenLED;
    }

    // Reinit ports every 10 sec
    if (g500msTimer > 20) {
        g500msTimer = 0;
        init_encoder_ports();
    }

    // Reset timeout
    gTimeout = 0;
}
