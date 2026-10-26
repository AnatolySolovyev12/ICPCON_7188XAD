Instructions for interacting with the ICP CON I-7188XAD industrial controller are posted here. The repository also contains code and executables that can be downloaded to the controller /
Здесь выложены инструкции по взаимодействию с промышленным контроллером ICP CON I-7188XAD. Также в репозитории присутствуют код и исполняемые файлы, которые можно загрузить на контроллер

Instructions for connecting to the controller via MiniOS 7 / Инструкция по подключению к контроллеру через MiniOS 7
[fast start.docx](https://github.com/user-attachments/files/32741718/fast.start.docx)

Borland Turbo C++ 3.x Compiler Installation and Startup Instructions (DOS) / Инструкция по установке и запуску компилятора Borland Turbo C++ 3.x (DOS)
[compiler manula.docx](https://github.com/user-attachments/files/32742054/compiler.manula.docx)

List of commands for working with MiniOS 7 with explanations / Список команд для работы с MiniOS 7 с пояснениями
[miniOS command.docx](https://github.com/user-attachments/files/32742110/miniOS.command.docx)  
<br>

### **baseStation**

The sketch converts 7188 into a base station performing the following functions:

1) When a new day arrives, it polls from 1 to 255 network addresses on the COM2 according to the NZIF/SET counter protocol (3TM/ 4TM, etc.). The readings of the first and second tariff at the beginning of the current day are requested.
2) The obtained readings are recorded in the first file on disk B (conditional database).
3) All manipulations are duplicated in the form of a log on the COM4.
4) A high signal level is set on the DO1 and DO2 from the moment of starting.
5) When a high signal level is applied to the DI2, the program execution is completed and the OS is released. No DI check is performed during counter polling.
6) When a high signal level is applied to the DI1, a transition to the context menu is made where you can:
a) display the entire database
b) display information from the database at a specific network address
c) restart the program

/

Скетч преобразует 7188 в базовую станцию выполняющую следующие функции:

1) При наступлении новых суток опрашивает от 1 до 255 сетевой адрес на COM2 по протоколу счётчика ПСЧ/СЭТ (3ТМ / 4ТМ и т.п.) производства NZIF. Запрашиваются показания первого и второго тарифа на начало текущих суток. 
2) Полученные показания записываются в первый файл на диске B (условная база данных).
3) Все манипуляции дублируются в виде лога на COM4. 
4) На DO1 и DO2 с момента запуска задаётся высокий уровень сигнала.
5) При подаче высокого уровня сигнала на DI2 выполнение программы завершается  и производится выход в ОС. Во время опроса счётчиков проверка DI не производится.
6) При подаче высокого уровня сигнала на DI1, производится переход в контекстное меню где можно:
    a) вывести на экран всю базу данных
    б) вывести на экран информацию из базы по конкретному сетевому адресу
    в) перезагрузить программу
    г) выйти из программы в ОС

<img width="947" height="502" alt="12345" src="https://github.com/user-attachments/assets/f8a64300-cb2e-4d4d-ad48-ef1396148c84" />
...
### **5DigitLed**

Displays user input on a ticker of 5 LED elements. / Выводит пользовательский ввод на бегущую строку из 5-и светодиодных элементов.

```

    T
   TE
  TES
 TEST
TEST
EST W
ST WI
T WIT
 WITH
WITH
ITH F
TH FU
H FUL
 FULL
FULL
ULL C
LL CH
L CHA
 CHAR
CHAR
HAR A
AR AR
R ARR
 ARRA
ARRAY
RRAY
RAY
AY
Y

```
<img width="518" height="379" alt="image" src="https://github.com/user-attachments/assets/51279b42-932c-4a44-906d-d3c82bff87a3" />  
<br>

### **FileSystem**

Subtracts an existing file on disk B and allows you to add user input there by caching the contents of the old file, then deleting it and creating a new file into which the cache is written and then what the user entered. The file is saved with the same name as the old file. / Вычитывает существующий файл на диске B и позволяет дописывать туда пользовательский ввод, путём кэширования содержимого старого файла, последующего его удаления и создания нового файла, в который записывается кэш и затем то что ввёл пользователь. Файл сохраняется с тем же именем что было у старого файла. 

<img width="472" height="380" alt="image" src="https://github.com/user-attachments/assets/4bb2fe71-2fc5-4b76-a5b5-3189f9a207e8" />

<img width="410" height="501" alt="image" src="https://github.com/user-attachments/assets/c772e173-d4db-4d9f-ad10-bbdc796d75b8" />  
<br>



### **comExchange**

Throws traffic from COM1 (RS-232/485) to COM2 (RS-485) and vice versa. Also, all traffic is duplicated for COM4 analysis (RS-232). As an example, 7188 is placed in the gap between the FST counter and the modem and through COM4 you can track the exchange. / Прокидывает траффик от COM1(RS-232/485) к COM2(RS-485) и наоборот. Также весь траффик дублируется для анализа на COM4(RS-232). В качестве примера 7188 ставится в разрыв между счётчиком ПСЧ и модемом и через COM4 можно отслеживать наличие обмена.


<img width="1014" height="813" alt="123 (1)" src="https://github.com/user-attachments/assets/2931d001-3dc0-458d-bb1f-d5c404094f3a" />
