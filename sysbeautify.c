#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ctype.h>






void clrscr() {
#ifdef  __linux
    system("clear");
#elif _WIN32
    system("cls");
#else
    print("WHAT ARE YOU RUNNING??")
#endif
}

void delay(short delaysec) { for (time_t wait = clock(); clock() - wait < delaysec * CLOCKS_PER_SEC;); }



void stdinclr(void) {
    int ch;
    while ((ch = getchar()) != EOF && ch != '\n');
}

void spaceprint(unsigned short spaces) {
    for (int i = 0; i < spaces; ++i)
        printf(" ");
}

void replace(char *strr, char toremove, char toput) {
    int length = strlen(strr);
        for (int i = 0; i < length - 1; ++i) {
            if (*(strr + i) == toremove) *(strr + i) = toput;

        }
    }
