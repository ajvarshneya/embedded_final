/*
 * A.J. Varshneya
 * ajv4dg@virginia.edu
 * 12.3.15
 */

#include "init.h"

// Efficient parity computation
unsigned long parity(unsigned long word) {
    // This will give us all the bits XOR'd (in the resulting LSB)
    unsigned long int x = word ^ (word >> 1);
    x = x ^ (x >> 2);
    x = x ^ (x >> 4);
    x = x ^ (x >> 8);
    x = x ^ (x >> 16);

    // return LSB
    return (x & 1);
}

unsigned long check_parity(unsigned long word) {
    unsigned long received_parity = parity(word & DATA_MASK);
    unsigned long sent_parity = (word & 0x80000000) >> 31;
    return (received_parity == sent_parity) ? 1 : 0;
}
