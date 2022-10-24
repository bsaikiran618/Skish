#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include <dirent.h>

#define COLS 10

void listDirectoryContents(char * directoryPath)
{
	struct stat statbuf;
	int ret = stat(directoryPath, &statbuf);

	if(ret < 0)
	{
		fprintf(stderr, "stat() failed!\n");
		fprintf(stderr, "%s\n", strerror(errno));
		exit(-1);
	}
	else
	{
		if((statbuf.st_mode & S_IFMT) == S_IFDIR)
		{
			//read the dir content
			DIR * dir = opendir(directoryPath);
			if(!dir)
			{
				fprintf(stderr, "Couldn't open directory %s\n", directoryPath);
				fprintf(stderr, "%s", strerror(errno));
			}
			else
			{
				struct dirent * entry;
				int i = 0;

				while(entry = readdir(dir))
				{
					if(entry->d_type == DT_DIR) //print directories in blue
						printf("\033[0;34m");
					printf("%s\033[0;37m", entry->d_name);

					i++;

					if(i == COLS)
					{
						i = 0;
						printf("\n");
					}
					else printf("\t");
				}
				printf("\n");
				closedir(dir);
			}
		} 
		else
		{
			//The path did not lead to a directory
			fprintf(stderr, "%s is not a directory!\n", directoryPath);
			exit(-1);
		}
	}
}

int main(int argc, char *argv[])
{
	fprintf(stderr, "=== Skish ls ===\n");
	if(argc == 1)
	{
		//print contents of the current directory.
		listDirectoryContents(".");
	}
	else
	{
		//print contents of the directory specified at each argument
		for(int i = 1; i < argc; i++)
		{
			printf("%s:\n\n", argv[i]);
			listDirectoryContents(argv[i]);
		}
	}
	return 0;
}
