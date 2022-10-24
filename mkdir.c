#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

int main(int argc, char * argv[])
{
	fprintf(stderr, "=== Skish mkdir ===\n");
	if(argc != 2)
	{
		fprintf(stderr, "Usage: %s <pathname of the new directory>\n", argv[0]);
		return -1;
	}
	else
	{
		int ret = mkdir(argv[1], 0777);
		if(ret < 0)
		{
			fprintf(stderr, "Directory creation failed for %s\n", argv[1]);
			fprintf(stderr, "%s\n", strerror(errno));
			return -1;
		}
	}
	return 0;
}
