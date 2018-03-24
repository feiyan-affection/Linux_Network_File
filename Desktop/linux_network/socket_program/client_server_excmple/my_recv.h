#ifndef __MY_RECV_H
#define __MY_RECV_H

#define BUF_SIZE 1024

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

void my_error(const char *err_string, int line);
int my_recv(int conn_fd, char *data_buf, int lne);


#endif