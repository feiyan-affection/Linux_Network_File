#include "my_recv.h"

#define INVALID_USERINFO 'n'
#define VALID_USERINFO 'y'

int get_userinfo(char *buf, int len)
{
	int  i, c;

	if(buf == NULL)
	{
		return -1;
	}

	i = 0;
	while(((c = getchar()) != '\n') && (c != EOF) && (i < len -2))
	{
		buf[i++] = c;
	}
	buf[i++] = '\n';
	buf[i] = '\0';

	return 0;
}

void input_userinfo(int conn_fd, const char *string)
{
	char input_buf[32];
	char recv_buf[BUF_SIZE];
	int flag_userinfo;

	//until the info is correct
	do
	{
		printf("%s:", string);
		if(get_userinfo(input_buf, 32) < 0)
		{
			printf("error return from get_userinfo\n");
			exit(1);
		}

		if(send(conn_fd, input_buf, strlen(input_buf), 0) < 0)
		{
			my_error("send", __LINE__);
		}

		//read data from connected's socket
		if(my_recv(conn_fd, recv_buf, sizeof(recv_buf)) < 0)
		{
			printf("data is too long\n");
			exit(1);
		}

		if(recv_buf[0] == VALID_USERINFO)
		{
			flag_userinfo = VALID_USERINFO;
		}
		else
		{
			printf("%s error, input again.\n", string);
			flag_userinfo = INVALID_USERINFO;
		}
	}while(flag_userinfo == INVALID_USERINFO);
}

int main(int argc, char *const argv[], char *const environ[])
{
	
	int i, ret;
	int conn_fd;
	int serv_port;
	struct sockaddr_in serv_addr;
	char recv_buf[BUF_SIZE];

	//chect out the count of parameter
	if(argc != 5)
	{
		printf("Usage: [-p] [serv_port] [-a] [serv_address]\n");
		exit(1);
	}

	//initialization server's address struct
	memset(&serv_addr, 0, sizeof(struct sockaddr_in));
	serv_addr.sin_family = AF_INET;
	//get the server's port and address from command line
	for (i = 1; i < argc; ++i)
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
				serv_addr.sin_port = htons(serv_port);
			}
			continue;
		}

		if(strcmp("-a", argv[i]) == 0)
		{
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
	if((conn_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		my_error("socket", __LINE__);
	}

	//send a request to server
	if(connect(conn_fd, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr)) < 0)
	{
		my_error("connect", __LINE__);
	}

	//input user's name and password
	input_userinfo(conn_fd, "username");
	input_userinfo(conn_fd, "password");

	//print welcome interface
	if((ret = my_recv(conn_fd, recv_buf, sizeof(recv_buf))) < 0)
	{
		printf("data is too long\n");
		exit(1);
	}
	recv_buf[ret] = '\0';
	printf("%s\n", recv_buf);
	/*
	if(execve("/home/feiyan/code_file/linux_practice_file/socket_program/network_security_practice/client_tcp",
			argv, environ) < 0)
	{
		my_error("execve", __LINE__);
	}
	*/
	printf(">>>please first input \"exit\" to exit server.\n");
	printf(">>>and then input \"quit\" to exit client.\n");
	while(1)
	{
		memset(recv_buf, 0, BUF_SIZE);
		fflush(stdin);
		fflush(stdout);
		printf("\tClient: ");
		fgets(recv_buf, BUF_SIZE, stdin);

		if(send(conn_fd, recv_buf, BUF_SIZE, 0) < 0)
		{
			my_error("send_data", __LINE__);
		}

		if(strncmp("quit", recv_buf, 4) == 0)
		{
			printf("\tClient: exit!\n");
			break;
		}

		memset(recv_buf, 0, BUF_SIZE);
		if(recv(conn_fd, recv_buf, BUF_SIZE, 0) < 0)
		{
			my_error("recv", __LINE__);
		}
		printf("\tServer: %s\n", recv_buf);
	}
	close(conn_fd);

	return 0;
}