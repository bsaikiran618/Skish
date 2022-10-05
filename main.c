#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <signal.h>

#include "helper.h"

void executeCommand(char *myCommand)
{
	/*
		We have to split the command into parts delimited by space.
		e.g:
			"foo -Aex -K"
		has to be split into ["foo", "-Aex", "-K"]
		and then we can call execve
	*/
	char *argv[MAX_STATEMENT_LENGTH];
	tokenize(myCommand, argv, MAX_STATEMENT_LENGTH);

	//argv[0] is the name of the command/executable.

	char executableDirPath[MAX_CWDPATH_SIZE];

	int executableExists = findExecutable(argv[0], executableDirPath);

	if(executableExists < 0)
	{
		//this could be a shell command.
		handleShellCommand(argv);
	}
	else
	{
		//this is an executable only.
		char pathName[MAX_STATEMENT_LENGTH + MAX_CWDPATH_SIZE];
		sprintf(pathName, "%s/%s", executableDirPath, argv[0]); //skish only executes commands in the /bin directory.

		int ret = execv(pathName, argv);
		if(ret < 0)
		{
			errExit("Failed to execute command");
		}
	}

}

int getCommand(char *myCommand, char *restOfIt, int len)
{
	char statement[MAX_STATEMENT_LENGTH];
	char *readStatus = fgets(statement, MAX_STATEMENT_LENGTH, stdin);

	if(!readStatus)
	{
		//if there was a EOF, we have to die.
		exit(-1);
	}

	trim(statement);

	int statementLen = strlen(statement);
	int i = 0, j = statementLen - 1;

	int specialCode = 0;

	while(j >= 0 && statement[j] != '|')
	{
		myCommand[i++] = statement[j--];
	}

	switch(statement[j])
	{
		case '|' : specialCode = 1; break;
		default: break;
	}

	myCommand[i] = '\0';
	statement[j] = '\0';
	trim(myCommand);

	revStr(myCommand);

	strcpy(restOfIt, statement);
	trim(restOfIt);
	
	return specialCode;
}

void runCommand(char *executableDirPath, char *argv[])
{
	char pathName[MAX_CWDPATH_SIZE];
	sprintf(pathName, "%s/%s", executableDirPath, argv[0]); //skish only executes commands in the /bin directory.
	int ret = execv(pathName, argv);

	if(ret < 0)
		errExit("Failed to execute command");
}

void getStatement()
{
	char myCommand[MAX_STATEMENT_LENGTH];
	char restOfIt[MAX_STATEMENT_LENGTH];

	int special = getCommand(myCommand, restOfIt, MAX_STATEMENT_LENGTH);

	if(strlen(myCommand) == 0) return;

	char * argv[MAX_STATEMENT_LENGTH];
	tokenize(myCommand, argv, MAX_STATEMENT_LENGTH);

	//check if this command is an executable or a shell command.
	char executableDirPath[MAX_CWDPATH_SIZE];
	int executableExists = findExecutable(argv[0], executableDirPath);

	int restLen = strlen(restOfIt);
	restOfIt[restLen] = '\n';
	restOfIt[restLen + 1] = '\0';

	if(executableExists < 0)
	{
		//this could be a shell command.
		handleShellCommand(argv);
	}
	else
	{
		pid_t child1;

		if((child1  = fork()) < 0)
		{
			printErr("fork failed!\n");
			return;
		}
		if(child1 == 0)
		{
			if(special == 1)
			{
				int mainPipe[2];
				int commandPipe[2];
				if(pipe(mainPipe) < 0)
				{
					printErr("pipe failed!\n");
					return;
				}
				if(pipe(commandPipe) < 0)
				{
					printErr("pipe failed!\n");
					return;
				}

				pid_t child2;
				if((child2 = fork()) < 0)
				{
					printErr("fork failed!\n");
					return;
				}
				if(child2 == 0)
				{
					//its output goes to mainpipe
					close(mainPipe[0]);
					dup2(mainPipe[1], 1);
				
					//it reads the input from the command pipe.
					close(commandPipe[1]);
					dup2(commandPipe[0], 0);

					getStatement();
					exit(0);
				}
				else
				{
					//this is child1 after forking child2.

					//its input comes from mainpipe.
					close(mainPipe[1]);
					dup2(mainPipe[0], 0);
					
					//write the remaining command to the commandPipe.
					close(commandPipe[0]);
					write(commandPipe[1], restOfIt, strlen(restOfIt));

					runCommand(executableDirPath, argv);
					exit(0);
				}
			}
			else
			{
				runCommand(executableDirPath, argv);
				exit(0);
			}
		}
		else
		{
			//root shell waits for child to finish.
			wait(NULL);
		}
	}
}


int main(int argc, char *argv[])
{
	while(1)
	{
		printPrompt();
		getStatement();
	}
	return 0;
}
