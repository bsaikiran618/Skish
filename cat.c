#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char * argv[])
{

	fprintf(stderr, "=== Skish cat ===\n");

	if(argc == 1)
	{
		//there is no file to read from except stdin
		char c;
		while((c = fgetc(stdin)) != EOF)
			printf("%c", c);
	}
	else
	{
		//for each file in argv, read and print the output
		for(int i = 1; i < argc; i++)
		{
			FILE * ptr = fopen(argv[i], "r");
			if(!ptr)
			{
				fprintf(stderr, "no such file: %s\n", argv[i]);
			}
			else
			{
				fprintf(stderr, "READING FILE: %s\n", argv[i]);
				char c;
				while((c = fgetc(ptr)) != EOF)
					printf("%c", c);
			}
			fclose(ptr);
		}
	}
	return 0;
}
