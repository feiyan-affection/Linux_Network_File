#include "my_recv.h"

#define SERV_PORT 4507		//server port
#define LISTEN_QUEUE 12		//queue of listen max number
#define INVALID_USERINFO 'n' //invalid user information
#define VALID_USERINFO 'y'	//valid user infomation
#define USER_NAME 0			//user's name received
#define PASSWORD  1			//user's password received
 
//it is used to save user's informaiton, example name and pssword
struct userinfo
{
	char username[32];
	char password[32];
};

struct userinfo users[] = 
{
	{"linux", "unix"},
	{"4507", "4508"},
	{"feiyan", "feiyan"},
	{"xixi", "xixi"},
	{" ", " "}
};

//search user's name whether exist in list
//if yes, return the number
//if no, return -1
//if error, return -2
int find_name(const char *name)
{

	if(name == NULL)
	{
		printf("in find_name, NULL pointer");
		return -2;
	}

	for (int i = 0; users[i].username[0] != ' '; ++i)
	{
		if(strcmp(users[i].username, name) == 0)
		{
			return i;
		}
	}

	return -1;
}

//send data
void send_data(int conn_fd, const char *string)
{
	if(send(conn_fd, string, strlen(string), 0) < 0)
	{
		my_error("send", __LINE__);
	}
}

int main(int argc, char *const argv[], char *const environ[])
{
	int sock_fd, conn_fd;
	int optval;
	int flag_recv = USER_NAME;
	int ret;
	int name_num;
	pid_t pid;
	socklen_t cli_addr_len;
	struct sockaddr_in cli_addr, serv_addr;
	char recv_buf[128];

	if(argc != 3)
	{
		printf("Usage: %s [-p] [serv_port]\n", argv[0]);
		exit(0);
	}
	else if(strncmp("-p", argv[1], 2) != 0)
	{
		printf("Usage: %s [-p] [serv_port]\n", argv[0]);
		exit(0);
	}


	//create TCP socket
	if((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		my_error("socket", __LINE__);
	}

	//set socket
	optval = 1;
	if(setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, (void *)&optval, sizeof(int)) < 0)
	{
		my_error("setsockopt", __LINE__);
	}

	//initlization server's addr struct
	memset(&serv_addr, 0, sizeof(struct sockaddr_in));
	serv_addr.sin_family = AF_INET;
	//serv_addr.sin_port = htons(SERV_PORT);
	serv_addr.sin_port = htons(atoi(argv[2]));
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	//bind the socket to local
	if(bind(sock_fd, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr_in)) < 0)
	{
		my_error("bind", __LINE__);
	}

	//transform sock_fd to listen's sock_fd
	if(listen(sock_fd, LISTEN_QUEUE) < 0)
	{
		my_error("listen", __LINE__);
	}

	cli_addr_len = sizeof(struct sockaddr_in);
	while(1)
	{
		conn_fd = accept(sock_fd, (struct sockaddr *)&cli_addr, &cli_addr_len);
		if(conn_fd < 0)
		{
			my_error("accept", __LINE__);
		}

		printf("accept a new client, ip:%s\n", inet_ntoa(cli_addr.sin_addr));

		//create a child process to handle reauest
		if((pid = fork()) == 0)
		{
			printf("The child process is servering now.\n");
			while(1)
			{
				if((ret = recv(conn_fd, recv_buf, sizeof(recv_buf), 0)) < 0)
				{
					my_error("recv", __LINE__);
				}
				recv_buf[ret - 1] = '\0';

				if(flag_recv == USER_NAME)
				{
					name_num = find_name(recv_buf);
					switch(name_num)
					{
						case -1:
							send_data(conn_fd, "n\n");
							break;
						case -2:
							exit(1);
							break;
						default:
							send_data(conn_fd, "y\n");
							flag_recv = PASSWORD;
							break;
					}
				}
				else if(flag_recv == PASSWORD)
				{
					if(strcmp(users[name_num].password, recv_buf) == 0)
					{
						send_data(conn_fd, "y\n");
						send_data(conn_fd, "welcome login my TCP server\n");
						printf("%s login.\n", users[name_num].username);

						//have a talk show
						/*
						if(execve("/home/feiyan/code_file/linux_practice_file/socket_program/network_security_practice/server_tcp",
							argv, environ) < 0)
						{
							my_error("execve", __LINE__);
						}
						*/
						while(1)
						{
							memset(recv_buf, 0, BUF_SIZE);
							fflush(stdin);
							fflush(stdout);
							if(recv(conn_fd, recv_buf, sizeof(recv_buf), 0) < 0)
							{
								my_error("recv", __LINE__);
							}

							if(strncmp("exit", recv_buf, 4) == 0)
							{
								printf("\tClient: exit!\n");
								break;
							}
							printf("\tClient: %s\n", recv_buf);

							memset(recv_buf, 0, BUF_SIZE);
							printf("\tServer: ");
							fgets(recv_buf, BUF_SIZE, stdin);

							if(send(conn_fd, recv_buf, sizeof(recv_buf), 0) < 0)
							{
								my_error("write", __LINE__);
							}

						}
						break;
					}
					else
					{
						send_data(conn_fd, "n\n");
					}
				}
			}
			
			printf("%s logout.\n", users[name_num].username);
			printf("The child process is exit.\n");
			close(sock_fd);
			close(conn_fd);
			exit(0);
		}
		else
		{
			close(conn_fd);
		}
	}

	return 0;
}