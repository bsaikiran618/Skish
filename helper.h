#ifndef SKISH
#define SKISH

#define MAX_HOSTNAME_SIZE 256
#define MAX_USERNAME_SIZE 256
#define MAX_CWDPATH_SIZE 1024
#define MAX_STATEMENT_LENGTH 2048

void initPaths();

void errExit(char *);
void printErr(char *);

void getPrompt(char *);
int findExecutable(char *, char *);
int handleShellCommand(char*[]);
int tokenize(char *, char **, int, int *);
void handleRedirection(int *, char *[]);
char * trim(char *);
char *revStr(char *);

#endif
