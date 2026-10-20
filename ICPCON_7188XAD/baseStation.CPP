#include <string.h>
#include <stdio.h>
#include "7188xa.h"
    
	 // %d (или %i) — для знакового int
	 // %u - беззнаковый int
	 // %c - для одного символа char
	 // %s - массив символов
	 // %ld — для знакового long
	 // %lu — для беззнакового unsigned long
	 // %f - для преобразования в float/double
	
int Receive_Data_Length(int iPort,unsigned char* cInBuf, size_t iLength, long lTimeout);
void PrintHex(int port, char *label, unsigned char *buf, int len);
void completeTX(int serialTX, unsigned char *dataTX, int dataLen);



void main(void)
{
    int TxLength;
    int RxLength;
    int i;
	int counterQuery = 1;

    unsigned char TxData1[] = { 0x00, 0x08, 0x12, 0x00, 0x00 };
	unsigned char TxData2[] = { 0x00, 0x01, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x00, 0x00 };
	unsigned char TxData3[] = { 0x00, 0x0A, 0x84, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00 }; // Т1
	unsigned char TxData4[] = { 0x00, 0x0A, 0x84, 0x00, 0x02, 0x01, 0x00, 0x00, 0x00 }; // Т2
			
    unsigned char RxData[100];

    InitLib();

    int PschCOMPort  = 2;
    int DebugCOMPort = 4;

    InstallCom(PschCOMPort,  9600, 8, 0, 1);
    InstallCom(DebugCOMPort, 115200, 8, 0, 1);

    ToComStr(DebugCOMPort, "Debug information: Start program\n\r");

    unsigned long escapeTimer = *TimeTicks;
	
	int serialAdress = 21;
	
	
	
	
	size_t length = (sizeof(TxData1) / sizeof(TxData1[0])) - 2;
    completeTX(serialAdress, TxData1, length);
	
	length = (sizeof(TxData2) / sizeof(TxData2[0])) - 2;
    completeTX(serialAdress, TxData2, length);
	
	length = (sizeof(TxData3) / sizeof(TxData3[0])) - 2;
    completeTX(serialAdress, TxData3, length);
	
	length = (sizeof(TxData4) / sizeof(TxData4[0])) - 2;
    completeTX(serialAdress, TxData4, length);
	
	
	
	
	
	
	char millisFirst[15];

	sprintf(millisFirst, "TX(%lu)", *TimeTicks);
	
    PrintHex(DebugCOMPort, millisFirst, TxData1, sizeof(TxData1));
		
	ToComBufn(PschCOMPort, TxData1, sizeof(TxData1));
	WaitTransmitOver(PschCOMPort);
			
    while (1)
    {
        if (*TimeTicks - escapeTimer >= 3000)
            break;

        RxLength = Receive_Data_Length(PschCOMPort, RxData, sizeof(RxData), 100);

        if (RxLength > 0)
        {
			char millisRx[15];

			sprintf(millisRx, "RX(%lu)", *TimeTicks);
			
            PrintHex(DebugCOMPort, millisRx, RxData, RxLength);
			
			Delay(50);
			
			if(counterQuery == 3 || counterQuery == 4)
			{
				unsigned char hexVal[4]; // unsigned char - обязательно иначе будет переполенние 
				
				for(int exchange = 1; exchange < 5; exchange++)
					hexVal[exchange-1] = RxData[exchange];

				double value;
				char decVal[16];

				value = ((unsigned long)hexVal[0] << 24)
					  | ((unsigned long)hexVal[1] << 16)
					  | ((unsigned long)hexVal[2] << 8)
					  | (unsigned long)hexVal[3];

				if(counterQuery == 3)
				{
					sprintf(decVal, "T1: %f   \n", (value / 2500));
				}
				else
				{
					sprintf(decVal, "T2: %f   \n", (value / 2500));
				}

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



unsigned int CalculateCRC16(unsigned char *data, int len)
{
    unsigned int crc = 0xFFFF;
    int i, j;

    for (i = 0; i < len; ++i)
    {
        crc ^= (unsigned int)data[i];

        for (j = 0; j < 8; ++j)
        {
            if (crc & 0x0001)
                crc = (crc >> 1) ^ 0xA001;
            else
                crc = crc >> 1;
        }
    }

    return crc;
}



void completeTX(int serialTX, unsigned char *dataTX, int dataLen)
{
    unsigned char hexSerial;
    unsigned char *TempForCRC;
    unsigned int crc;
    int counter;

    hexSerial = (unsigned char)serialTX;

    dataTX[0] = hexSerial;

    TempForCRC = new unsigned char[dataLen];

    for (counter = 0; counter < dataLen; ++counter)
        TempForCRC[counter] = dataTX[counter];

    crc = CalculateCRC16(TempForCRC, dataLen);

    dataTX[dataLen]     = (unsigned char)(crc & 0xFF);        // младший байт 
    dataTX[dataLen + 1] = (unsigned char)((crc >> 8) & 0xFF); // старший байт

    delete[] TempForCRC;
    TempForCRC = NULL;
}