#include "helper.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>

void initPaths()
{
	//check if paths.txt exists in the $HOME directory.
	DIR *dir = opendir(getenv("HOME"));
	if(!dir)
		errExit("couldn't open home directory\n");
	struct dirent * entry;

	while(entry = readdir(dir))
	{
		if(entry->d_type == DT_REG && strcmp("paths.txt", entry->d_name) == 0)
		{
			//paths.txt exists.
			return;
		}
	}
	char filePath[MAX_CWDPATH_SIZE];
	strcpy(filePath, getenv("HOME"));
	strcat(filePath, "/paths.txt");

	FILE *fptr = fopen(filePath, "w");

	fprintf(fptr, "%s ", ".");
	fprintf(fptr, "%s ", "/bin");
	fprintf(fptr, "%s ", "/usr/bin");
	fprintf(fptr, "%s ", "/usr/sbin");

	fclose(fptr);
}
void errExit(char *message)
{
	fprintf(stderr, "%s", message);
	fprintf(stderr, "%s", strerror(errno));
	exit(-1);
}
void printErr(char *message)
{
	fprintf(stderr, "%s", message);
	fprintf(stderr, "%s", strerror(errno));
}

int findExecutable(char *name, char *buf)
{
	//this function tries to find the given executable in the directories listed in paths.txt
	
	char pathsFilePath[MAX_CWDPATH_SIZE];
	strcpy(pathsFilePath, getenv("HOME"));
	strcat(pathsFilePath, "/paths.txt");

	FILE *fptr = fopen(pathsFilePath, "r");
	if(!fptr)
		errExit("paths.txt not found!");

	char currentDirectoryPath[MAX_CWDPATH_SIZE];
	while(fscanf(fptr, "%s", currentDirectoryPath) != EOF)
	{
		DIR *thisDir = opendir(currentDirectoryPath);
		if(!thisDir)
		{
			errExit("Couldn't open directory\n");
		}
		struct dirent *currentEntry;

		while(currentEntry = readdir(thisDir))
		{
			if(strcmp(currentEntry->d_name, name) == 0)
			{
				//name has matched but check if it is a file.
				if(currentEntry->d_type == DT_REG)
				{
					//this is the correct directory.
					strcpy(buf, currentDirectoryPath);
					return 0;
				}
			}
		}
		closedir(thisDir);
	}
	fclose(fptr);
	return -1;
}
int handleShellCommand(char *cmds[])
{
	if(strcmp(cmds[0], "exit") == 0)
	{
		kill(getppid(), SIGTERM);
		exit(0);
	}
	if(strcmp(cmds[0], "cd") == 0)
	{
		chdir(cmds[1]);
		return 0;
	}
	fprintf(stderr, "Command not found!\n");
}

void printPrompt()
{
	char username[MAX_USERNAME_SIZE];
	char hostname[MAX_USERNAME_SIZE];
	char cwdPath[MAX_CWDPATH_SIZE];

	if(gethostname(hostname, MAX_HOSTNAME_SIZE) < 0)
		errExit("gethostname failed\n");
	if(getlogin_r(username, MAX_USERNAME_SIZE) < 0)
		errExit("getlogin_r failed\n");
	if(getcwd(cwdPath, MAX_CWDPATH_SIZE) == NULL)
		errExit("getcwd failed\n");

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
int tokenize(char *string, char **argv, int len)
{
	char *token = strtok(string, " ");
	int i = 0;
	argv[i++] = token;
	
	while(token = strtok(NULL, " "))
	{
		if(i >= len) return -1;
		argv[i++] = token;
	}
	
	argv[i] = NULL;

	return 0;
}
char * revStr(char *str)
{
	int left = 0, right = strlen(str) - 1;
	while(left < right)
	{
		char temp = str[left];
		str[left] = str[right];
		str[right] = temp;
		left++;
		right--;
	}
	return str;
}
