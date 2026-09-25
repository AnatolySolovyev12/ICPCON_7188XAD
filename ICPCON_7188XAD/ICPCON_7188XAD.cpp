#include <7188XA.H>
#include <stdio.h>
#include <string.h>
#include <iostream.h>

/*

128 | 64 | 32 | 16 | 8 | 4 | 2 | 1

00000000

       a = 6
       ---
  f = 1 |   | b = 5
       -g-     g = 0
  e = 2 |   | c = 4
       ---
        d = 3       dp = 7

        */

#define SEG_G   0x01  /* средний, bit 0 */
#define SEG_F   0x02  /* верхний левый, bit 1 */
#define SEG_E   0x04  /* нижний левый, bit 2 */
#define SEG_D   0x08  /* нижний, bit 3 */
#define SEG_C   0x10  /* нижний правый, bit 4 */
#define SEG_B   0x20  /* верхний правый, bit 5 */
#define SEG_A   0x40  /* верхний, bit 6 */
#define SEG_DP  0x80  /* decimal point, bit 7 */

#define SEG_OFF 0x00
#define SEG_ALL (SEG_A | SEG_B | SEG_C | SEG_D | \
                 SEG_E | SEG_F | SEG_G)



#define SEG_0   (SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F) /* 0x7E */
#define SEG_1   (SEG_B | SEG_C)                                 /* 0x30 */
#define SEG_2   (SEG_A | SEG_B | SEG_D | SEG_E | SEG_G)         /* 0x6D */
#define SEG_3   (SEG_A | SEG_B | SEG_C | SEG_D | SEG_G)         /* 0x79 */
#define SEG_4   (SEG_B | SEG_C | SEG_F | SEG_G)                 /* 0x33 */
#define SEG_5   (SEG_A | SEG_C | SEG_D | SEG_F | SEG_G)         /* 0x5B */
#define SEG_6   (SEG_A | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G) /* 0x5F */
#define SEG_7   (SEG_A | SEG_B | SEG_C)                         /* 0x70 */
#define SEG_8   (SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G) /* 0x7F */
#define SEG_9   (SEG_A | SEG_B | SEG_C | SEG_D | SEG_F | SEG_G) /* 0x7B */



#define SEG_MINUS      SEG_G                   /* 0x01: - */
#define SEG_UNDERSCORE SEG_D                   /* 0x08: _ */
#define SEG_DOT        SEG_DP                  /* 0x80: . */
#define SEG_BLANK      SEG_OFF                  /* 0x00: пробел */
#define SEG_EQUAL      (SEG_G | SEG_D)          /* 0x09: = */
#define SEG_DEGREE     (SEG_A | SEG_B | SEG_F | SEG_G) /* 0x63 */
#define SEG_STAR       SEG_ALL                  /* 0x7F: приближённо */



#define SEG_A_CHAR  (SEG_A | SEG_B | SEG_C | SEG_E | SEG_F | SEG_G) /* 0x77 */
#define SEG_B_CHAR  (SEG_C | SEG_D | SEG_E | SEG_F | SEG_G)         /* 0x1F: b */
#define SEG_C_CHAR  (SEG_A | SEG_D | SEG_E | SEG_F)                 /* 0x4E */
#define SEG_D_CHAR  (SEG_B | SEG_C | SEG_D | SEG_E | SEG_G)         /* 0x3D: d */
#define SEG_E_CHAR  (SEG_A | SEG_D | SEG_E | SEG_F | SEG_G)         /* 0x4F */
#define SEG_F_CHAR  (SEG_A | SEG_E | SEG_F | SEG_G)                 /* 0x47 */
#define SEG_G_CHAR  (SEG_A | SEG_C | SEG_D | SEG_E | SEG_F)         /* 0x5E */
#define SEG_H_CHAR  (SEG_B | SEG_C | SEG_E | SEG_F | SEG_G)         /* 0x37 */
#define SEG_I_CHAR  (SEG_B | SEG_C)                                 /* 0x30: похоже на 1 */
#define SEG_J_CHAR  (SEG_B | SEG_C | SEG_D | SEG_E)                 /* 0x3C */
#define SEG_K_CHAR  (SEG_E | SEG_F | SEG_G)                         /* 0x07: приблизительно K */
#define SEG_L_CHAR  (SEG_D | SEG_E | SEG_F)                         /* 0x0E */
#define SEG_M_CHAR  (SEG_A | SEG_B | SEG_C | SEG_E | SEG_F)         /* 0x76: приблизительно M */
#define SEG_N_CHAR  (SEG_C | SEG_E | SEG_G)                         /* 0x15: n */
#define SEG_O_CHAR  SEG_0                                            /* 0x7E */
#define SEG_P_CHAR  (SEG_A | SEG_B | SEG_E | SEG_F | SEG_G)         /* 0x67 */
#define SEG_Q_CHAR  (SEG_A | SEG_B | SEG_C | SEG_D | SEG_F | SEG_G) /* 0x7B: похоже на 9 */
#define SEG_R_CHAR  (SEG_E | SEG_G)                                 /* 0x05: r */
#define SEG_S_CHAR  SEG_5                                            /* 0x5B */
#define SEG_T_CHAR  (SEG_D | SEG_E | SEG_F | SEG_G)                 /* 0x0F: t */
#define SEG_U_CHAR  (SEG_B | SEG_C | SEG_D | SEG_E | SEG_F)         /* 0x3E */
#define SEG_V_CHAR  (SEG_C | SEG_D | SEG_E)                         /* 0x1C: v */
#define SEG_W_CHAR  (SEG_B | SEG_C | SEG_D | SEG_E | SEG_F)         /* 0x3E: визуально U/W */
#define SEG_X_CHAR  (SEG_B | SEG_C | SEG_E | SEG_F | SEG_G)         /* 0x37: похоже на H */
#define SEG_Y_CHAR  (SEG_B | SEG_C | SEG_D | SEG_F | SEG_G)         /* 0x3B */
#define SEG_Z_CHAR  SEG_2                                            /* 0x6D */



unsigned char Get5DigitLedMask(char ch)
{
    if (ch >= 'a' && ch <= 'z')
        ch = (char)(ch - 'a' + 'A');

    switch (ch)
    {
    case '0': return SEG_0;
    case '1': return SEG_1;
    case '2': return SEG_2;
    case '3': return SEG_3;
    case '4': return SEG_4;
    case '5': return SEG_5;
    case '6': return SEG_6;
    case '7': return SEG_7;
    case '8': return SEG_8;
    case '9': return SEG_9;

    case 'A': return SEG_A_CHAR;
    case 'B': return SEG_B_CHAR;
    case 'C': return SEG_C_CHAR;
    case 'D': return SEG_D_CHAR;
    case 'E': return SEG_E_CHAR;
    case 'F': return SEG_F_CHAR;
    case 'G': return SEG_G_CHAR;
    case 'H': return SEG_H_CHAR;
    case 'I': return SEG_I_CHAR;
    case 'J': return SEG_J_CHAR;
    case 'K': return SEG_K_CHAR;
    case 'L': return SEG_L_CHAR;
    case 'M': return SEG_M_CHAR;
    case 'N': return SEG_N_CHAR;
    case 'O': return SEG_O_CHAR;
    case 'P': return SEG_P_CHAR;
    case 'Q': return SEG_Q_CHAR;
    case 'R': return SEG_R_CHAR;
    case 'S': return SEG_S_CHAR;
    case 'T': return SEG_T_CHAR;
    case 'U': return SEG_U_CHAR;
    case 'V': return SEG_V_CHAR;
    case 'W': return SEG_W_CHAR;
    case 'X': return SEG_X_CHAR;
    case 'Y': return SEG_Y_CHAR;
    case 'Z': return SEG_Z_CHAR;

    case '-': return SEG_MINUS;
    case '_': return SEG_UNDERSCORE;
    case '.': return SEG_DOT;
    case '=': return SEG_EQUAL;
    case ' ': return SEG_BLANK;
    }

    return SEG_BLANK;
}



void ShowText5(const char* text)
{
    int pos;
    unsigned char mask;

    for (pos = 1; pos <= 5; ++pos)
    {
        if (*text != 0)
        {
            mask = Get5DigitLedMask(*text);
            ++text;
        }
        else
        {
            mask = SEG_BLANK;
        }

        Show5DigitLedSeg(pos, mask);
    }
}





// Оба варианта допустимы
const char* fullText = "     TEST WITH FULL CHAR ARRAY     ";
char fullTextCharArray[] = "     TEST WITH FULL CHAR ARRAY     ";
int count;



void main(void)
{

    InitLib();
    Init5DigitLed();

    for (count = 0; count < strlen(fullTextCharArray); ++count)
    {
        if (Kbhit()) // Точка выхода из программы                   
        {
            int data = Getch();

            if (data == 'q' || data == 'Q')
                return;
        }

        char tempTextCharArray[6];

        tempTextCharArray[0] = fullText[count];
        tempTextCharArray[1] = fullText[count + 1];
        tempTextCharArray[2] = fullText[count + 2];
        tempTextCharArray[3] = fullText[count + 3];
        tempTextCharArray[4] = fullText[count + 4];
        tempTextCharArray[5] = '\0';

        cout << tempTextCharArray << endl;

        ShowText5(tempTextCharArray);
        Delay(300);
    }

}