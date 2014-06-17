#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <unistd.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
	int i;
	int fp;
	char s[2000];
		
	// Allumage automatique !
	while(1)
	{
		printf("Allumage automatique du Ciscrea...\n");
		
		for(i = 0 ; i < 2000 ; i ++)
			s[i] = 's';
		
		for(i = 0 ; i < 2000 ; i ++)
			printf("r");
		
		printf("\n");
		fp = open("/dev/ttyUSB0", O_RDWR);
		printf("%d\n", (int)fp);
		for(i = 0 ; i < 1 ; i ++)
		{
			write(fp, s, 2000);
			printf("\t%d\n", i);
		}
		close(fp);
		sleep(5);
	}
	
	return EXIT_SUCCESS;
}
