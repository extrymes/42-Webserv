#pragma once

# include <sys/types.h>
# include <sys/socket.h>
# include <cstring>
# include <unistd.h>
# include <netinet/in.h>
# include "webserv.hpp"

typedef struct s_socket t_socket;

struct s_socket {
	int server_fd;
	struct sockaddr_in server_addr;
	int client_fd;
	struct sockaddr_in client_addr;
	socklen_t client_len;
};

sockaddr_in init_sockaddr_in(std::vector<t_server> servers);
void makeSocketNonBlocking(int socket_fd);
