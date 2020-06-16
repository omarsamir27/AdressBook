#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include "sysbeautify.h"
#include "logfns.h"

/*BEWARE YOU MUST CLEAR STDIN AFTER GETCHAR(),FGETS IS SAFE BUT REMOVE \N FROM ARRAY*/











int main(void) {
    puts("Welcome to Your Phonebook");
    delay(1);
    FILE *plog = load_create('1');
    Contact *data = initlog(plog);
    home(data, plog);
    fclose(plog);
    puts("Exiting");
    free(data);
    delay(3);
    return 0;
}



