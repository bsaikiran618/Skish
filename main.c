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
	char *token = strtok(myCommand, " ");
	char *argv[MAX_STATEMENT_LENGTH];

	int i=0;

	argv[i++] = token;

	while((token = strtok(NULL, " ")))
	{
		argv[i++] = token;
	}
	argv[i] = NULL; //terminate the argv array with NULL

	char pathName[MAX_STATEMENT_LENGTH + MAX_CWDPATH_SIZE];
	sprintf(pathName, "/bin/%s", argv[0]); //skish only executes commands in the /bin directory.

	int ret = execv(pathName, argv);
	if(ret < 0)
	{
		fprintf(stderr, "failed to execute command: %s\n", myCommand);
	}
}

void getStatement(pid_t rootpid)
{
	//this is the root shell's loop.
	while(1)
	{
		if(getpid() == rootpid)
		{
			//print the prompt.
			printPrompt();
			fflush(stdout); //this is needed to prevent printing the prompt from the child.
			pid_t child = fork();
			if(child != 0)
			{
				//this is the root shell.
				int status;
				waitpid(child, &status, 0);
			}
			else
				break; //send the child to read input and execute.
		}
	}

	//this is the child's loop. We could have used GOTO but they say that it is bad practice.
	while(1)
	{
		fflush(stdout);
		char statement[MAX_STATEMENT_LENGTH];
		char *readStatus = fgets(statement, MAX_STATEMENT_LENGTH, stdin);

		if(!readStatus)
		{
			//kill the root shell.
			kill(rootpid, SIGTERM);
			exit(-1);
		}

		trim(statement);

		int statementLen = strlen(statement);

		char myCommand[MAX_STATEMENT_LENGTH];

		int i = 0;
		while(i < statementLen && statement[i] != '|')
		{
			myCommand[i] = statement[i];
			i++;
		}

		myCommand[i] = '\0';
		trim(myCommand);

		if(strlen(myCommand) == 0) exit(0);

		pid_t pipedBrother = -1;
		if(statement[i] == '|')
		{
			char restOfIt[MAX_STATEMENT_LENGTH];
			strcpy(restOfIt, statement + i + 1);
			trim(restOfIt);

			int restLen = strlen(restOfIt);
			restOfIt[restLen] = '\n';
			restOfIt[restLen + 1] = '\0';

			int fd[2];
			if(pipe(fd) < 0) printAndExit("pipe creation failed");

			pipedBrother = fork();
			if(pipedBrother == 0)
			{
				//close the write end of the pipe.
				close(fd[1]);
				//redirect child stdin to read from the read end of the pipe.
				dup2(fd[0], 0);
				continue; //this new child has to read its own command, so we ask it to go back to read. Hence the loop.
			}
			else
			{
				//close the read side of the pipe for parent
				close(fd[0]);
				//redirect the stdout of parent to the write end of the pipe.
				dup2(fd[1], 1);
				//send the remaining statement to the child shell.
				write(1, restOfIt, strlen(restOfIt));
			}
		}
		/*
			This child will not directly execute the command.
			Consider the case when there is a pipe: cmd1 | cmd2
			here the child which executes cmd1 may exit before the child which executes cmd2.
			since cmd2 is the child of cmd1, and since cmd1 is dead, the root shell will
			print the prompt before the cmd2 prints it output.
			therefore the rootshell has to wait for cmd1 to finish,
			similarly, cmd1 has to wait till cmd2 finishes and so on.
			The prompt must be printed only after all the processes in the pipe chain exit.
			which requires a 'wait-chain';
		*/
		if(fork())
		{
			//this procs job is to wait for the pipe brother to end.
			if(pipedBrother > 0)
			{
				int status;
				waitpid(pipedBrother,&status, 0);
			}
			exit(0);
		}
		else
			executeCommand(myCommand);
		break;
	}
}


int main(int argc, char *argv[])
{
	pid_t pid = getpid();
	getStatement(pid);
	return 0;
}
