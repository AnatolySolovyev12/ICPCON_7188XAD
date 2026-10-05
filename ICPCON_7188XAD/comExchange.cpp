/* 
Slv_COM.c : Slave COM port demo for (request/reply) or (command/response) application.

Compiler: BC++ 3.1, 
          Turbo C ++ 1.01(3.01) (free from http://community.borland.com/museum)
          MSC 6.0,
          MSVC 1.52.
          
Compile mode: large

Project: Slv_COM.c
         ..\..\Lib\(8000E.Lib,7188XAL.Lib,7188XBL.Lib,7188XCL.Lib or 7188EL.Lib) 

Detail description:
    Slv_COM.c and Receive.c are a little diffent. One is non-blocked mode,
    one is blocked mode.
    
    PC sends commands to 7188/8000, and get back responses is 
    a very typical application.
    In this kind appliction, one COM port of 7188/8000 must 
    acts as a command decoder.
    This demo shows how to do this and how to use another COM port or LED 
    to show information to help you debug your program.

Hardware: 7188/8000

[May,13,2005] by Kevin
[31/Oct/2005] compiled by Liam
*/

#include <string.h>
#include <stdio.h>
#include "7188xa.h"   /* you must change this lib to suit the hardware that you want to use. */
                               
int Receive_Data(int iPort,unsigned char* cInBuf,char cTerminator,long lTimeout);
    // Uses COM port to receive data with a terminative char.
    // If time between two byte grater than lTimeout, abort receiving.

void main(void)
{
	int iLength;
	short waitAnswer = 0;
    int iQuit=0;
   
   // int iCommandCOMPort,iDebugCOMPort, iCommandSecondCOMPort;
    unsigned char cData[100] = "123456789";    
    
    InitLib();   

    int ModemCOMPort=1;
    int PschCOMPort=2;
	int iDebugCOMPort=4;
    
	InstallCom(ModemCOMPort,115200,8,0,1);	    
	InstallCom(PschCOMPort,115200,8,0,1);
	InstallCom(iDebugCOMPort,115200,8,0,1);
	
    ToComStr(iDebugCOMPort,"Debug information: Start program\n\r");

    while(!iQuit)
    {
        if(waitAnswer == 0)
		{
			iLength=Receive_Data(ModemCOMPort,cData,0xD,1000); /* Terminator is 0xD= CR, */
			//ToComStr(iDebugCOMPort,"Listen data from modem\n\r");
		}
		else
		{
			iLength=Receive_Data(PschCOMPort,cData,0xD,1000); /* Terminator is 0xD= CR, */
			//ToComStr(iDebugCOMPort,"Listen data from psch\n\r");
		}
  
		if(iLength > 0 || iLength == -1)
		{
			if(!strcmp(cData,"q"))
			{   
				ToComStr(iDebugCOMPort,"Debug information: Quit program\n\r");
				Delay(100); /* Wait for all data are transmited to COM port. */
				iQuit=1;
			}
			else
			{
                if(waitAnswer == 0)
				{
					printCom(iDebugCOMPort,"TX: %s\n\r",cData);
					printCom(PschCOMPort,"TX: %s\n\r",cData);
					ToComStr(PschCOMPort,cData);
					waitAnswer = 1;
				}
				else
				{
					printCom(iDebugCOMPort,"RX: %s\n\r",cData);
				    printCom(ModemCOMPort,"RX: %s\n\r",cData);
					ToComStr(ModemCOMPort,cData);
					waitAnswer = 0;
				}
			}
		}
    }
	RestoreCom(ModemCOMPort);
    RestoreCom(PschCOMPort);
    RestoreCom(iDebugCOMPort);
}
    
/*=====================================================================*/
/*==== Following functions are used to receive data from COM port.  ===*/
/*==== You can copy the functions to your own program.              ===*/
/*=====================================================================*/
/*
The following two functions are implemented by [non-block] method.

Non-Block method: After calling the function, if there is no data in the 
                  COM port input buffer, the CPU skip the function and 
                  go to execute next code.
*/
    
int Receive_Data(int iPort,unsigned char* cInBuf,char cTerminator,long lTimeout)
{	 
    /* 
    Uses COM port to receive data with a terminative char.
    
    iPort:    COM port number to receive data.
              0:COM0, 1:COM1, 2:COM2  .....
    *cInBuf:  Input buffer to receive data.
    cTerminator: what is the last byte ?
    lTimeout: timeout to receive data. (Unit: ms)
              The timeout is measured from last received byte 
              to the terminator.
    return: >0 :length of received data
             0 :doen't receive any data
            -1 :timeout
    */
    unsigned char cChar;
    int iIndex=0;
    unsigned long lStartTime;

    if(IsCom(iPort))
    {
        lStartTime=*TimeTicks;
        for(;;)
        {
            while(IsCom(iPort)) //check COM port
            {
                cChar=ReadCom(iPort);
                if(cChar==cTerminator) /* the terminal char is 0x0D */
                {
                    cInBuf[iIndex]=0;  /* Add the zero end to the data. */
                    return iIndex;     /* return data length                */
                                       /* (doesn't include the cTerminator) */
                }
                else
                    cInBuf[iIndex++]=cChar;
                    
                lStartTime=*TimeTicks;  /* refresh data timeout */
            }
            if((*TimeTicks-lStartTime)>=lTimeout)
            {
                cInBuf[iIndex]=0; /* Add the zero end to the data. */
                return -1;  /* receive data timeout */
            }
                
            RefreshWDT();
        }
    }
    else
        return 0;
}


int Receive_Data_Length(int iPort,unsigned char* cInBuf,int iLength,long lTimeout)
{
    /* 
    Uses COM port to receive string (fixed data length). 
    
    iPort:    COM port number to receive data.
              0:COM0, 1:COM1, 2:COM2  .....
    *cInBuf:  Input buffer to receive data.
    iLength:  how many bytes to receive?
    lTimeout: timeout to receive data. (Unit: ms)
              The timeout is measured from last received byte 
              to receive whole data.
    return: >0 :length of received data
             0 :doen't receive any data
            -1 :timeout
    */
    unsigned char cChar;
    int iIndex=0;
    unsigned long lStartTime;

    if(IsCom(iPort))
    {
        lStartTime=*TimeTicks;
        for(;;)
        {
            while(IsCom(iPort)) //check COM port
            {
                cInBuf[iIndex++]=ReadCom(iPort);
                if(iIndex>=iLength)
                {
                    cInBuf[iIndex]=0;
                    return iIndex;     /* return data length */
                }
                    
                lStartTime=*TimeTicks;  /* refresh data timeout */
            }
            if((*TimeTicks-lStartTime)>=lTimeout)
                return -1;  /* receive data timeout */
                
            RefreshWDT();
        }
    }
    else
        return 0;
}


