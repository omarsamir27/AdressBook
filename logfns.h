#ifndef LOGFNS_H_INCLUDED
#define LOGFNS_H_INCLUDED
#include <stdio.h>

typedef struct Contact Contact;
typedef struct Dob Dob;

FILE *load_create(char c);

Contact *initlog(FILE *plog);

void contactadd(FILE *plog);

void contactsave(Contact *datatowrite, unsigned int, FILE *target);

void logprint(Contact *data);

int isstring(char *tocheck);

int isdate(int day, int month, int year);

int isphone(char *phone);

void replace(char *strr, char toremove, char toput);


void home(Contact *data, FILE *plog);

void sortbook(Contact *data);

void contactprint(Contact *data, int *indexarr);

int contactcmpr(Contact a, Contact b, char sp);

int qsorthack(const void *a, const void *b);

void modifycontact(Contact *data);


void deletecontact(Contact *data, FILE *plog);

#endif // LOGFNS_H_INCLUDED
