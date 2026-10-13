#include <string.h>
#include <stdio.h>
#include "7188xa.h"
    

	
int Receive_Data_Length(int iPort,unsigned char* cInBuf, size_t iLength, long lTimeout);
void PrintHex(int port, char *label, unsigned char *buf, int len);



void main(void)
{
    int TxLength;
    int RxLength;
    int i;
	int counterQuery = 1;

    unsigned char TxData1[] = { 0x15, 0x08, 0x12, 0xE7, 0xC9 };
	//unsigned char TxData1[] = { 0x15, 0x0A, 0x84, 0x00, 0x00, 0x01, 0x00, 0xAE, 0xE5 };
	unsigned char TxData2[] = { 0x15, 0x08, 0x03, 0x27, 0xC5 };
	unsigned char TxData3[] = { 0x15, 0x08, 0x02, 0xE6, 0x05 };
	unsigned char TxData4[] = { 0x15, 0x08, 0x00, 0x67, 0xC4 };
	unsigned char TxData5[] = { 0x15, 0x08, 0x0B, 0x26, 0x03 }; 
	unsigned char TxData6[] = { 0x15, 0x08, 0x00, 0x62, 0xC4 };
	unsigned char TxData7[] = { 0x15, 0x0A, 0x84, 0x00, 0x01, 0x01, 0x00, 0xFF, 0x25 };
	unsigned char TxData8[] = { 0x15, 0x0A, 0x84, 0x00, 0x02, 0x01, 0x00, 0x0F, 0x25 };
	
    unsigned char RxData[100];

    InitLib();

    int PschCOMPort  = 2;
    int DebugCOMPort = 4;

    InstallCom(PschCOMPort,  9600, 8, 0, 1);
    InstallCom(DebugCOMPort, 115200, 8, 0, 1);

    ToComStr(DebugCOMPort, "Debug information: Start program\n\r");

    unsigned long escapeTimer = *TimeTicks;
	
    PrintHex(DebugCOMPort, "TX", TxData1, sizeof(TxData1));
			
    for (i = 0; i < sizeof(TxData1); ++i) // alter ToComBufn
        ToCom(PschCOMPort, TxData1[i]);
				
    while (1)
    {
        if (*TimeTicks - escapeTimer >= 15000)
            break;

        RxLength = Receive_Data_Length(PschCOMPort, RxData, sizeof(RxData), 500);

        if (RxLength > 0)
        {
            PrintHex(DebugCOMPort, "RX", RxData, RxLength);
			
			Delay(20);
			
			counterQuery++;
			
			switch(counterQuery)
			{
				case(2) :
				{
					 PrintHex(DebugCOMPort, "TX", TxData2, sizeof(TxData2));
					 
					for (i = 0; i < sizeof(TxData2); ++i) // alter ToComBufn
						ToCom(PschCOMPort, TxData2[i]);
					break;
				};
				
				case(3) :
				{
					 PrintHex(DebugCOMPort, "TX", TxData3, sizeof(TxData3));
					 
					for (i = 0; i < sizeof(TxData3); ++i) // alter ToComBufn
						ToCom(PschCOMPort, TxData3[i]);
					break;
				};
				
				case(4) :
				{
					 PrintHex(DebugCOMPort, "TX", TxData4, sizeof(TxData4));
					 
					for (i = 0; i < sizeof(TxData4); ++i) // alter ToComBufn
						ToCom(PschCOMPort, TxData4[i]);
					break;
				};
				
				case(5) :
				{
					 PrintHex(DebugCOMPort, "TX", TxData5, sizeof(TxData5));
					 
					for (i = 0; i < sizeof(TxData5); ++i) // alter ToComBufn
						ToCom(PschCOMPort, TxData5[i]);
					break;
				};
				
				case(6) :
				{
					 PrintHex(DebugCOMPort, "TX", TxData6, sizeof(TxData6));
					 
					for (i = 0; i < sizeof(TxData6); ++i) // alter ToComBufn
						ToCom(PschCOMPort, TxData6[i]);
					break;
				};
				
				case(7) :
				{
					 PrintHex(DebugCOMPort, "TX", TxData7, sizeof(TxData7));
					 
					for (i = 0; i < sizeof(TxData7); ++i) // alter ToComBufn
						ToCom(PschCOMPort, TxData7[i]);
					break;
				};
				
				case(8) :
				{
					 PrintHex(DebugCOMPort, "TX", TxData8, sizeof(TxData8));
					 
					for (i = 0; i < sizeof(TxData8); ++i) // alter ToComBufn
						ToCom(PschCOMPort, TxData8[i]);
					break;
				};

				//for (i = 0; i < RxLength; ++i) // alter ToComBufn
					//ToCom(ModemCOMPort, RxData[i]);
			}
        }
		else if(RxLength == 0)
		{
			ToComStr(DebugCOMPort, "RxLength == 0\n\r");
		}
		else
		{
			ToComStr(DebugCOMPort, "RxLength < 0\n\r");
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