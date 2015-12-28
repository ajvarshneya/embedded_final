#ifndef USCI_SPI_H
#define USCI_SPI_H

#include <msp430.h>

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * USCI A0 Slave-Out-Master-In (SOMI) 
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * GPIO      :  P1.1
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
#ifdef __useful
#define USCIA0_SOMI_PIN					B8_1(P1IN)
#define USCIA0_SOMI_DDR					B8_1(P1DIR)
#define READ_BIT_FROM_SLAVE				((unsigned char) USCIA0_SOMI_PIN)
#define SET_USCIA0_SOMI_AS_AN_INPUT		USCIA0_SOMI_DDR = LOW
#else
#define USCIA0_SOMI_BIT					BIT1
#define USCIA0_SOMI_PORT				P1IN
#define USCIA0_SOMI_DDR					P1DIR
#define READ_BIT_FROM_SLAVE				((unsigned char) (USCIA0_SOMI_PORT & USCIA0_SOMI_BIT) >> 1)
#define SET_USCIA0_SOMI_AS_AN_INPUT		USCIA0_SOMI_DDR &= ~USCIA0_SOMI_BIT
#endif
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * USCI A0 Slave-In-Master-Out (SIMO) 
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * GPIO      :  P1.2
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
#ifdef __useful
#define USCIA0_SIMO_PIN					B8_2(P1OUT)
#define USCIA0_SIMO_DDR 				B8_2(P1DIR)
#define WRITE_LOGIC_1_TO_SLAVE			USCIA0_SIMO_PIN = HIGH
#define WRITE_LOGIC_0_TO_SLAVE			USCIA0_SIMO_PIN = LOW
#define SET_USCIA0_SIMO_AS_AN_OUTPUT	USCIA0_SIMO_DDR = HIGH
#else
#define USCIA0_SIMO_BIT					BIT7
#define USCIA0_SIMO_PORT				P1OUT
#define USCIA0_SIMO_DDR 				P1DIR
#define WRITE_LOGIC_1_TO_SLAVE			USCIA0_SIMO_PORT |= USCIA0_SIMO_BIT
#define WRITE_LOGIC_0_TO_SLAVE			USCIA0_SIMO_PORT &= ~USCIA0_SIMO_BIT
#define SET_USCIA0_SIMO_AS_AN_OUTPUT	USCIA0_SIMO_DDR |= USCIA0_SIMO_BIT
#endif

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Slave Clock for GPIO Flash Memory Board
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * GPIO      :  P1.4
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
#ifdef __useful
#define SPI_SCK_PIN					B8_4(P1OUT) // SPI Slave Clock output
#define SPI_SCK_DDR					B8_4(P1DIR) // SPI Slave Clock direction
#define SET_SPI_SCK_AS_AN_OUTPUT	SPI_SCK_DDR = HIGH
#define INITIALIZE_SPI_SCK			SPI_SCK_PIN = LOW
#define TOGGLE_SPI_SCK				SPI_SCK_PIN ^= 1
#else
#define SPI_SCK_BIT					BIT5
#define SPI_SCK_PORT				P1OUT // SPI Slave Clock output
#define SPI_SCK_DDR					P1DIR // SPI Slave Clock direction
#define SET_SPI_SCK_AS_AN_OUTPUT	SPI_SCK_DDR |= SPI_SCK_BIT
#define INITIALIZE_SPI_SCK			SPI_SCK_PORT &= ~SPI_SCK_BIT
#define TOGGLE_SPI_SCK				SPI_SCK_PORT ^= SPI_SCK_BIT
#endif

void SPI_Init();
void SPISendByte(unsigned char byte_value);
unsigned char SPIReceiveByte();
void USCI_A0_SPI_Init();
#endif
