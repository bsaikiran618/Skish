#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>

struct result {
	int lines;
	int words;
	int characters;
};

int iswhitespace(char c)
{
	switch(c)
	{
		case ' ':
		case '\t':
		case '\n':
		case '\r': return 1;
		default: return 0;
	}
}

struct result count()
{
	struct result r;
	r.lines = 0;
	r.words = 0;
	r.characters = 0;

	char c, prev = '\0';
	while((c = fgetc(stdin)) != EOF)
	{
		r.characters++;
		
		if(iswhitespace(c) && prev!='\0' && !iswhitespace(prev))
		{
			if(c == '\n'){r.lines++; if(!iswhitespace(prev))r.words++;}
			if(c == ' ') r.words++;
		}
		
		prev = c;
	}
	if(c == EOF && prev!='\0' && !iswhitespace(prev)) r.words++;

	return r;
}

int main(int argc, char * argv[])
{
	//if argc > 1, read them as files.
	//else read from stdin

	if(argc > 1)
	{
		for(int i=1; i < argc; i++)
		{
			int fd = open(argv[i], O_RDONLY);
			if(fd < 0)
			{
				fprintf(stderr, "Failed to open file: %s\n", argv[i]);
				exit(-1);
			}

			dup2(fd, 0);
			struct result r = count();

			printf("\t%d\t%d\t%d\n", r.lines, r.words, r.characters);
		}
	}
	else
	{
		struct result r = count();
		printf("\t%d\t%d\t%d\n", r.lines, r.words, r.characters);
	}
	

	return 0;
}
