/*
 *  This file is derived from HAL.cpp of the Repetier-Firmware.
 *  This I2C/TWI code is used in place of that in the Arduino library
 *  so that DMA may be used.
 *
 *  And this isn't quite a proper library.  It might be generalized
 *  by passing in "Twi *pTwi" and then use that rather than the #define'd
 *  TWI_INTERFACE.  Likewise, the init routine might have SDA and SCL
 *  parameters passed in.
 *
 *  Repetier-Firmware is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Repetier-Firmware is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Repetier-Firmware.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "Arduino.h"
#include "I2C.h"
#include "Pins.h"

#if TWI_ID == ID_TWI0
#define TWI_INTERFACE TWI0
#define TWI_SDA       PIN_WIRE1_SDA 
#define TWI_SCL       PIN_WIRE1_SCL
#elif TWI_ID == ID_TWI1
#define TWI_INTERFACE TWI1
#define TWI_SDA       PIN_WIRE_SDA
#define TWI_SCL       PIN_WIRE_SCL
#else
#error TWI_ID is not defined
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*************************************************************************
 Initialization of the I2C bus interface. Need to be called only once
*************************************************************************/
void i2cInit(unsigned long clockSpeedHz)
{
    uint32_t dwCkDiv, dwClDiv;

    // enable TWI
    pmc_enable_periph_clk(TWI_ID);

    // Configure pins
    PIO_Configure(g_APinDescription[TWI_SDA].pPort,
                  g_APinDescription[TWI_SDA].ulPinType,
                  g_APinDescription[TWI_SDA].ulPin,
                  g_APinDescription[TWI_SDA].ulPinConfiguration);
    PIO_Configure(g_APinDescription[TWI_SCL].pPort,
                  g_APinDescription[TWI_SCL].ulPinType,
                  g_APinDescription[TWI_SCL].ulPin,
                  g_APinDescription[TWI_SCL].ulPinConfiguration);

    // Set to Master mode with known state
    TWI_INTERFACE->TWI_CR = TWI_CR_SVEN;
    TWI_INTERFACE->TWI_CR = TWI_CR_SWRST;
    TWI_INTERFACE->TWI_RHR;
	// Repetier attempts to assign TWI_IMR which is verbotten: it's read-only
    // TWI_INTERFACE->TWI_IMR = 0;

    TWI_INTERFACE->TWI_CR = TWI_CR_SVDIS;
    TWI_INTERFACE->TWI_CR = TWI_CR_MSDIS;
    TWI_INTERFACE->TWI_CR = TWI_CR_MSEN;

    // Set i2c clock rate
	// Repetier fails to initialize dwClDiv.  And since stack variables
	// are not guaranteed to be initialized to zero automatically, Repetier's
	// version of this code may set the wrong I2C speed.
	dwClDiv = 0;
	dwCkDiv = 0;
    while (dwClDiv == 0)
    {
        dwClDiv = ((F_CPU / (2 * clockSpeedHz)) - 4) / (1<<dwCkDiv);

        if ( dwClDiv > 255 )
        {
            dwCkDiv++;
            dwClDiv = 0;
        }
    }
    TWI_INTERFACE->TWI_CWGR = 0;
    TWI_INTERFACE->TWI_CWGR = (dwCkDiv << 16) | (dwClDiv << 8) | dwClDiv;
}


/*************************************************************************
  Issues a start condition and sends address and transfer direction.
  return 0 = device accessible, 1= failed to access device
*************************************************************************/
unsigned char i2cStart(unsigned char address_and_direction)
{
    uint32_t twiDirection = address_and_direction & 1;
    uint32_t address = address_and_direction >> 1;

    TWI_INTERFACE->TWI_CR = TWI_CR_MSEN | TWI_CR_SVDIS;

    // set master mode register with no internal address
    TWI_INTERFACE->TWI_MMR = 0;
    TWI_INTERFACE->TWI_MMR = (twiDirection << 12) | TWI_MMR_IADRSZ_NONE | TWI_MMR_DADR(address);

    // returning readiness to send/recieve not device accessibility
    // return value not used in code anyway
    return !(TWI_INTERFACE->TWI_SR & TWI_SR_TXCOMP);
}


/*************************************************************************
 Issues a start condition and sends address and transfer direction.
 If device is busy, use ack polling to wait until device is ready

 Input:   address and transfer direction of I2C device
*************************************************************************/
void i2cStartWait(unsigned char address_and_direction)
{
	uint32_t twiDirection = address_and_direction & 1;
    uint32_t address = address_and_direction >> 1;

    while (!(TWI_INTERFACE->TWI_SR & TWI_SR_TXCOMP));

    // set to master mode
    TWI_INTERFACE->TWI_CR = TWI_CR_MSEN | TWI_CR_SVDIS;

    // set master mode register with no internal address
    TWI_INTERFACE->TWI_MMR = 0;
    TWI_INTERFACE->TWI_MMR = (twiDirection << 12) | TWI_MMR_IADRSZ_NONE | TWI_MMR_DADR(address);
}


/*************************************************************************
 Issues a start condition and sends address and transfer direction.
 Also specifies internal address of device

 Input:   address and transfer direction of I2C device, internal address
*************************************************************************/
void i2cStartAddr(unsigned char address_and_direction, unsigned int pos)
{
    uint32_t twiDirection = address_and_direction & 1;
    uint32_t address = address_and_direction >> 1;

    // set master mode register with internal address
    TWI_INTERFACE->TWI_MMR = 0;
    TWI_INTERFACE->TWI_MMR = (twiDirection << 12) | TWI_MMR_DADR(address);

    // write internal address register
    TWI_INTERFACE->TWI_IADR = 0;
    TWI_INTERFACE->TWI_IADR = TWI_IADR_IADR(pos);
}


/*************************************************************************
 Terminates the data transfer and releases the I2C bus
*************************************************************************/
void i2cStop(void)
{
    i2cTxFinished();
    TWI_INTERFACE->TWI_CR = TWI_CR_STOP;
    i2cCompleted();
}


/*************************************************************************
 Signal start of data transfer
*************************************************************************/
void i2cStartBit(void)
{
    TWI_INTERFACE->TWI_CR = TWI_CR_START;
}


/*************************************************************************
 Wait for transaction to complete
*************************************************************************/
void i2cCompleted(void)
{
    while (!((TWI_INTERFACE->TWI_SR & TWI_SR_TXCOMP) == TWI_SR_TXCOMP));
}


/*************************************************************************
 Wait for transmission to complete
*************************************************************************/
void i2cTxFinished(void)
{
    while ((TWI_INTERFACE->TWI_SR & TWI_SR_TXRDY) != TWI_SR_TXRDY);
}


/*************************************************************************
  Send one byte to I2C device

  Input:    byte to be transfered
  Return:   0 write successful
            1 write failed
*************************************************************************/
unsigned char i2cWrite(uint8_t data)
{
	unsigned char result;

	i2cWriting(data);
	TWI_INTERFACE->TWI_CR = TWI_CR_STOP;
	i2cTxFinished();
	result = (TWI_INTERFACE->TWI_SR & TWI_SR_NACK) == TWI_SR_NACK;
	i2cCompleted();
	return result;
}


/*************************************************************************
  Send one byte to I2C device
  Transaction can continue with more writes or reads
************************************************************************/
void i2cWriting(uint8_t data)
{
    TWI_INTERFACE->TWI_THR = data;
}


/*************************************************************************
 Read one byte from the I2C device, request more data from device
 Return:  byte read from I2C device
*************************************************************************/
unsigned char i2cReadAck(void)
{
    while ((TWI_INTERFACE->TWI_SR & TWI_SR_RXRDY) != TWI_SR_RXRDY);
    return TWI_INTERFACE->TWI_RHR;
}


/*************************************************************************
 Read one byte from the I2C device, read is followed by a stop condition

 Return:  byte read from I2C device
*************************************************************************/
unsigned char i2cReadNak(void)
{
	unsigned char data;
    TWI_INTERFACE->TWI_CR = TWI_CR_STOP;

    while ((TWI_INTERFACE->TWI_SR & TWI_SR_RXRDY) != TWI_SR_RXRDY);
	data = i2cReadAck();
    i2cCompleted();
    return data;
}

#ifdef __cplusplus
}
#endif
