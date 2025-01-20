#pragma once

#include <sys/types.h>
#include <sys/socket.h>
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <netinet/in.h>

typedef struct s_socket t_socket;

struct s_socket {
	int server_fd;
	sockaddr_in server_addr;
	int client_fd;
	sockaddr_in client_addr;
};

sockaddr_in init_sockaddr_in(t_config serverConfig);
