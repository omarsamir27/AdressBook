#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include "logfns.h"



unsigned short filemode; //mode to use in functions 1:read 2:write(new log |removing records|writing new sort) 3:append(adding contacts)
unsigned int contact_count;
char filepath[260];
char sortpriority;
struct Dob {
    unsigned short day;
    unsigned short month;
    unsigned short year;
};
struct Contact {
    char firstn[31];
    char lastn[31];
    char phone[13];
    char mail[41];
    char adrs[50];
    Dob dob;
};


FILE *load_create(char c) {
    char choice;
    if (c == '1') {//main()calling me
        puts("Path of Phonebook to load:");
        fgets(filepath, 260, stdin);
        filepath[strcspn(filepath, "\n")] = '\0';
    }
    FILE *plog;
    plog = fopen(filepath, "r");
    if (!plog) {
        while (1) {
            fputs("Can't open file,it's probably not there.Do you want to create it\nY to create or N to exit ? ",stderr);
            choice = getchar();
            stdinclr();
            if (tolower(choice) == 'y') {
                plog = fopen(filepath, "w");
                //fprintf(plog, "%s", "0\0\n");
                fflush(plog);
                break;
            } else if (tolower(choice) == 'n') exit(0);
            else continue;
        }
    }
    return plog;
}

Contact *initlog(FILE *plog) {
    fscanf(plog, "%d", &contact_count);
    fseek(plog, 2, SEEK_CUR);
    fseek(plog,0,SEEK_END);
    long fsize=ftell(plog);
    rewind(plog);
    fscanf(plog, "%d", &contact_count);
    fseek(plog, 2, SEEK_CUR);
    if (!contact_count&&fsize!=0) {
        filemode = 2;
        int linecount=1;
        char tempc;
         for (tempc = getc(plog); tempc != EOF; tempc = getc(plog)){
            if (tempc == 10)
                {linecount += 1;}}
                 //printf("%d\n",linecount);
       rewind(plog);
       Contact *pdata = (Contact *) (malloc(linecount * sizeof(Contact)));
          for (int currecord = 0; currecord <= linecount; ++currecord) {
            fscanf(plog, "%[^,],%[^,],%hu-%hu-%hu,%[^,],%[^,],%s\r\n", pdata[currecord].firstn, pdata[currecord].lastn,&pdata[currecord].dob.day,
                   &pdata[currecord].dob.month, &pdata[currecord].dob.year,pdata[currecord].adrs,pdata[currecord].mail,pdata[currecord].phone);
                   replace(pdata[currecord].adrs, ' ','-');}
                   fflush(plog);
                   contactsave(pdata,linecount,fopen(filepath,"w"));
                   fflush(plog);}
    else if(!contact_count&&fsize==0){
        filemode = 2;
        contactadd(plog);
    }
     else {
        Contact *pdata = (Contact *) (malloc(contact_count * sizeof(Contact)));
        for (int currecord = 0; currecord < contact_count; ++currecord) {
            fscanf(plog, "%[^,],%[^,],%[^,],%[^,],%[^,],%hu/%hu/%hu\r\n", pdata[currecord].firstn, pdata[currecord].lastn,
                   pdata[currecord].phone, pdata[currecord].mail, pdata[currecord].adrs, &pdata[currecord].dob.day,
                   &pdata[currecord].dob.month, &pdata[currecord].dob.year);
            replace(pdata[currecord].firstn, ' ', '\0');
            replace(pdata[currecord].lastn, ' ', '\0');
            //replace(pdata[currecord].adrs, ' ','-');
            replace(pdata[currecord].mail, ' ', '\0');
            replace(pdata[currecord].phone, ' ', '\0');
            if (!isstring(pdata[currecord].firstn))
                memcpy(pdata[currecord].firstn, "CRPT\0", sizeof(pdata[currecord].firstn));
            if (!isstring(pdata[currecord].lastn))
                memcpy(pdata[currecord].lastn, "CRPT\0", sizeof(pdata[currecord].lastn));
            if (!isphone(pdata[currecord].phone))
                memcpy(pdata[currecord].phone, "CRPT\0", sizeof(pdata[currecord].phone));
            if (!strstr(pdata[currecord].mail, "@"))
                memcpy(pdata[currecord].mail, "CRPT\0", sizeof(pdata[currecord].mail));
            if (!isdate(pdata[currecord].dob.day, pdata[currecord].dob.month, pdata[currecord].dob.year)) {
                pdata[currecord].dob.day = 0;
                pdata[currecord].dob.month = 0;
                pdata[currecord].dob.year = 0;
            }}return pdata;
    }
    return NULL;
}

void contactadd(FILE *plog) {
    Contact *tempdata = (Contact *) malloc(sizeof(Contact) * 10);
    puts("Taking max of 10 contacts per write");
    unsigned int contacts_req = 0;
    char choice;
    for (; contacts_req < 10;) {
        if (contacts_req == 9) {
            do {
                puts("you have reached 10 contacts\nProceed to save?Yes[Y],No[N]");
                choice = getchar();
                stdinclr();
                if (tolower(choice) == 'y') {
                    contactsave(tempdata, contacts_req, plog);
                    return;
                } else if (tolower(choice) == 'n') {
                    free(tempdata);
                    return;
                } else continue;
            } while (tolower(choice) != 'y' && tolower(choice) != 'n');
        }
        printf("Add new contact \nY or N\n");
        choice = getchar();
        stdinclr();
        if (tolower(choice) == 'y') {
            do {
                puts("Write First Name:");
                fgets((tempdata[contacts_req].firstn), 31, stdin);
            } while (!isstring(tempdata[contacts_req].firstn));
            tempdata[contacts_req].firstn[strcspn(tempdata[contacts_req].firstn, "\n")] = '\0';
            do {
                puts("Write Last Name:");
                fgets((tempdata[contacts_req].lastn), 31, stdin);
            } while (!isstring(tempdata[contacts_req].lastn));
            tempdata[contacts_req].lastn[strcspn(tempdata[contacts_req].lastn, "\n")] = '\0';
            do {
                puts("Mobile Number:");
                fgets((tempdata[contacts_req].phone), 13, stdin);
            } while (!isphone(tempdata[contacts_req].phone));
            tempdata[contacts_req].phone[strcspn(tempdata[contacts_req].phone, "\n")] = '\0';
            puts("Address:");
            fgets((tempdata[contacts_req].adrs), 50, stdin);
            tempdata[contacts_req].adrs[strcspn(tempdata[contacts_req].adrs, "\n")] = '\0';
            do {
                puts("Email:");
                fgets((tempdata[contacts_req].mail), 41, stdin);
            } while (!strstr(tempdata[contacts_req].mail, "@"));
            tempdata[contacts_req].mail[strcspn(tempdata[contacts_req].mail, "\n")] = '\0';
            do {
                puts("DOB dd/mm/yy");
                scanf("%hu/%hu/%hu", &tempdata[contacts_req].dob.day, &tempdata[contacts_req].dob.month,
                      &tempdata[contacts_req].dob.year);
                stdinclr();
            } while (!isdate(tempdata[contacts_req].dob.day, tempdata[contacts_req].dob.month,
                             tempdata[contacts_req].dob.year));
            contacts_req++;
        } else if (tolower(choice) == 'n') {
            contactsave(tempdata, contacts_req, plog);
            return;
        }
    }
}

void contactsave(Contact *datatowrite, unsigned int contacts_req, FILE *target) {
    char choice;
    do {
        puts("Y save your changes or N Discard them\n");
        choice = getchar();
        stdinclr();
    } while (tolower(choice) != 'y' && tolower(choice) != 'n');
    if (filemode == 2 && tolower(choice) == 'y') {
           freopen(filepath, "w", target);
        fprintf(target, "%d\n", contacts_req);
        for (int i = 0; i < contacts_req; ++i) {
            fprintf(target, "%-31s", datatowrite[i].firstn);
            fprintf(target, "%c", ',');
            fprintf(target, "%-31s", datatowrite[i].lastn);
            fprintf(target, "%c", ',');
            fprintf(target, "%-12s", datatowrite[i].phone);
            fprintf(target, "%c", ',');
            fprintf(target, "%-41s", datatowrite[i].mail);
            fprintf(target, "%c", ',');
            fprintf(target, "%-50s", datatowrite[i].adrs);
            fprintf(target, "%c", ',');
            fprintf(target, "%2d", datatowrite[i].dob.day);
            fprintf(target, "%c", '/');
            fprintf(target, "%2d", datatowrite[i].dob.month);
            fprintf(target, "%c", '/');
            fprintf(target, "%4d", datatowrite[i].dob.year);
            fprintf(target, "%c", '\n');
        }
        fflush(target);
    }
    if (filemode == 3 && tolower(choice) != 'n') {
        int totalcontacts = contacts_req + contact_count;
        freopen(NULL, "r+", target);
        fprintf(target, "%d\n", totalcontacts);
        fseek(target, 0, SEEK_END);
        for (int i = 0; i < contacts_req; ++i) {
            fprintf(target, "%-31s", datatowrite[i].firstn);
            fprintf(target, "%c", ',');
            fprintf(target, "%-31s", datatowrite[i].lastn);
            fprintf(target, "%c", ',');
            fprintf(target, "%-12s", datatowrite[i].phone);
            fprintf(target, "%c", ',');
            fprintf(target, "%-41s", datatowrite[i].mail);
            fprintf(target, "%c", ',');
            fprintf(target, "%-50s", datatowrite[i].adrs);
            fprintf(target, "%c", ',');
            fprintf(target, "%2d", datatowrite[i].dob.day);
            fprintf(target, "%c", '/');
            fprintf(target, "%2d", datatowrite[i].dob.month);
            fprintf(target, "%c", '/');
            fprintf(target, "%4d", datatowrite[i].dob.year);
            fprintf(target, "%c", '\n');
        }
        fflush(target);
    }
    if (tolower(choice) == 'n') {
        free(datatowrite);
        return;
    }
}

void logprint(Contact *data) {
    for (int i = 0; i < contact_count; ++i) {
        if (i == 0) {
            printf("First name");
            spaceprint(21);
            printf("Last name");
            spaceprint(22);
            printf("Email");
            spaceprint(36);
            printf("Address");
            spaceprint(23);
            printf("Mobile");
            spaceprint(7);
            printf("DOB\n");
        }
        printf("%s", data[i].firstn);
        spaceprint(31 - strcspn(data[i].firstn, "\0"));
        printf("%s", data[i].lastn);
        spaceprint(31 - strcspn(data[i].lastn, "\0"));
        printf("%s", data[i].mail);
        spaceprint(41 - strcspn(data[i].mail, "\0"));
        printf("%s", data[i].adrs);
        spaceprint(30 - strcspn(data[i].adrs, "\0"));
        printf("%s", data[i].phone);
        spaceprint(13 - strcspn(data[i].phone, "\0"));
        printf("%hu/%hu/%hu\n\n", data[i].dob.day, data[i].dob.month, data[i].dob.year);
    }
    char choice;
    do {
        puts("return to home?Y?");
        choice = getchar();
        stdinclr();
    } while (tolower(choice) != 'y');
}

void contactprint(Contact *data, int *indexarr) {
    for (int i = 0; i < contact_count; ++i) {
        if (i == 0) {
                 printf("First name");
            spaceprint(21);
            printf("Last name");
            spaceprint(22);
            printf("Email");
            spaceprint(36);
            printf("Address");
            spaceprint(23);
            printf("Mobile");
            spaceprint(7);
            printf("DOB\n");;}
        if (indexarr[i] != -1) {
             printf("%s", data[i].firstn);
        spaceprint(31 - strcspn(data[i].firstn, "\0"));
        printf("%s", data[i].lastn);
        spaceprint(31 - strcspn(data[i].lastn, "\0"));
        printf("%s", data[i].mail);
        spaceprint(41 - strcspn(data[i].mail, "\0"));
        printf("%s", data[i].adrs);
        spaceprint(30 - strcspn(data[i].adrs, "\0"));
        printf("%s", data[i].phone);
        spaceprint(13 - strcspn(data[i].phone, "\0"));
        printf("%hu/%hu/%hu\n\n", data[i].dob.day, data[i].dob.month, data[i].dob.year);
//            printf("%s", data[indexarr[i]].firstn);
//            printf("\t%s", data[indexarr[i]].lastn);
//            printf("\t%s", data[indexarr[i]].mail);
//            printf("\t%s", data[indexarr[i]].adrs);
//            printf("\t%s", data[i].phone);
//            printf("\t%hu/%hu/%hu\n\n", data[indexarr[i]].dob.day, data[indexarr[i]].dob.month,
//                   data[indexarr[i]].dob.year);
        }
    }

}

int *querycontact(const Contact *data, char mode,
                  char *searchfor) { //returns array of indices that match your needed mode search target
    int *indexarr = malloc(sizeof(int) * contact_count);
    for (int i = 0; i < contact_count; ++i) {
        indexarr[i] = -1;
    }
    if (mode == '1') {
        for (int i = 0; i < contact_count; ++i) {

            if(!(stricmp(data[i].firstn,searchfor))){
             indexarr[i]=i;
            }

        }
        return indexarr;
    }
    if (mode == '2') {
        for (int i = 0; i < contact_count; ++i) {


            if(!(stricmp(data[i].lastn,searchfor))){
             indexarr[i]=i;
            }

        }
        return indexarr;
    }
    if (mode == '3') {
           searchfor=strcat(searchfor," ");
        for (int i = 0; i < contact_count; ++i) {
            if (!(strcmp(data[i].phone, searchfor))) {
                indexarr[i] = i;
            }
        }
        return indexarr;
    }
    return indexarr;
}

void home(Contact *data, FILE *plog) {
    data = initlog(load_create('0'));
    clrscr();
    puts("\t\t\t\tMain Menu\n\r1.Print Book\t\t\t\t\t2.Query Contact\n3.Modify Contact\t\t\t\t4.Delete Contact\n5.Sort Book\t\t\t\t\t6.Add Contacts\n\t\t\t\t7.Exit");
    char choice = getchar();
    stdinclr();
    switch (choice) {
        case '1':
            clrscr();
            logprint(data);
            home(data, plog);
            break;
        case '2': {
            char searchmode;
            do {
                puts("1.firstname\t2.lastname\t3.mobilenumber");
                searchmode = getchar();
                stdinclr();
            } while (searchmode != '1' && searchmode != '2' && searchmode != '3');
            char *searchfor = malloc(sizeof(char) * 31);
            fgets(searchfor, 31, stdin);
            searchfor[strcspn(searchfor, "\n")] = '\0';
            int *indexarr = querycontact(data, searchmode, searchfor);
            int indexsum = 0;
            for (int i = 0; i < contact_count; i++) {
                indexsum = indexsum + indexarr[i];
            }
            if (indexsum == -contact_count) {
                printf("Not Found\n");
            } else contactprint(data, indexarr);
            char choice2;
            do {
                puts("return to home?Y?");
                choice2 = getchar();
                stdinclr();
            } while (tolower(choice2) != 'y');
            free(searchfor);
            home(data, plog);
            break;
        }
        case '3':
            modifycontact(data);
            home(data, plog);
            break;
        case '4':
            deletecontact(data, plog);
            home(initlog(plog), plog);
            break;
        case '5':
            clrscr();
            sortbook(data);
            filemode = 2;
            contactsave(data, contact_count, fopen(filepath, "r"));
            home(data, plog);
            break;
        case '6':
            filemode = 3;
            contactadd(fopen(filepath, "r+"));
            free(data);
            clrscr();
            home(data, plog);
            break;
        case '7':
            return;
        default:
            clrscr();
            home(data, plog);
    }

}

void deletecontact(Contact *data, FILE *plog) {
    int indxdel=-1;
    char searchmode;
    do {
        puts("1.lastname 2.mobile number");
        searchmode = getchar();
        stdinclr();
    } while (searchmode != '1'&&searchmode!='2');
    sprintf(&searchmode, "%d", atoi(&searchmode) + 1);
    char *searchfor = malloc(sizeof(char) * 31);
    fgets(searchfor, 31, stdin);
    searchfor[strcspn(searchfor, "\n")] = '\0';
    int *indexarr = querycontact(data, searchmode, searchfor);
    for (int i = 0; i < contact_count; i++) {
        if (indexarr[i] != -1) indxdel = indexarr[i];
    }
    if (indxdel == -1) {
        printf("Not Found\n");
    } else {
        int newcont = contact_count - 1;
        char choice;
        do {
            puts("1.Delete this? 2.Return to Home");
            contactprint(data, indexarr);
            choice = getchar();
            stdinclr();
        } while (choice != '1' && choice != '2');
        if (tolower(choice) == '1') {
            data[indxdel]=data[contact_count-1];
            filemode=2;
            contactsave(data,newcont,plog);
        } else return;
    }
}

void modifycontact(Contact *data) {
    int indxmdfy=-1;
    char searchmode;
    do {
        puts("1.lastname\t2.mobilenumber");
        searchmode = getchar();
        stdinclr();
    } while (searchmode != '1' && searchmode != '2');
    sprintf(&searchmode, "%d", atoi(&searchmode) + 1);
    char *searchfor = malloc(sizeof(char) * 31);
    fgets(searchfor, 31, stdin);
    searchfor[strcspn(searchfor, "\n")] = '\0';
    int *indexarr = querycontact(data, searchmode, searchfor);
    for (int i = 0; i < contact_count; i++) {
        if (indexarr[i] != -1) indxmdfy = indexarr[i];
    }
    if (indxmdfy == -1) {
        printf("Not Found\n");
    } else {
        char choice2;
        do{
            puts("1.Modify this 2.Return to Home");
            contactprint(data,indexarr);
            choice2=getchar();
            stdinclr();
        }while(choice2!='1'&&choice2!='2');
        if(choice2=='2') return;
        else{
            do {
            puts("Write First Name:");
            fgets((data[indxmdfy].firstn), 31, stdin);
        } while (!isstring(data[indxmdfy].firstn));
        data[indxmdfy].firstn[strcspn(data[indxmdfy].firstn, "\n")] = '\0';
        do {
            puts("Write Last Name:");
            fgets((data[indxmdfy].lastn), 31, stdin);
        } while (!isstring(data[indxmdfy].lastn));
        data[indxmdfy].lastn[strcspn(data[indxmdfy].lastn, "\n")] = '\0';
        do {
            puts("Mobile Number:");
            fgets((data[indxmdfy].phone), 14, stdin);
        } while (!isphone(data[indxmdfy].phone));
        data[indxmdfy].phone[strcspn(data[indxmdfy].phone, "\n")] = '\0';
        puts("Address:");
        fgets((data[indxmdfy].adrs), 50, stdin);
        data[indxmdfy].adrs[strcspn(data[indxmdfy].adrs, "\n")] = '\0';
        do {
            puts("Email:");
            fgets((data[indxmdfy].mail), 41, stdin);
        } while (!strstr(data[indxmdfy].mail, "@"));
        data[indxmdfy].mail[strcspn(data[indxmdfy].mail, "\n")] = '\0';
        do {
            puts("DOB dd/mm/yy");
            scanf("%hu/%hu/%hu", &data[indxmdfy].dob.day, &data[indxmdfy].dob.month,
                  &data[indxmdfy].dob.year);
            stdinclr();
        } while (!isdate(data[indxmdfy].dob.day, data[indxmdfy].dob.month, data[indxmdfy].dob.year));
        }

        char choice;
        do {
            puts("Y save your changes or N Discard them\n");
            choice = getchar();
            stdinclr();
        } while (tolower(choice) != 'y' && tolower(choice) != 'n');
        if (tolower(choice) == 'y') {
            FILE *target = fopen(filepath, "r+");

            fscanf(target, "%d", &contact_count);
            fseek(target, 2, SEEK_CUR);
            fseek(target, indxmdfy*182, SEEK_CUR);
            fprintf(target, "%-31s", data[indxmdfy].firstn);
            fprintf(target, "%c", ',');
            fprintf(target, "%-31s", data[indxmdfy].lastn);
            fprintf(target, "%c", ',');
            fprintf(target, "%-12s", data[indxmdfy].phone);
            fprintf(target, "%c", ',');
            fprintf(target, "%-41s", data[indxmdfy].mail);
            fprintf(target, "%c", ',');
            fprintf(target, "%-50s", data[indxmdfy].adrs);
            fprintf(target, "%c", ',');
            fprintf(target, "%2d", data[indxmdfy].dob.day);
            fprintf(target, "%c", '/');
            fprintf(target, "%2d", data[indxmdfy].dob.month);
            fprintf(target, "%c", '/');
            fprintf(target, "%4d", data[indxmdfy].dob.year);
            fflush(target);
            fclose(target);
        } else return;
    }
}

void sortbook(Contact *data) {
    sortpriority = 0;
    qsort(data, contact_count, sizeof(Contact), qsorthack);
    sortpriority = 0;

}

int qsorthack(const void *a, const void *b) {
    if (sortpriority == '1' || sortpriority == '2' || sortpriority == '3');
    else
        do {
            puts("1.first name\t2.last name\t3.Date of Birth");
            sortpriority = getchar();
            stdinclr();
        } while (sortpriority != '1' && sortpriority != '2' && sortpriority != '3');
    return contactcmpr(*(Contact *) a, *(Contact *) b, sortpriority);
}

int contactcmpr(Contact a, Contact b, char sp) {
    switch (sp) {
        case '1':


        if(stricmp(a.firstn,b.firstn)<0)
                return -1;
            else if (stricmp(a.firstn,b.firstn)>0)
                return 1;
            else {
                if (stricmp(a.lastn, b.lastn) < 0)
                    return -1;
                else if (stricmp(a.lastn, b.lastn) > 0)
                    return 1;
                else
                    return 0;
            }

        case '2':

        if(stricmp(a.lastn,b.lastn)<0)
                return -1;
            else if (stricmp(a.lastn,b.lastn)>0)
                return 1;
            else {
                if (stricmp(a.firstn, b.firstn) < 0)
                    return -1;
                else if (stricmp(a.firstn, b.firstn) > 0)
                    return 1;
                else
                    return 0;
            }

        case '3':
            if (a.dob.year < b.dob.year) return 1;
            else if (a.dob.year > b.dob.year) return -1;
            else {
                if (a.dob.month < b.dob.month) return 1;
                else if (a.dob.month > b.dob.month) return -1;
                else {
                    if (a.dob.day < b.dob.day) return 1;
                    else if (a.dob.day > b.dob.day) return -1;
                    else return 0;
                }
            }
        default:
            break;
    }

    return 0;
}
int isstring(char *tocheck) { //returns 0 if any non-alpha char is found
    int length = strlen(tocheck);
    for (int i = 0; i < length - 1; i++) {
        if (isalpha(tocheck[i]) == 0) {
            return 0;
        }
    }
    return 1;
}

int isdate(int day, int month, int year) {
    //1 if correct
    if (year < 2020 && year > 1900 && month >= 0 && month <= 12 && day >= 1 && day <= 31) {
        if (year % 4 == 0 && month == 2 && day == 29) { //leap year february

            return 1;
        }
        if (month == 2 && day <= 28) { //28 days february

            return 1;
        }
        if ((month <= 7 && month % 2 == 1 && day <= 31) || (month <= 7 && month % 2 == 0 && day <= 30 && month != 2) ||
            (month >= 8 && month % 2 == 0 && day <= 31) ||
            (month >= 8 && month % 2 == 1 && day <= 30)) { //i can't think of any another possible date instance

            return 1;
        }
    }
    return 0;
}

int isphone(char *phone) { //1 if phone,0 if any element is not an number
    int length = strlen(phone);
    for (int i = 0; i < length - 1; i++) {
        if (!isdigit(phone[i])) {
            return 0;
        }
    }
    return 1;
}

