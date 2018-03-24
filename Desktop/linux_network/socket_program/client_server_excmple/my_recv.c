#define __MY_RECV_C
 
#include "my_recv.h"

void my_error(const char *err_string, int line)
{
	fprintf(stderr, "line:%d  ", line);
	perror(err_string);
	exit(1);
}

int my_recv(int conn_fd, char *data_buf, int len)
{
	static char recv_buf[BUF_SIZE];
	static char *pread;				//position of next time read data
	static int len_remain = 0;
	int i;

	//if hava nothing in buffer, read data from socket
	if(len_remain <= 0)
	{
		if((len_remain = recv(conn_fd, recv_buf, sizeof(recv_buf), 0)) < 0)
		{
			my_error("recv", __LINE__);
		}
		else if(len_remain == 0)
		{
			return 0;
		}

		pread = recv_buf;
	}

	//read data from buffer
	for (i = 0; *pread != '\n'; ++i)
	{
		if(i > len)
		{
			return -1;
		}
		data_buf[i] = *pread++;
		len_remain--;
	}

	//handle tail
	len_remain--;
	pread++;

	return i;
}