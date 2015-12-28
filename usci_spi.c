#include "usci_spi.h"

/*
 * USCI_A0_SPI_Init()
 * 
 * USCI_A0 initialization and reset for SPI mode. See Section 16.3.1 of users' guide.
 * 
 */
void SPI_Init()
{	
	/* Note that the SPI functionality is being implemented in software 
	 * (that is, "bit banging").  As such, the USCI module is not being used.
	 * If the USCI SPI is used, see function, USCI_A0_SPI_Init().
	 */		
	SET_USCIA0_SIMO_AS_AN_OUTPUT;	// Set SPI SIMO DDR to output
	SET_USCIA0_SOMI_AS_AN_INPUT;	// Set SPI SOMI DDR to input
	INITIALIZE_SPI_SCK;				// Initialize SPI clock to LOW
	SET_SPI_SCK_AS_AN_OUTPUT;		// SPI slave clock is an output
}

void SPISendByte(unsigned char byte_value)
{
	short k;
		
	// Shift the bits, beginning with the MSB
	for (k = 7; k >= 0; k--){
		
		// Right-shift byte_value k places so that the LSB
		// contains the kth bit. Then, mask off the LSB
		// and assign it to the SIMO port pin.
		if ((byte_value >> k) & 0x01) {
			WRITE_LOGIC_1_TO_SLAVE;
		}
		else {
			WRITE_LOGIC_0_TO_SLAVE;
		}
		
		// Toggle SPI Clock: (HIGH XOR 1) -> LOW, and (LOW XOR 1) -> HIGH
		TOGGLE_SPI_SCK; TOGGLE_SPI_SCK; 
	}

}

unsigned char SPIReceiveByte()
{
	short k;
	unsigned char receive_value = 0;
		
	// Shift the bits, beginning with the LSB
	for (k = 7; k >= 0; k--){
		
		// Left-shift the value on SOMI k places, and then OR the
		// resulting value with the current value of receive_value.
		receive_value |= (READ_BIT_FROM_SLAVE << k);
		
		// Toggle SPI Clock: (HIGH XOR 1) -> LOW, and (LOW XOR 1) -> HIGH
		TOGGLE_SPI_SCK; TOGGLE_SPI_SCK;
	}
	return receive_value;
}


/*
 * USCI_A0_SPI_Init()
 * 
 * USCI_A0 initialization and reset for SPI mode. See Section 16.3.1 of users' guide.
 * 
 */
void USCI_A0_SPI_Init()
{
	// Software reset enabled. USCI logic held in reset state.
	UCA0CTL1 = UCSWRST;

    // Select USCI SPI functionality for port pins.
#ifdef __useful
	//B8_1(P1SEL2) = HIGH; B8_2(P1SEL2) = HIGH;
	//B8_2(P1SEL) = HIGH; B8_1(P1SEL) = HIGH;
#else
	//P1SEL2 |= (BIT2|BIT1); P1SEL |= (BIT2|BIT1);
#endif
		
	//UCA0CTL0 = (UCMODE1+UCSYNC); // Initialize control register 0.
	/* UCCKPH = 0x80 Sync. Mode: Clock Phase
	 * UCCKPL = 0x40 Sync. Mode: Clock Polarity
	 * UCMSB = 0x20 Sync. Mode: MSB first  0:LSB / 1:MSB
	 * UC7BIT = 0x10 Sync. Mode: Data Bits  0:8-bits / 1:7-bits
	 * UCMST = 0x08 Sync. Mode: Master Select
	 * UCMODE1 = 0x04 Sync. Mode: USCI Mode 1
	 * UCMODE0 = 0x02 Sync. Mode: USCI Mode 0
	 *  00 = 3-Pin SPI
	 *  01 = 4-pin SPI with UCA0 Slave Transmit Enable (STE) is active high
	 *  10 = 4-pin SPI with UCA0 Slave Transmit Enalbe (STE) is active low
	 *  11 = I2C mode
	 * UCSYNC = 0x01 Sync-Mode  0:UART-Mode / 1:SPI-Mode
	 */
	
	// Software reset disabled. USCI logic released for operation.
    UCA0CTL1 &= ~UCSWRST;
}
