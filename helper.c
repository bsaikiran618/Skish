#include "helper.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

void printAndExit(char *message)
{
	fprintf(stderr, "%s", message);
	fprintf(stderr, "%s", strerror(errno));
	exit(-1);
}

void printPrompt()
{
	char username[MAX_USERNAME_SIZE];
	char hostname[MAX_USERNAME_SIZE];
	char cwdPath[MAX_CWDPATH_SIZE];

	if(gethostname(hostname, MAX_HOSTNAME_SIZE) < 0)
		printAndExit("gethostname failed\n");
	if(getlogin_r(username, MAX_USERNAME_SIZE) < 0)
		printAndExit("getlogin_r failed\n");
	if(getcwd(cwdPath, MAX_CWDPATH_SIZE) == NULL)
		printAndExit("getcwd failed\n");

	fprintf(stdout, "\033[1;32m"); // To set color.
	fprintf(stdout, "%s", username); 
	fprintf(stdout, "\033[1;37m"); 
	fprintf(stdout, "@");
	fprintf(stdout, "\033[1;34m"); 
	fprintf(stdout, "%s", hostname);
	fprintf(stdout, "\033[1;37m"); 
	fprintf(stdout, ":");
	fprintf(stdout, "\033[1;37m"); 
	fprintf(stdout, "%s", cwdPath);
	fprintf(stdout, "\033[1;33m"); 
	fprintf(stdout, "$ ");
	fprintf(stdout, "\033[0;37m"); 
}

char * trim(char * message)
{
	//this function removes any trailing and leading whitespaces.

	char temp[MAX_STATEMENT_LENGTH];

	int i=0;
	for(;i<strlen(message) && (message[i] == ' ' || message[i] == '\t' || message[i] == '\n');i++)
	{

	}
	
	int j = strlen(message) - 1;
	for(;j>=0 && ((message[j] == ' ' || message[j] == '\t' || message[j] == '\n'));j--)
	{

	}

	int k = 0;
	while(k+i <= j)
	{
		temp[k] = message[i+k];
		k++;
	}	

	temp[k] = '\0';
	strcpy(message, temp);
	return message;
}
