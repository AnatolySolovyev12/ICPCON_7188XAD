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

int Receive_Data_Length(int iPort, unsigned char* cInBuf, size_t iLength, long lTimeout);
void PrintHex(int port, char* label, unsigned char* buf, int len);
void completeTX(int serialTX, unsigned char* dataTX, int dataLen);
int ReadLine(int port, char* buf, int maxLen);
void (far* Reset_Program)(void) = (void (far*)(void))0xFFFF0000L;

FILE_DATA getFileDataStruct(char** bufferInFile, int iPort);

void main(void)
{
	InitLib();
	ReadSizeAB();

	int TxLength;
	int RxLength;
	int i;
	int counterQuery = 1;

	unsigned char TxData1[] = { 0x00, 0x08, 0x12, 0x00, 0x00 };
	unsigned char TxData2[] = { 0x00, 0x01, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x00, 0x00 };
	unsigned char TxData3[] = { 0x00, 0x0A, 0x84, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00 }; // Т1
	unsigned char TxData4[] = { 0x00, 0x0A, 0x84, 0x00, 0x02, 0x01, 0x00, 0x00, 0x00 }; // Т2

	unsigned char RxData[100];

	int PschCOMPort = 2;
	int DebugCOMPort = 4;

	InstallCom(PschCOMPort, 9600, 8, 0, 1);
	InstallCom(DebugCOMPort, 115200, 8, 0, 1);

	const char* spaceCharArray = "   ";
	char infoArray[50];

	sprintf(infoArray, "Disk A size: %u\n", SizeAB.sizeA);
	ToComStr(DebugCOMPort, infoArray);
	WaitTransmitOver(DebugCOMPort);

	sprintf(infoArray, "Disk B size: %u\n", SizeAB.sizeB);
	ToComStr(DebugCOMPort, infoArray);
	WaitTransmitOver(DebugCOMPort);

	sprintf(infoArray, "Disk C size:    %u\n", SizeAB.sizeC);
	ToComStr(DebugCOMPort, infoArray);
	WaitTransmitOver(DebugCOMPort);

	sprintf(infoArray, "SUMM:    %u\n", SizeAB.sum);
	ToComStr(DebugCOMPort, infoArray);
	WaitTransmitOver(DebugCOMPort);

	sprintf(infoArray, "Files in A = %d\r\n", GetFileNo_AB(0));
	ToComStr(DebugCOMPort, infoArray);
	WaitTransmitOver(DebugCOMPort);

	sprintf(infoArray, "Files in B = %d\r\n", GetFileNo_AB(1));
	ToComStr(DebugCOMPort, infoArray);
	WaitTransmitOver(DebugCOMPort);

	char strA[100];
	char strB[100];

	if (!GetFileName_AB(0, 0, strA))
	{
		sprintf(infoArray, "A: File with sketch named = %s\r\n", strA);
		ToComStr(DebugCOMPort, infoArray);
		WaitTransmitOver(DebugCOMPort);
	}
	else
	{
		sprintf(infoArray, "A: Do not save any file in disk\r\n");
		ToComStr(DebugCOMPort, infoArray);
		WaitTransmitOver(DebugCOMPort);
	}

	if (!GetFileName_AB(1, 0, strB))
	{
		sprintf(infoArray, "B: File for dataBase named = %s\r\n", strB);
		ToComStr(DebugCOMPort, infoArray);
		WaitTransmitOver(DebugCOMPort);
	}
	else
		ToComStr(DebugCOMPort, "B: Do not save any file in disk\r\n");

	ToComStr(DebugCOMPort, "\nDebug information: Start program\n\n\r");

	TIME_DATE* checkDateTime;
	TIME_DATE* currDateTime;

	SetDo1(1);
	SetDo2(1);

	while (1)
	{
		GetTimeDate(currDateTime); // для проверки дат на предмет наступления новых суток

		Delay(2000);

		if (!GetDi1()) // при замыкании Di1 выводим базу данных на экран
		{
			int exit = 0;

			for (;;)
			{
				char charCommand[2];
				ToComStr(DebugCOMPort, "\n\rChoose what command do you need?\n\r"
					"1-show all data base\n\r"
					"2-show data for one serial number\n\r"
					"3-restart programm\n\r"
					"4-quit from programm\n\r"
				);
				ReadLine(DebugCOMPort, charCommand, 2);

				if (strcmp(charCommand, "1") == 0)
				{
					ToComStr(DebugCOMPort, "\n\rchoose 1\n\r");
					char far* testBufferInFile;
					getFileDataStruct(&testBufferInFile, DebugCOMPort);
				}
				else if (strcmp(charCommand, "2") == 0)
					ToComStr(DebugCOMPort, "\n\rchoose 2\n\r");
				else if (strcmp(charCommand, "3") == 0)
				{
					ToComStr(DebugCOMPort, "\n\rchoose 3\n\r");
					WaitTransmitOver(DebugCOMPort);
					Reset_Program();
				}
				else if (strcmp(charCommand, "4") == 0)
				{
					exit = 1;
					break;
				}
			}
			if (exit) break;
		}

		if (!GetDi2()) break; // при замыкании Di2 завершаем программу

		if (checkDateTime->day != currDateTime->day)
		{
			for (int serialAdress = 1; serialAdress < 255; ++serialAdress) // сканируем 255 адресов
			{
				unsigned long escapeTimer = *TimeTicks;

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

				char fullString[50];

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

						if (counterQuery == 3 || counterQuery == 4)
						{
							unsigned char hexVal[4]; // unsigned char - обязательно иначе будет переполенние 

							for (int exchange = 1; exchange < 5; exchange++)
								hexVal[exchange - 1] = RxData[exchange];

							double value;
							char decVal[50];

							value = ((unsigned long)hexVal[0] << 24)
								| ((unsigned long)hexVal[1] << 16)
								| ((unsigned long)hexVal[2] << 8)
								| (unsigned long)hexVal[3];

							if (counterQuery == 3)
							{
								sprintf(decVal, "#%d#   $%d.%d.%d$   T1: %f", serialAdress, currDateTime->day, currDateTime->month, currDateTime->year, (value / 2500));
								strcpy(fullString, decVal);
								strcat(fullString, spaceCharArray);
							}
							else
							{
								sprintf(decVal, "T2: %f\n\r", (value / 2500));
								strcat(fullString, decVal);
								ToComStr(DebugCOMPort, fullString);
								WaitTransmitOver(DebugCOMPort);

								char far* bufferInFile;

								FILE_DATA noPtrFileStruct = getFileDataStruct(&bufferInFile, DebugCOMPort);
								FILE_DATA* ptrFileStructSecond = &noPtrFileStruct;

								int status = OS7_DeleteAllFile(1);

								if (status == 0) ToComStr(DebugCOMPort, "\nFILE IS DELETE\r\n");
								else if (status == -1) { ToComStr(DebugCOMPort, "\nDELETE FAIL\r\n"); break; }
								else if (status == -2) { ToComStr(DebugCOMPort, "\nNO FILE IN DISK\r\n"); break; }
								else if (status == -3) { ToComStr(DebugCOMPort, "\nDiskB size is zero\r\n"); break; }

								if (!OS7_OpenWriteFile(1) && ptrFileStructSecond != NULL)
								{
									if (OS7_WriteFile(1, bufferInFile, strlen(bufferInFile)) >= 0)
									{
										if (OS7_WriteFile(1, fullString, strlen(fullString)) >= 0)
										{
											if (OS7_CloseWriteFile(1, ptrFileStructSecond) >= 0)
												ToComStr(DebugCOMPort, "\nDATA IS ACCEPT\n\n\r");
											else
												ToComStr(DebugCOMPort, "OS7_CloseWriteFile Error: somthing going wrong\r\n");
										}
										else
											ToComStr(DebugCOMPort, "OS7_WriteFile Error: somthing going wrong with fullString\r\n");
									}
									else
										ToComStr(DebugCOMPort, "OS7_WriteFile Error: somthing going wrong with bufferInFile\r\n");
								}
								else
								{
									if (ptrFileStructSecond != NULL)
										ToComStr(DebugCOMPort, "OS7_OpenWriteFile Error: somthing going wrong\r\n");
									else
										ToComStr(DebugCOMPort, "OS7_OpenWriteFile Error: fileStruct is NULL\r\n");
								}
								delete[] bufferInFile;
							}

							if (counterQuery == 4) break;
						}

						counterQuery++;

						char millis[15];

						sprintf(millis, "TX(%lu)", *TimeTicks);

						switch (counterQuery)
						{
						case(1):
						{
							PrintHex(DebugCOMPort, millis, TxData1, sizeof(TxData1));
							ToComBufn(PschCOMPort, TxData1, sizeof(TxData1));
							WaitTransmitOver(PschCOMPort);

							break;
						};

						case(2):
						{
							PrintHex(DebugCOMPort, millis, TxData2, sizeof(TxData2));
							ToComBufn(PschCOMPort, TxData2, sizeof(TxData2));
							WaitTransmitOver(PschCOMPort);

							break;
						};

						case(3):
						{
							PrintHex(DebugCOMPort, millis, TxData3, sizeof(TxData3));
							ToComBufn(PschCOMPort, TxData3, sizeof(TxData3));
							WaitTransmitOver(PschCOMPort);

							break;
						};

						case(4):
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
			}
			GetTimeDate(checkDateTime);
		}
	}
	RestoreCom(PschCOMPort);
	RestoreCom(DebugCOMPort);
}



int Receive_Data_Length(int iPort, unsigned char* cInBuf, size_t iLength, long lTimeout)
{
	unsigned char cChar;
	int iIndex = 0;
	unsigned long lStartTime;

	if (IsCom(iPort))
	{
		lStartTime = *TimeTicks;

		for (;;)
		{
			while (IsCom(iPort)) //check COM port
			{
				cInBuf[iIndex++] = ReadCom(iPort); // alter ReadComn

				if (iIndex >= iLength)
				{
					cInBuf[iIndex] = 0;
					return iIndex;     /* return data length */
				}

				lStartTime = *TimeTicks;  /* refresh data timeout */
			}

			if ((*TimeTicks - lStartTime) >= lTimeout)
				return iIndex;  /* receive data timeout */

			RefreshWDT();
		}
	}
	else
		return 0;
}



void PrintHex(int port, char* label, unsigned char* buf, int len)
{
	if (len > 0)
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



unsigned int CalculateCRC16(unsigned char* data, int len)
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



void completeTX(int serialTX, unsigned char* dataTX, int dataLen)
{
	unsigned char hexSerial;
	unsigned char* TempForCRC;
	unsigned int crc;
	int counter;

	hexSerial = (unsigned char)serialTX;

	dataTX[0] = hexSerial;

	TempForCRC = new unsigned char[dataLen];

	for (counter = 0; counter < dataLen; ++counter)
		TempForCRC[counter] = dataTX[counter];

	crc = CalculateCRC16(TempForCRC, dataLen);

	dataTX[dataLen] = (unsigned char)(crc & 0xFF);        // младший байт 
	dataTX[dataLen + 1] = (unsigned char)((crc >> 8) & 0xFF); // старший байт

	delete[] TempForCRC;
	TempForCRC = NULL;
}



FILE_DATA getFileDataStruct(char** bufferInFile, int iPort)
{
	char infoArray[50];

	char far* textFromFile; // если файл будет больше 64КБ то требуется: char huge *textFromFile;

	textFromFile = GetFilePositionByNo_AB(1, 0);

	FILE_DATA far* fileStruct = GetFileInfoByNo_AB(1, 0);

	*bufferInFile = new char[fileStruct->size + 1]; // +1 для '\0'

	if (textFromFile != NULL)
	{
		sprintf(infoArray, "\nB: File size in Strcut: %ld\r\nB: Text in file:\n\n", fileStruct->size);
		ToComStr(iPort, infoArray);
		WaitTransmitOver(iPort);

		for (int i = 0; i < fileStruct->size; ++i)
		{
			(*bufferInFile)[i] = textFromFile[i];

			/*if ((*bufferInFile)[i] == '\n') // Если необходимо отследить окончание строки в текстовике
				Print("\r\n******************\r\n");*/

			sprintf(infoArray, "%c", (*bufferInFile)[i]);
			ToComStr(iPort, infoArray);
			WaitTransmitOver(iPort);
		}

		(*bufferInFile)[fileStruct->size] = '\0';

		return *fileStruct;
	}
	else
	{
		ToComStr(iPort, "B: Text in file is null\r\n");
		WaitTransmitOver(iPort);

		FILE_DATA emptyData;

		return emptyData;
	}
}



int ReadLine(int port, char* buf, int maxLen)
{
	int i = 0;
	int ch;

	for (;;)
	{
		if (IsCom(port))
		{
			ch = ReadCom(port);

			if (ch == '\r' || ch == '\n')   /* Enter */
			{
				buf[i] = '\0';
				/* отправить эхо перевода строки */
				ToCom(port, '\r');
				ToCom(port, '\n');
				return i;
			}
			else if (ch == 0x08 || ch == 0x7F)  /* Backspace */
			{
				if (i > 0)
				{
					--i;
					ToCom(port, 0x08);
					ToCom(port, ' ');
					ToCom(port, 0x08);
				}
			}
			else if (ch >= 0x20 && ch < 0x7F)   /* печатаемый символ */
			{
				if (i < maxLen - 1)
				{
					buf[i++] = (char)ch;
					ToCom(port, ch);   /* эхо символа */
				}
			}
		}
	}
}