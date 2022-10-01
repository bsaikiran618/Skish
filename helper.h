#ifndef SKISH
#define SKISH

#define MAX_HOSTNAME_SIZE 256
#define MAX_USERNAME_SIZE 256
#define MAX_CWDPATH_SIZE 1024
#define MAX_STATEMENT_LENGTH 2048

void printAndExit(char *);
void printPrompt();
char * trim(char *);

#endif
