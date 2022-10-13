#include <stdio.h>

struct result {
	int lines = 0;
	int words = 0;
	int characters = 0;
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

	char c, prev = '\0';
	while((c = fgetc(stdin)) != EOF)
	{
		c = fgetc(stdin);
		r.characters++;
		
		if(iswhitespace(c) && prev!='\0' && !iswhitespace(prev))
		{
			if(c == '\n') r.lines++;
			if(c == ' ') r.words++;
		}
		
		prev = c;
	}

	return result;
}

int main(int argc, int *argv[])
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
				fprintf(stderr, "Failed to open file: %d\n", argv[i]);
				exit(-1);
			}

			dup2(fd, 0);
			struct result r = count();

			printf("%d\t%d\t%d\n", r.lines, r.words, r.characters);
		}
	}
	else
	{
		struct result r = count();
		printf("%d\t%d\t%d\n", r.lines, r.words, r.characters);
	}
	

	return 0;
}
