#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX_PATH_SIZE 512
#define DETAILS_BUFFER_SIZE 1024


#define DETAILS_FIELDS 128

#define DETAILS_INDEX_PID 0
#define DETAILS_INDEX_PNAME 1
#define DETAILS_INDEX_PSTATE 2
#define DETAILS_INDEX_PPID 3


void printHeaders(unsigned int printMask[])
{
	if(printMask[DETAILS_INDEX_PID]) printf("PID\t");
	if(printMask[DETAILS_INDEX_PNAME]) printf("NAME\t");
	if(printMask[DETAILS_INDEX_PSTATE]) printf("STATE\t");
	if(printMask[DETAILS_INDEX_PPID]) printf("PPID\t");
}

void printDetails(char *details[], unsigned int printMask[])
{
	for(int i = 0; i < DETAILS_FIELDS && details[i]; i++)
	{
		if(printMask[i] == 1)
		{
			printf("%s\t", details[i]);
		}
	}
	printf("\n");
}

int main(int argc, char * argv[])
{
	//The following mask tells us which fields to print.
	unsigned int printMask[DETAILS_FIELDS];
	memset(printMask, 0, sizeof(unsigned int) * DETAILS_FIELDS);

	fprintf(stderr, "=== Skish ps ===\n");
	
	DIR * dir = opendir("/proc");
	if(!dir)
	{
		fprintf(stderr, "Couldn't open /proc\n%s", strerror(errno));
		exit(-1);
	}
	else
	{
		//HARDCODING THE PRINTMASK FOR NOW
		printMask[DETAILS_INDEX_PID] = 1;
		printMask[DETAILS_INDEX_PNAME] = 1;
		printMask[DETAILS_INDEX_PPID] = 1;
		printMask[DETAILS_INDEX_PSTATE] = 1;

		printHeaders(printMask);
		printf("\n");

		//start reading data
		struct dirent * entry;
		while(entry = readdir(dir))
		{
			if(entry->d_type == DT_DIR && isdigit(entry->d_name[0]))
			{
				char statFilePath[MAX_PATH_SIZE];

				//All the fields in the stat file will be read into the details buffer.
				//They will be '\0' separated.

				char detailsBuffer[DETAILS_BUFFER_SIZE];
					
				//This is an array of pointers to each field in the detailsBuffer. It will be NULL terminated.
				char * details[DETAILS_FIELDS];

				sprintf(statFilePath, "/proc/%s/stat", entry->d_name);

				FILE * fptr = fopen(statFilePath, "r");

				if(!fptr)
				{
					fprintf(stderr, "couldn't read %s\n%s", statFilePath, strerror(errno));
					continue;
				}

				//stat file has only one line.

				fgets(detailsBuffer, DETAILS_BUFFER_SIZE, fptr);

				detailsBuffer[strlen(detailsBuffer) - 1] = '\0';

				int j = 0;
				details[j++] = detailsBuffer; //The first field's pointer will be the base address of the detailsBuffer only.

				int detailsBufferLen = strlen(detailsBuffer);

				for(int i = 0; i < detailsBufferLen; i++)
				{
					if(isspace(detailsBuffer[i]))
					{
						detailsBuffer[i] = '\0';
						details[j++] = (char *) &(detailsBuffer[i+1]);
					}
				}
				
				details[j] = NULL;

				printDetails(details, printMask);
				fclose(fptr);
			}
		}
	}

	return 0;
}
