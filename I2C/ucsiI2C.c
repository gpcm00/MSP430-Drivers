/*
 * ucsiI2C.c
 *
 *  Created on: Feb 25, 2020
 *      Author: gianp
 */

#include <msp430.h>
#include "ucsiI2C.h"

volatile char i2cTxBuffer[I2C_MAX_BUF];         // TxBuffer is used by the txIsr
volatile char i2cRxBuffer[I2C_MAX_BUF];         // RxBuffer is used by the rxIsr
volatile int  i2cTxBufLen = 0;
volatile int  i2cRxBufLen = 0;

void ucsiB0I2CInit(char addrSize, int i2cClk)
{
    UCB0CTL1 |= UCSWRST|UCSSEL_3;                   // restart B0 and select SMCLK

    UCB0CTL0  = UCMODE_3|UCMST|(addrSize << 6);     // select i2c mode, make is a master and choose the right addr size

    UCB0BR1  |= i2cClk >> 8;                        // clk divider most significant byte
    UCB0BR0  |= i2cClk;                             // clk divider least significant byte

    P3SEL    |= I2C_SDA|I2C_SCL;                    // set P3.0 as the SDA and P3.1 as the CLK

    UCB0CTL1 &= ~UCSWRST;                           // start i2c

    UCB0IE   |= UCNACKIE;                           // turn on the the nack interrupt
}

int ucsiB0I2CTxChar(char* data, int bufLen, int addr)
{
    int success = -1;

    // check if the bus is busy before sending another byte
    if(!(UCB0STAT & UCBBUSY))
    {
        success = 0;                        // acknowledge that a transmit took place

        volatile int i;
        for(i = 0; i < bufLen; i++)
        {
            i2cTxBuffer[i] = data[i];       // place the bytes in the buffer
        }

        i2cTxBufLen = bufLen;               // set the length of the buffer

        UCB0I2CSA = addr;                   // set the slave addr

        UCB0CTL1 |= UCTR;                   // set master in transmit mode

        UCB0CTL1 |= UCTXSTT;                // start transaction

        UCB0IE   |= UCTXIE;                 // let the isr take care of the rest
    }

    return success;
}

int ucsiB0I2CRxChar(char* data, int bufLen, int addr)
{
    int success = -1;

    // check if the buffer is being used before initiating a receive
    if(!(UCB0STAT & UCBBUSY))
    {
        success = 0;

        i2cRxBuffer[0] = UCB0RXBUF;

        UCB0I2CSA = addr;

        UCB0CTL1 &= ~UCTR;

        UCB0CTL1 |= UCTXSTT;

        volatile unsigned int i;
        for(i = 0; i < bufLen; i++)
        {

            while(!(UCB0IFG & UCRXIFG));

            i2cRxBuffer[i] = UCB0RXBUF;

            data[i] = i2cRxBuffer[i];

        }

        UCB0CTL1 |=  UCTXSTP;
        UCB0IFG  &= ~UCRXIFG;
//*/
    }

    return success;
}

int ucsiB0I2CRxCharNoPoll(char** data, int bufLen, int addr)
{
    int success = -1;

    if(!(UCB0STAT & UCBBUSY))
    {
        success = 0;
        i2cRxBuffer[0] = UCB0RXBUF;

        *data = i2cRxBuffer;
        i2cRxBufLen = bufLen;

        UCB0I2CSA = addr;

        UCB0CTL1 &= ~UCTR;

        UCB0IFG &= ~UCRXIFG;

        UCB0CTL1 |= UCTXSTT;

        UCB0IE   |= UCRXIE;
    }

    return success;
}

volatile char* i2cGetRxAddr()
{
    return i2cRxBuffer;
}

#pragma vector = USCI_B0_VECTOR
__interrupt void ucsiB0Isr()
{
    // initialize all events
    static int txBufIdx     = 0;
    static int rxBufIdx     = 0;
    static int nackEvent    = 0;

    switch(__even_in_range(UCB0IV, 12))
    {
    case USCI_I2C_UCNACKIFG:
        nackEvent++;            // count the number of nack events

        // if nack event is less than 2 try again
        if( nackEvent < 2 )
        {
            UCB0CTL1 |=  UCTXSTT;
        }
        else
        {
            UCB0CTL1 |=  UCTXSTP;       // if more than two nack in a row, stop the transmission
            nackEvent = 0;
        }

        rxBufIdx = 0;
        txBufIdx = 0;

        break;
    // Ideally this is was planned with the system flag in mind, but I might as well just poll the rxBuf without an interrupt
        //*
    case USCI_I2C_UCRXIFG:

        i2cRxBuffer[rxBufIdx++] = UCB0RXBUF;

        // once the interrupt is equal to zero, stop receiving data and release the i2c bus
        if(!(i2cRxBufLen--))
        {

            UCB0CTL1 |=  UCTXSTP;
            UCB0IE   &= ~UCRXIE;

            // reset all events
            nackEvent =  0;
            txBufIdx  =  0;
            rxBufIdx  = -1;
        }

        break;
//*/

    case USCI_I2C_UCTXIFG:

        UCB0TXBUF = i2cTxBuffer[txBufIdx++];

        // once the interrupt is less than zero, stop transmitting data and release the i2c bus
        if(!(i2cTxBufLen--))
        {

            UCB0CTL1 |=  UCTXSTP;
            UCB0IE   &= ~UCTXIE;

            nackEvent = 0;
            txBufIdx = 0;
            rxBufIdx = 0;
        }

        break;

    }
}
