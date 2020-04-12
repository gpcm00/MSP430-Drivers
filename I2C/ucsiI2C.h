/**************************************************
 * Author:  Gian Moreira
 *
 * This is a driver for the I2C interface only
 **************************************************/

#ifndef UCSII2C_H_
#define UCSII2C_H_

#define I2C_SDA         BIT0
#define I2C_SCL         BIT1
#define I2C_MAX_BUF     50

/******************************************************************************************
 * Function:    ucsiB0I2CInit
 *
 * Description: - Initializes ucsiB0 register to operate in I2C mode
 *
 * Input:       - None
 * Outputs:     - None
 *
 * Returns: Nothing
 ******************************************************************************************/
void ucsiB0I2CInit(char addrSize, int i2cClk);

/******************************************************************************************
 * Function:    ucsiB0I2CTxChar
 *
 * Description: - Transmit an array of bytes through I2C to an given slave
 *
 * Input:       - data:     The array of bytes to be transmitted
 *              - bufLen:   Size of the array
 *              - addr:     The address of the slave
 * Outputs:     - None
 *
 * Returns: 0 if the bus is released, otherwise returns -1
 ******************************************************************************************/
int ucsiB0I2CTxChar(char* data, int bufLen, int addr);

/******************************************************************************************
 * Function:    ucsiB0I2CRxChar
 *
 * Description: - Receive an array of bytes through I2C by an given slave
 *
 * Input:       - data:     The array of bytes to be received
 *              - bufLen:   Size of the array
 *              - addr:     The address of the slave
 * Outputs:     - None
 *
 * Returns: 0 if the bus is released, otherwise returns -1
 ******************************************************************************************/
int ucsiB0I2CRxChar(char* data, int bufLen, int addr);

/******************************************************************************************
 * Function:    ucsiB0I2CRxCharNoPoll
 *
 * Description: - Receive an array of bytes through I2C by an given slave without polling
 *              the receiving data. The receiving data is handled by the ISR.
 *
 * Input:       - data:     The address of the array of bytes to be received
 *              - bufLen:   Size of the array
 *              - addr:     The address of the slave
 * Outputs:     - None
 *
 * Returns: 0 if the bus is released, otherwise returns -1
 ******************************************************************************************/
int ucsiB0I2CRxCharNoPoll(char** data, int bufLen, int addr);

/******************************************************************************************
 * Function:    i2cGetRxAddr
 *
 * Description: - This function will return an address to the I2C RxBuffer
 *
 * Input:       - None
 * Outputs:     - None
 *
 * Returns: The address of the I2C RxBuffer
 ******************************************************************************************/
volatile char* i2cGetRxAddr();




#endif /* UCSII2C_H_ */
