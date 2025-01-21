#pragma once

# include <sys/types.h>
# include <sys/socket.h>
# include <cstring>
# include <unistd.h>
# include <string>
# include <netinet/in.h>
# include "webserv.hpp"

typedef struct s_socket t_socket;
typedef struct s_config t_config;

struct s_socket {
	int server_fd;
	struct sockaddr_in server_addr;
	int client_fd;
	struct sockaddr_in client_addr;
	socklen_t client_len;
};

sockaddr_in init_sockaddr_in(t_config serverConfig);
void makeSocketNonBlocking(int socket_fd);
