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

    unsigned char TxData[100];
    unsigned char RxData[100];

    InitLib();

    int ModemCOMPort = 1;
    int PschCOMPort  = 2;
    int DebugCOMPort = 4;

    InstallCom(ModemCOMPort, 115200, 8, 0, 1);
    InstallCom(PschCOMPort,  9600, 8, 0, 1);
    InstallCom(DebugCOMPort, 115200, 8, 0, 1);

    ToComStr(DebugCOMPort, "Debug information: Start program\n\r");

    unsigned long escapeTimer = *TimeTicks;

    while (1)
    {
        if (*TimeTicks - escapeTimer >= 20000)
            break;

        /* Modem -> PSCH */
        TxLength = Receive_Data_Length(ModemCOMPort, TxData, sizeof(TxData), 30);

        if (TxLength > 0 || TxLength == -1)
        {
            PrintHex(DebugCOMPort, "TX", TxData, TxLength);

            for (i = 0; i < TxLength; ++i) // alter ToComBufn
                ToCom(PschCOMPort, TxData[i]);
        }

        /* PSCH -> Modem */
        RxLength = Receive_Data_Length(PschCOMPort, RxData, sizeof(RxData), 30);

        if (RxLength > 0 || RxLength == -1)
        {
            PrintHex(DebugCOMPort, "RX", RxData, RxLength);

            for (i = 0; i < RxLength; ++i) // alter ToComBufn
                ToCom(ModemCOMPort, RxData[i]);
        }

        RefreshWDT();
    }

    RestoreCom(ModemCOMPort);
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