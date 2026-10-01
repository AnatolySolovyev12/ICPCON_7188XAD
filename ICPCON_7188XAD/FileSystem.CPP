#include <7188XA.H>
#include <stdio.h>
#include <string.h>



FILE_DATA getFileDataStruct(char ** bufferInFile)
{
	char far *textFromFile; // если файл будет больше 64КБ то требуется: char huge *textFromFile;

	textFromFile = GetFilePositionByNo_AB(1, 0); 

	FILE_DATA far * fileStruct = GetFileInfoByNo_AB(1, 0);
	
	Print("B: File name in Struct: %s\r\n", fileStruct->fname);

	*bufferInFile = new char[fileStruct->size + 1]; // +1 для '\0'

	if(textFromFile != NULL)
	{
		Print("B: File size in Strcut: %ld\r\n", fileStruct->size);
		Print("B: Text in file:\n\n");

		for (int i = 0; i < fileStruct->size; ++i)
		{
			(*bufferInFile)[i] = textFromFile[i];

			/*if ((*bufferInFile)[i] == '\n') // Если необходимо отследить окончание строки в текстовике
				Print("\r\n******************\r\n");*/

			Print("%c", (*bufferInFile)[i]);
		}

		(*bufferInFile)[fileStruct->size] = '\0';
		
		return *fileStruct;
	}
	else
	{
		Print("B: Text in file is null\r\n");
		
		FILE_DATA emptyData;
		
		return emptyData;
	}
}



int count;



void main(void)
{   
	InitLib();
	Init5DigitLed();
	ReadSizeAB();
 
	 // %d (или %i) — для знакового int
	 // %u - беззнаковый int
	 // %c - для одного символа char
	 // %s - массив символов
	 // %ld — для знакового long
	 // %lu — для беззнакового unsigned long
 
    Print("Disk A size: %u\n", SizeAB.sizeA);
    Print("Disk B size: %u\n", SizeAB.sizeB);
    Print("Disk C size:    %u\n", SizeAB.sizeC);
    Print("SUMM:    %u\n", SizeAB.sum);
	
    Print("Files in A = %d\r\n", GetFileNo_AB(0));
    Print("Files in B = %d\r\n", GetFileNo_AB(1));
	
	int allFiles = GetFileNo_AB(1);
	
	char strA [100];
	char strB [100];
	
	
	if(!GetFileName_AB(0,0,strA))
		 Print("A: File name = %s\r\n", strA);
	else
		 Print("A: Do not save any data to fname\r\n");

	for(int countOfFileName = 0; countOfFileName < allFiles; ++countOfFileName)
	{	
		if(!GetFileName_AB(1,0,strB))
			 Print("B: File name = %s\r\n", strB);
		else
			 Print("B: Do not save any data to fname\r\n");
	}
	
	Print("\r\n");
	
	// В 16 битной DOS модели far указатель содержит сегмент и смещение и может обращаться за пределами текущего сегмента; 
	
	while(1)
	{
		Delay(1000); // тобы успевал буферизироваться символ для выходя из цикла
		
		if(Kbhit()) // Точка выхода из программы                   
		{                             
			int data=Getch();  
					
			if(data=='q' || data=='Q')                        
				return;                                      
		}  
		
		char far * bufferInFile;
						
		FILE_DATA noPtrFileStruct = getFileDataStruct(&bufferInFile);
		FILE_DATA * ptrFileStructSecond = &noPtrFileStruct;	
			
		int status = OS7_DeleteAllFile(1);	
		
		if(status == 0) Print("\n\nFILE IS DELETE\r\n");
		else if(status == -1) { Print("\nDELETE FAIL\r\n"); return;}
		else if(status == -2) { Print("\nNO FILE IN DISK\r\n"); return;}
		else if(status == -3) { Print("\nDiskB size is zero\r\n"); return;}
		
		char buffForWriteSecond[2401];
		Print("\nEnter new data for file:\r\n\n");
		LineInput(buffForWriteSecond, 2401);
		
		if(!OS7_OpenWriteFile(1) && ptrFileStructSecond != NULL)
		{
			if(OS7_WriteFile(1,bufferInFile,strlen(bufferInFile)) >= 0)
			{
				if(OS7_WriteFile(1,buffForWriteSecond,strlen(buffForWriteSecond)) >= 0)
				{
					if(OS7_CloseWriteFile(1, ptrFileStructSecond) >=0)
						Print("\nDATA IS ACCEPT\r\n");
					else
						Print("OS7_CloseWriteFile Error: somthing going wrong\r\n");
				}
				else
					Print("OS7_WriteFile Error: somthing going wrong with buffForWriteSecond\r\n");
			}
			else
				Print("OS7_WriteFile Error: somthing going wrong with bufferInFile\r\n");
		}
		else
		{
			if(ptrFileStructSecond != NULL)
				Print("OS7_OpenWriteFile Error: somthing going wrong\r\n");
			else
				Print("OS7_OpenWriteFile Error: fileStruct is NULL\r\n");
		}
			delete[] bufferInFile;
	}
}