#include <string.h>
#include <stdio.h>
#include "7188xa.h"
    
	 // %d (или %i) — для знакового int
	 // %u - беззнаковый int
	 // %c - для одного символа char
	 // %s - массив символов
	 // %ld — для знакового long
	 // %lu — для беззнакового unsigned long
	
int Receive_Data_Length(int iPort,unsigned char* cInBuf, size_t iLength, long lTimeout);
void PrintHex(int port, char *label, unsigned char *buf, int len);



void main(void)
{
    int TxLength;
    int RxLength;
    int i;
	int counterQuery = 1;

    unsigned char TxData1[] = { 0x15, 0x08, 0x12, 0xE7, 0xC9 };
	unsigned char TxData2[] = { 0x15, 0x01, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xCF, 0x17 };
	unsigned char TxData3[] = { 0x15, 0x0A, 0x84, 0x00, 0x01, 0x01, 0x00, 0xFF, 0x25 }; // Т1
	unsigned char TxData4[] = { 0x15, 0x0A, 0x84, 0x00, 0x02, 0x01, 0x00, 0x0F, 0x25 }; // Т2
		
	/*Print("TEST with print TxData1  = %d\r\n", TxData1); //////////////////
	Delay(500);
	Print("u = %u\r\n", TxData1); //////////////////
	Delay(500);
	Print("s = %s\r\n", TxData1); //////////////////
	Delay(500);
	for(int ii = 0; ii < sizeof(TxData1); ++ii)
	{
		Print("c = %c   ", TxData1[ii]); //////////////////
	    Print("d = %d", TxData1[ii]); //////////////////
		Print("\r\n");
	}
	Delay(500);
	Print("\r\n");
	Delay(500);
	*/
	
    unsigned char RxData[100];

    InitLib();

    int PschCOMPort  = 2;
    int DebugCOMPort = 4;

    InstallCom(PschCOMPort,  9600, 8, 0, 1);
    InstallCom(DebugCOMPort, 115200, 8, 0, 1);

    ToComStr(DebugCOMPort, "Debug information: Start program\n\r");

    unsigned long escapeTimer = *TimeTicks;
	
	char millisFirst[15];

	sprintf(millisFirst, "TX(%lu)", *TimeTicks);
	
    PrintHex(DebugCOMPort, millisFirst, TxData1, sizeof(TxData1));
		
	ToComBufn(PschCOMPort, TxData1, sizeof(TxData1));
	WaitTransmitOver(PschCOMPort);
			
    while (1)
    {
        if (*TimeTicks - escapeTimer >= 15000)
            break;

        RxLength = Receive_Data_Length(PschCOMPort, RxData, sizeof(RxData), 100);

        if (RxLength > 0)
        {
			char millisRx[15];

			sprintf(millisRx, "RX(%lu)", *TimeTicks);
			
            PrintHex(DebugCOMPort, millisRx, RxData, RxLength);
			
			Delay(50);
			
			if(counterQuery == 3)
			{
				char decVal[4];
				char hexVal[4];
				
				for(int exchange = 1; exchange < 5; ++exchange)
					hexVal[exchange] = RxData[exchange];

				sprintf(decVal, "TEST(%4lu)", hexVal);
				
				 ToComStr(DebugCOMPort, decVal);
			}
			
			counterQuery++;
			
			char millis[15];

			sprintf(millis, "TX(%lu)", *TimeTicks);
						
			switch(counterQuery)
			{
				case(1) :
				{
					PrintHex(DebugCOMPort, millis, TxData1, sizeof(TxData1));
		
					ToComBufn(PschCOMPort, TxData1, sizeof(TxData1));
	                WaitTransmitOver(PschCOMPort);
					
					break;
				};
				
				case(2) :
				{
					PrintHex(DebugCOMPort, millis, TxData2, sizeof(TxData2));
					 
					ToComBufn(PschCOMPort, TxData2, sizeof(TxData2));
	                WaitTransmitOver(PschCOMPort);
					
					break;
				};
				
				case(3) :
				{
					PrintHex(DebugCOMPort, millis, TxData3, sizeof(TxData3));
					 
					ToComBufn(PschCOMPort, TxData3, sizeof(TxData3));
	                WaitTransmitOver(PschCOMPort);
					
					break;
				};
				
				case(4) :
				{
					PrintHex(DebugCOMPort, millis, TxData4, sizeof(TxData4));
					 
					ToComBufn(PschCOMPort, TxData4, sizeof(TxData4));
	                WaitTransmitOver(PschCOMPort);
					
					break;
				};
				
			}
        }
        RefreshWDT();
    }

    RestoreCom(PschCOMPort);
    RestoreCom(DebugCOMPort);
}
    

    
int Receive_Data_Length(int iPort, unsigned char* cInBuf, size_t iLength, long lTimeout)
{
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
                cInBuf[iIndex++]=ReadCom(iPort); // alter ReadComn
				
                if(iIndex>=iLength)
                {
                    cInBuf[iIndex]=0;
                    return iIndex;     /* return data length */
                }
                    
                lStartTime=*TimeTicks;  /* refresh data timeout */
            }
			
            if((*TimeTicks-lStartTime)>=lTimeout)
                return iIndex;  /* receive data timeout */
                
            RefreshWDT();
        }
    }
    else
        return 0;
}



void PrintHex(int port, char *label, unsigned char *buf, int len)
{
	if(len > 0)
	{
		int i;

		ToComStr(port, label);
		ToComStr(port, ": ");

		for (i = 0; i < len; ++i)
		{
			char hexBuf[10];

			sprintf(hexBuf, "%02X ", buf[i]);
			
/*
% — начало спецификатора формата.
0 — флаг: заполнять поле нулями слева, если значение короче указанной ширины.
2 — минимальная ширина поля: минимум 2 символа.
X — тип: беззнаковое целое в шестнадцатеричном виде, буквы A–F заглавные.
*/
			ToComStr(port, hexBuf);
		}
	}
	else
		ToComStr(port, "Len is <= 0\n\r");

    ToComStr(port, "\n\r");
}