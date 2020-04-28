/*
 * DS18B20.c
 *
 *  Created on: Feb 27, 2020
 *     Authors: Gian Moreira
 */

#include <msp430.h>
#include "DS18B20.h"




// most of these functions follow the flow chart given in the datasheet



void tsInit()
{
    TS_BUS_H;                       // release the bus
    TS_INIT_INPORT  &= ~TS_INBIT;
    TS_INIT_OUTPORT |=  TS_OUTBIT;
}





int tsWrite(char byte)
{
    if(!tsMstRst())
        return tsWriteByte(byte);
    else return -1;
}





int tsMstRst()
{
    // initital reset pulse
     TS_BUS_L;
    __delay_cycles(TS_RST_DELAY);
    TS_BUS_H;
    __delay_cycles(TS_60us);

/*	if the sensor sends a feedback acknowledging the
	initial reset pulse, wait a bit and return
	that the function was a success, otherwise
	return that there was no feedback from the
	sensor	 */

    if(TS_BUS_IS_LOW)
    {
        __delay_cycles(TS_RST_DELAY);
        return 0;
    }
    else return -1;
}








int tsGetAddr(DS18B20* sensor)
{


/*  This is the fastest way to get the addr, but this only works for
    bus with a single wire. I recommend using this function one sensor
    at a time to get the address instead of using the search sensor function   */


    if(!tsMstRst())
    {
        tsWriteByte(READ_ROM);
        tsReadData(sensor->addr, 8);

        return 0;
    }
    else return -1;

}







void tsMatchAddr(DS18B20 sensor)
{
    volatile int i;

    tsWriteByte(MATCH_ROM);

    for(i = 0; i < 8 ; i++)
        tsWriteByte(sensor.addr[i]);
}







void tsConvertTemp()
{


// This function can be used to address all sensors and command them to convert temperature


    if(!tsMstRst())
    {
        tsWriteByte(SKIP_ROM);
        tsWriteByte(CONVERT_T);
    }

}






int tsValidateData(DS18B20 sensor)
{
    char shiftRegister = 0;
    char input;
    int  i, j;

    for(i = 0; i < 9; i++)
    {
        for(j = 0; j < 8; j++)
        {
            input = (sensor.scrPad[i]>>j) ^ shiftRegister;
            if(input&BIT0)
            {
                shiftRegister >>= 1;
                shiftRegister  |= BIT7;
                shiftRegister  ^= BIT2|BIT3;

            }
            else
            {
                shiftRegister >>= 1;
            }

        }
    }

    return (shiftRegister == 0 ? 0 : -1);
}







int tsReadTemp(DS18B20* sensor)
{
    // first, convert the temperature
    if(!tsMstRst())
    {
        tsMatchAddr(*sensor);

        tsWriteByte(CONVERT_T);

        while(!tsReadBit());        // wait for temperature conversion

        // then read the scratch pad
        if(!tsMstRst())
        {
            tsMatchAddr(*sensor);

            tsWriteByte(READ_SPAD);
            tsReadData(sensor->scrPad, 9);

            sensor->temp  = 0;

            sensor->temp |= sensor->scrPad[0];
            sensor->temp |= sensor->scrPad[1]<<8;

            return 0;
        }
        else return -1;
    }
    else return -1;
}

int tsReadTemp_sS(DS18B20* sensor)
{

    if(!tsMstRst())
    {
        tsWriteByte(SKIP_ROM);

        tsWriteByte(CONVERT_T);
        while(!tsReadBit());        // wait for temperature conversion

        if(!tsMstRst())
        {

            tsWriteByte(READ_ROM);
            tsReadData(sensor->addr, 8);

            tsWriteByte(READ_SPAD);
            tsReadData(sensor->scrPad, 9);

            sensor->temp  = 0;

            sensor->temp |= sensor->scrPad[0];
            sensor->temp |= sensor->scrPad[1]<<8;

            return 0;
        }
        else return -1;
    }
    else return -1;

}







int tsReadSPad(DS18B20* sensor)
{
    if(!tsMstRst())
    {
        tsMatchAddr(*sensor);

        tsWriteByte(READ_SPAD);
        tsReadData(sensor->scrPad, 9);

        sensor->temp  = 0;

        sensor->temp |= sensor->scrPad[0];
        sensor->temp |= sensor->scrPad[1]<<8;

        return 0;
    }
    else return -1;
}

int tsReadSPad_sS(DS18B20* sensor)
{
    volatile int i;
    if(!tsMstRst())
    {
        tsWriteByte(SKIP_ROM);

        tsWriteByte(READ_SPAD);
        tsReadData(sensor->scrPad, 9);

        sensor->temp  = 0;

        sensor->temp |= sensor->scrPad[0];
        sensor->temp |= sensor->scrPad[1]<<8;

        return 0;
    }
    else return -1;
}








int tsWriteSpad(DS18B20* sensor, char alarmHi, char alarmLo, char config)
{
    if(!tsMstRst())
    {
        tsMatchAddr(*sensor);

        tsWriteByte(WRITE_SPAD);
        sensor->scrPad[TS_ALARM_HI] = alarmHi;
        tsWriteByte(alarmHi);
        sensor->scrPad[TS_ALARM_LO] = alarmLo;
        tsWriteByte(alarmLo);
        sensor->scrPad[TS_CONFIG] = config;
        tsWriteByte(config);

        return 0;
    }
    else return -1;
}

int tsWriteSpad_sS(DS18B20* sensor, char alarmHi, char alarmLo, char config)
{
    volatile int i;
    if(!tsMstRst())
    {
        tsWriteByte(SKIP_ROM);

        tsWriteByte(WRITE_SPAD);
        sensor->scrPad[TS_ALARM_HI] = alarmHi;
        tsWriteByte(alarmHi);
        sensor->scrPad[TS_ALARM_LO] = alarmLo;
        tsWriteByte(alarmLo);
        sensor->scrPad[TS_CONFIG] = config;
        tsWriteByte(config);

        return 0;
    }
    else return -1;
}








int tsConfig(DS18B20* sensor, char config)
{
    if(!tsReadSPad(sensor))
    {
        if( !tsWriteSpad(sensor, sensor->scrPad[TS_ALARM_HI], sensor->scrPad[TS_ALARM_LO], config) )
        {
            sensor->scrPad[TS_CONFIG] = config;
            return 0;
        }
        else return -1;
    }
    else return -1;
}

int tsConfig_sS(DS18B20* sensor, char config)
{
    if(!tsReadSPad_sS(sensor))
    {
        if( !tsWriteSpad_sS(sensor, sensor->scrPad[TS_ALARM_HI], sensor->scrPad[TS_ALARM_LO], config) )
        {
            sensor->scrPad[TS_CONFIG] = config;
            return 0;
        }
        else return -1;
    }
    else return -1;
}








int tsSetAlarm(DS18B20* sensor, char alarmHi, char alarmLo)
{
    if(!tsReadSPad(sensor))
    {
        if( !tsWriteSpad(sensor, alarmHi, alarmLo, sensor->scrPad[TS_CONFIG]) )
            return 0;
        else return -1;
    }
    else return -1;
}

int tsSetAlarm_sS(DS18B20* sensor, char alarmHi, char alarmLo)
{
    if(!tsReadSPad_sS(sensor))
    {
        if( !tsWriteSpad_sS(sensor, alarmHi, alarmLo, sensor->scrPad[TS_CONFIG]) )
            return 0;
        else return -1;
    }
    else return -1;
}








int tsCopySpad(DS18B20 sensor)
{
    if(!tsMstRst())
        {
            tsMatchAddr(sensor);
            tsWriteByte(COPY_SPAD);

            while(!tsReadBit());

            return 0;
        }
    else return -1;
}

int tsCopySpad_sS(DS18B20 sensor)
{
    if(!tsMstRst())
        {
            tsWriteByte(SKIP_ROM);
            tsWriteByte(COPY_SPAD);

            while(!tsReadBit());

            return 0;
        }
    else return -1;
}
