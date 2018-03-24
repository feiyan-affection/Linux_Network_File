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
#define LISTEN_QUEUE 10

void my_error(const char *err_string, int line)
{
	perror(err_string);
	fprintf(stderr, "line: %d, %s\n", line, strerror(errno));
	exit(0);
}

int main(int argc, char const *argv[])
{
	int sock_fd, conn_fd;
	int serv_port;
	int cnt;
	struct sockaddr_in serv_addr, cli_addr;
	int serv_addr_len;
	char buffer[BUF_SIZE];

	if(argc != 3)
	{
		printf("Usage: %s [-p] [serv_port]\n", argv[0]);
		exit(0);
	}

	if((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		my_error("socket", __LINE__);
	}

	//initlization server's addr struct
	memset(&serv_addr, 0, sizeof(struct sockaddr_in));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(atoi(argv[2]));
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	//bind the socket to local
	if(bind(sock_fd, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr)) < 0)
	{
		my_error("bind", __LINE__);
	}

	//transform sock_fd to listen's sock_fd
	if(listen(sock_fd, LISTEN_QUEUE) < 0)
	{
		my_error("listen", __LINE__);
	}

	serv_addr_len = sizeof(struct sockaddr);
	if((conn_fd = accept(sock_fd, (struct sockaddr *)&serv_addr, &serv_addr_len)) < 0)
	{
		my_error("accept", __LINE__);
	}

	while(1)
	{
		memset(buffer, 0, BUF_SIZE);
		if((cnt = read(conn_fd, buffer, BUF_SIZE)) < 0)
		{
			my_error("read", __LINE__);
		}

		if(strncmp("exit", buffer, 4) == 0)
		{
			printf("Client: bye!\n");
		}
		printf("Client: %s\n", buffer);

		memset(buffer, 0, BUF_SIZE);
		fflush(stdin);
		printf("Server: ");
		fgets(buffer, BUF_SIZE, stdin);

		if((cnt = write(conn_fd, buffer, BUF_SIZE)) < 0)
		{
			my_error("write", __LINE__);
		}
	}

	close(sock_fd);
	close(conn_fd);
	return 0;
		
}