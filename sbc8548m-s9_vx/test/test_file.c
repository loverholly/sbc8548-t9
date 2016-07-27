#include <vxWorks.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef void (*func_t)(void *, void *);

typedef struct test_func
{
	func_t func;
	void *arg1;
	void *arg2;
}test_func_t;


test_func_t *test=NULL;
unsigned int tbl = 0;
unsigned int tbh = 0;

void test_file_cost(char *filename, unsigned int filesize)
{
	int fd;
	vxTimeBaseGet(&tbh, &tbl);
	fd = open(filename, 0x202,0x777);
	write(fd, 0x4300,filesize);
	close(fd);
	tbh = cpu_time_diff_ms(tbl);
	printf("this file write cost %d ms\r\n", tbh);

	return;
}


