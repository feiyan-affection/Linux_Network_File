#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUF_SIZE 256

void my_error(const char *err_string, int line)
{
	perror(err_string);
	fprintf(stderr, "line: %d, %s\n", line, strerror(errno));
	exit(0);
}

int main(int argc, char const *argv[])
{
	int sock_fd, serv_port;
	int cnt;
	struct sockaddr_in serv_addr;
	char buffer[BUF_SIZE];

	if(argc != 5)
	{
		printf("Usage: [-p] [serv_port] [-a] [serv_address]\n");
		exit(1);
	}

	//initialization server's address struct
	memset(&serv_addr, 0, sizeof(struct sockaddr_in));

	serv_addr.sin_family = AF_INET;
	//get the server's port and address from command line
	for (int i = 1; i < argc; ++i)
	{
		if(strcmp("-p", argv[i]) == 0)
		{
			serv_port = atoi(argv[i + 1]);
			if(serv_port < 0 || serv_port > 65535)
			{
				printf("invalid serv_addr.sin_port\n");
				exit(1);
			}
			else
			{
				//get the server port
				serv_addr.sin_port = htons(serv_port);
			}
			continue;
		}

		if(strcmp("-a", argv[i]) == 0)
		{
			//get server ip address
			if(inet_aton(argv[i + 1], &serv_addr.sin_addr) == 0)
			{
				printf("invalid server ip address\n");
				exit(1);
			}
			continue;
		}
	}

	if(serv_addr.sin_port == 0 || serv_addr.sin_addr.s_addr == 0)
	{
		printf("Usage: [-p] [serv_addr.sin_port] [-a] [serv_address]\n");
		exit(1);
	}

	//create a TCP socket
	if((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		my_error("socket", __LINE__);
	}

	//send a request to server
	if(connect(sock_fd, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr)) < 0)
	{
		my_error("connect", __LINE__);
	}

	while(1)
	{
		memset(buffer, 0, BUF_SIZE);
		fflush(stdin);
		printf("Client: ");
		fgets(buffer, BUF_SIZE, stdin);

		if((cnt = write(sock_fd, buffer, BUF_SIZE)) < 0)
		{
			my_error("write", __LINE__);
		}

		if(strncmp("exit", buffer, 4) == 0)
		{
			printf("Client: bye!\n");
			break;
		}


		memset(buffer, 0, BUF_SIZE);
		if((cnt = read(sock_fd, buffer, BUF_SIZE)) < 0)
		{
			my_error("read", __LINE__);
		}
		printf("Server: %s\n", buffer);
	}

	close(sock_fd);

	return 0;
}
