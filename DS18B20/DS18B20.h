/* DS18B20.h
 *
 * TS stands for Temperature Sensor (I used this prefix to avoid conflict with other drivers)
 *
 * A pull-down transistor must be attached to the bus and drive it low when a low signal is needed
 *
 * The 1-wire bus is attached to port P2.2 and it acts as the receiver
 *
 * The gate/base of the pull-down transistor is connected to P2.3 and it acts as the transmitter
 *
 * For more information, refer to the schematic
 *
 * All of the functions with a suffix _sS are for busses with only a single sensor in it
 * Do not attempt to call these function with multiple sensors on the bus as this will cause
 * the output of the sensor to be wired-ANDed.
 *
 * The truth table below represents the signal that will be on bus if two sensors are in the bus
 * and a function with the suffix _sS gets called:
 *                   ___________________________________________
 *                  |                         |                 |
 *                  | Logic level from sensor | Bus logic level |
 *                  |_________________________|_________________|
 *                  |            |            |                 |
 *                  |  Sensor_1  |  Sensor_2  |     Output      |
 *                  |____________|____________|_________________|
 *                  |            |            |                 |
 *                  |     0      |      0     |        0        |
 *                  |            |            |                 |
 *                  |     0      |      1     |        0        |
 *                  |            |            |                 |
 *                  |     1      |      0     |        0        |
 *                  |            |            |                 |
 *                  |     1      |      1     |        1        |
 *                  |____________|____________|_________________|
 *
 *  Created on: Feb 24, 2020
 *     Authors: Gian Moreira
 */


#ifndef DS18B20_H_
#define DS18B20_H_





// define input and output constants for the one-wire bus
#define TS_BUS          P2IN                        // P2.2 is the input port for the 1-wire bus
#define TS_OUT          P2OUT                       // P2.3 is driving an external pull-down transistor that will pull the bus low when needed

#define TS_INIT_INPORT  P2DIR
#define TS_INIT_OUTPORT P2DIR

#define TS_OUTBIT       BIT2                        // Transistor is being driven by P2.2
#define TS_INBIT        BIT3                        // 1-wire bus is directly connected to P2.3

#define TS_CYCLE_DELAY_W  18                        // amount of cycles needed to delay for the write bit
#define TS_CYCLE_DELAY_R  11                        // amount of cycles needed to delay for the read bit










#define TS_BUS_L        TS_OUT |=  TS_OUTBIT        // Since driving the bus low means writing a high to P2.2, this has been abstracted to simplify readability
#define TS_BUS_H        TS_OUT &= ~TS_OUTBIT        // For the same reasons as above, releasing the bus was also abstracted                                                   

#define TS_BUS_IS_LOW   !(TS_BUS&TS_INBIT)







// corresponding byte and its definition inside the scratchpad
#define TS_TEMP_LSB     0
#define TS_TEMP_MSB     1
#define TS_ALARM_HI     2
#define TS_ALARM_LO     3
#define TS_CONFIG       4
#define TS_CRC          8
// Note: bytes 5 to 7 are reserved









// for temperature resolution
#define TS_9BITS        0x1F
#define TS_10BITS       0x3F
#define TS_11BITS       0x5F
#define TS_12BITS       0x7F








// The MSP430F5529 SCLK is 1.048 MHz, so these macros define the amount of clk cycles needed to achieve a certain amount of time
#define TS_15us         16                              
#define TS_30us         31                             
#define TS_45us         47                              
#define TS_60us         63                              
#define TS_480us        503                             

#define TS_RST_DELAY    (TS_480us - 3)                  // Needs three less cycles since it takes a couple of clk cycles to drive the outputs
#define TS_RST_SAMPLE   210                             // Safe amount of clock cycles to sample whether the slave responded after a reset






// ROM commands
#define SEARCH_ROM      0xF0
#define READ_ROM        0x33
#define MATCH_ROM       0x55
#define SKIP_ROM        0xCC
#define ALARM_SEARCH    0xEC

// Function commands
#define CONVERT_T       0x44
#define WRITE_SPAD      0x4E        // write scratchpad command
#define READ_SPAD       0xBE        // read scratchpad command
#define COPY_SPAD       0x48        // copy scratchpad command
#define RECALL_E2       0xB8
#define READ_PSUPPLY    0xB4        // read power supply command











/* DS18B20 structure that can keep track of the scratch pad, temperature

 * if being used with multiple sensors in the bus, then the address must be declared unless the
   search ROM command was called                                                                    */
typedef struct DS18B20
{
    char addr[8];
    char scrPad[9];
    int  temp;
} DS18B20;








/**********************************************************************************************
 * Function:    tsWriteByte
 *
 * Description: - Sends 1 byte of data through the 1-wire bus as described by Maxim Integrated
 *              - This function doesn't include the initial reset signal
 *
 * Input:       - byte => the byte transmitted
 *
 * Output:      - None
 *
 * Return:      - Returns a the value of byte if all bytes were transmitted successfully without
 *              interference
 **********************************************************************************************/
char tsWriteByte(char byte);

/**********************************************************************************************
 * Function:    tsReadData
 *
 * Description: - Reads n-bytes of data through the 1-wire bus as described by Maxim Integrated
 *
 * Input:       - bufLen => the amount of bytes being received
 *
 * Output:      - byte   => an array of bytes with the input data
 *
 * Return:      - Returns the address of the first byte
 **********************************************************************************************/
char* tsReadData(char* byte, int bufLen);

/**********************************************************************************************
 * Function:    tsReadBit
 *
 * Description: - Reads 1 bit from the 1-wire bus
 *
 * Input:       - None
 *
 * Output:      - None
 *
 * Return:      - Returns a 1 if the bus was high, returns a 0 if the bus was low
 **********************************************************************************************/
int tsReadBit();

/**********************************************************************************************
 * Function:    tsWriteBit
 *
 * Description: - Writes 1 bit at a time to the 1-wire bus
 *
 * Input:       - polarity  => whether it should write a 0 or a 1
 *
 * Output:      - None
 *
 * Return:      - Nothing
 **********************************************************************************************/
void tsWriteBit(char polarity);

/**********************************************************************************************
 * Function:    tsInit
 *
 * Description: - Initializes P2.2 as the input port for the 1-wire bus and P2.3 as the output
 *              bit that will drive the external pull-down transistor.
 *
 * Input:       - None
 *
 * Output:      - None
 *
 * Return:      - Nothing
 **********************************************************************************************/
void tsInit();

/**********************************************************************************************
 * Function:    tsWrite
 *
 * Description: - Sends 1 byte of data through the 1-wire bus as described by Maxim Integrated
 *              - This function includes the initial reset signal
 *
 * Input:       - byte => the byte transmitted
 *
 * Output:      - None
 *
 * Return:      - Returns a the value of byte if all bytes were transmitted successfully without
 *              interference
 **********************************************************************************************/
int tsWrite(char byte);

/**********************************************************************************************
 * Function:    tsMstRst
 *
 * Description: - Sends a reset signal through the 1-wire bus as described by Maxim Integrated
 *
 * Input:       - None
 *
 * Output:      - None
 *
 * Return:      - Returns a 0 if a response from the slave is detected
 *              - Returns a -1 if the slave doesn't respond
 **********************************************************************************************/
int tsMstRst();

/**********************************************************************************************
 * Function:    tsGetAddr
 *
 * Description: - Get the address of the sensor on the bus
 *              - This function is a substitute for the search address function because this is
 *                a lot faster
 *              - ATTENTION: DO NOT USE MULTIPLE SENSORS ON THE BUS WHEN CALLING THIS FUNCTION!
 *
 * Input:       - None
 *
 * Output:      - sensor    => a structure that contains all data for the sensor
 *
 * Return:      - Returns a 0 if the sensor was detected, and a -1 if no sensor was detected
 **********************************************************************************************/
int tsGetAddr(DS18B20* sensor);

/**********************************************************************************************
 * Function:    tsMatchAddr
 *
 * Description: - Sends the 8 bytes of the ROM code through the bus
 *
 * Input:       - sensor    => a structure that contains all data for the sensor
 *
 * Output:      - None
 *
 * Return:      - Nothing
 **********************************************************************************************/
void tsMatchAddr(DS18B20 sensor);

/**********************************************************************************************
 * Function:    tsConvertTemp
 *
 * Description: - Sends the convert temperature command to all sensors in the bus at the same
 *              time
 *
 * Input:       - None
 *
 * Output:      - None
 *
 * Return:      - Nothing
 **********************************************************************************************/
void tsConvertTemp();

/**********************************************************************************************
 * Function:    tsValidateData
 *
 * Description: - Decodes the CRC register to validate the scratch pad
 *
 * Input:       - sensor    => a structure that contains all data for the sensor
 *
 * Output:      - None
 *
 * Return:      - Returns a 0 if the scratch pad was valid, and a -1 if invalid
 **********************************************************************************************/
int tsValidateData(DS18B20 sensor);

/**********************************************************************************************
 * Function:    tsReadTemp
 *
 * Description: - Converts the current temperature from the BS18D20 and stores it in the sensor
 *
 * Input:       - None
 *
 * Output:      - sensor   => a structure that contains all the data from the sensor
 *
 * Return:      - Returns a 0 if temperature was converted and the second reset signal was valid
 *                otherwise returns a -1 if it fails
 **********************************************************************************************/
int tsReadTemp(DS18B20* sensor);
int tsReadTemp_sS(DS18B20* sensor);

/**********************************************************************************************
 * Function:    tsReadSPad
 *
 * Description: - Reads the scratchpad of the sensor
 *
 * Input:       - None
 *
 * Output:      - sensor    => a structure that contains all data for the sensor
 *
 * Return:      - Returns a 0 if the sensor was detected, and a -1 if no sensor was detected
 **********************************************************************************************/
int tsReadSPad(DS18B20* sensor);
int tsReadSPad_sS(DS18B20* sensor);

/**********************************************************************************************
 * Function:    tsWriteSpad
 *
 * Description: - Writes the alarm high triger and low trigger, as well as configuring the
 *              resolution of the sensor
 *
 * Input:       - sensor    => a structure that contains all data for the sensor
 *              - alarmHi   => the high temperature trigger
 *              - alarmLo   => the low temperature trigger
 *              - config    => the resolution that of the temperature
 *
 * Output:      - sensor    => a structure that contains all data for the sensor
 *
 * Return:      - Returns a 0 if the sensor was detected, and a -1 if no sensor was detected
 **********************************************************************************************/
int tsWriteSpad(DS18B20* sensor, char alarmHi, char alarmLo, char config);
int tsWriteSpad_sS(DS18B20* sensor, char alarmHi, char alarmLo, char config);

/**********************************************************************************************
 * Function:    tsConfig
 *
 * Description: - configures the resolution of the sensor
 *
 * Input:       - sensor    => a structure that contains all data for the sensor
 *              - config    => the resolution that of the temperature
 *
 * Output:      - sensor    => a structure that contains all data for the sensor
 *
 * Return:      - Returns a 0 if the sensor was detected, and a -1 if no sensor was detected
 **********************************************************************************************/
int tsConfig(DS18B20* sensor, char config);
int tsConfig_sS(DS18B20* sensor, char config);

/**********************************************************************************************
 * Function:    tsSetAlarm
 *
 * Description: - Writes the alarm high trigger and low trigger
 *
 * Input:       - sensor    => a structure that contains all data for the sensor
 *              - alarmHi   => the high temperature trigger
 *              - alarmLo   => the low temperature trigger
 *
 * Output:      - sensor    => a structure that contains all data for the sensor
 *
 * Return:      - Returns a 0 if the sensor was detected, and a -1 if no sensor was detected
 **********************************************************************************************/
int tsSetAlarm(DS18B20* sensor, char alarmHi, char alarmLo);
int tsSetAlarm_sS(DS18B20* sensor, char alarmHi, char alarmLo);

/**********************************************************************************************
 * Function:    tsCopySpad
 *
 * Description: - Copies the value of the alarm High, Low and configuration register into the
 *                internal EEPROM so the DS18B20 is properly setup at power up
 *
 * Input:       - sensor    => a structure that contains all data for the sensor
 *
 * Output:      - None
 *
 * Return:      - Returns a 0 if the scratch pad was valid, and a -1 if invalid
 **********************************************************************************************/
int tsCopySpad(DS18B20 sensor);
int tsCopySpad_sS(DS18B20 sensor);

#endif /* DS18B20_H_ */
