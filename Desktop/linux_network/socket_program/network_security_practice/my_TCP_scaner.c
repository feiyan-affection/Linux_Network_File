#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

//defiction a port's informaiton
typedef struct _port_segment
{
	struct in_addr dest_ip;			//target(the same as destination) ip
	unsigned short int min_port;	//the begin of port
	unsigned short int max_port;	//the max port
}port_segment;

//defiction error function by self
void my_error(const char *err_string, int line)
{
	fprintf(stderr, "line:%d", line);
	perror(err_string);
	exit(1);
}

/**
 * @parameter			
 * @return	
 * if have port is opend, return 1
 * if not, return 0
 * if have error, return -1		
 */
int do_scan(struct sockaddr_in serv_addr)
{
	int conn_fd;
	int ret;

	//create a TCP socket
	if((conn_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		my_error("socket", __LINE__);
	}

	//send a reauest to server
	if((ret = connect(conn_fd, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr))) < 0)
	{
		if(errno == ECONNREFUSED)	//destination port is not open
		{
			close(conn_fd);
			return 0;
		}
		else
		{
			close(conn_fd);
			return -1;
		}
	}
	else if(ret == 0)
	{
		printf("port %d found in %s\n", ntohs(serv_addr.sin_port), inet_ntoa(serv_addr.sin_addr));
		close(conn_fd);
		return 1;
	}

	return -1;
}

void *scanner(void *arg)
{
	unsigned short int i;
	struct sockaddr_in serv_addr;
	port_segment portinfo;

	//read port information
	memcpy(&portinfo, arg, sizeof(struct _port_segment));

	//initialization server's address
	memset(&serv_addr, 0, sizeof(struct sockaddr_in));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = portinfo.dest_ip.s_addr;

	for (i = portinfo.min_port; i <= portinfo.max_port; ++i)
	{
		serv_addr.sin_port = htons(i);
		if(do_scan(serv_addr) < 0)
		{
			break;
			//continue;	
		}
	}

	return NULL;
}

/**
 * @parameter
 * @parameter
 * @return	
 * "-m: max port"
 * "-a: ip address of destination host"
 * "-n: max counts of thread"
 */
int main(int argc, char const *argv[])
{
	pthread_t *thread;
	int max_port;
	int thread_num;	
	int seg_len;		//the length of port
	struct in_addr dest_ip;

	//check out the count of parameter
	if(argc != 7)
	{
		printf("Usage: [-m] [max_port] [-a] [serv_addr] [-n] [thread_num]\n");
		exit(1);
	}

	//get command line
	for (int i = 1; i < argc; ++i)
	{
		if(strcmp("-m", argv[i]) == 0)
		{
			max_port = atoi(argv[i + 1]);
			if(max_port < 0 || max_port > 65535)
			{
				printf("Usage: invalid max destination port\n");
				exit(1);
			}
			continue;
		}

		if(strcmp("-a", argv[i]) == 0)
		{
			if(inet_aton(argv[i + 1], &dest_ip) == 0)
			{
				printf("Usage: invalid destination ip address\n");
				exit(1);
			}
			continue;
		}

		if(strcmp("-n", argv[i]) == 0)
		{
			thread_num = atoi(argv[i + 1]);
			if(thread_num <= 0)
			{
				printf("Usage: invalid thread number\n");
				exit(1);
			}
			continue;
		}
	}

	if(max_port < thread_num)
	{
		thread_num = max_port;
	}

	seg_len = max_port / thread_num;
	if((max_port % thread_num) != 0)
	{
		thread_num += 1;
	}

	//distribution all thread ID have memory
	thread = (pthread_t *)malloc(thread_num * sizeof(pthread_t));

	//create thread
	for (int i = 0; i < thread_num; ++i)
	{
		port_segment portinfo;
		portinfo.dest_ip = dest_ip;
		portinfo.min_port = i * seg_len + 1;
		if(i == thread_num - 1)
		{
			portinfo.max_port = max_port;
		}
		else
		{
			portinfo.max_port = portinfo.min_port + seg_len - 1;
		}

		if(pthread_create(&thread[i], NULL, scanner, (void *)&portinfo) != 0)
		{
			my_error("pthread_create", __LINE__);
		}

		//maintain pthread is waitting child's thread over
		pthread_join(thread[i], NULL);
	}

	return 0;
}
