#pragma once

# include <sys/types.h>
# include <sys/socket.h>
# include <cstring>
# include <unistd.h>
# include <string>
# include <netinet/in.h>
# include "webserv.hpp"
# include <poll.h>

# define MAX_CLIENTS 100

typedef struct s_socket t_socket;
typedef struct s_config t_config;
typedef struct s_info_client t_info_client;

struct s_socket {
	int server_fd;
	struct sockaddr_in server_addr;
	int client_fd;
	struct sockaddr_in client_addr;
	socklen_t client_len;
};

struct s_info_client {
	std::string method;
	std::string	url;
	std::string host;
};


// utilsSocket.cpp
sockaddr_in	init_sockaddr_in(t_config serverConfig);
std::string	readHtml(std::string index);
void		handleDeconnexionClient(int i, struct pollfd *clients);
void		checkEmptyPlace(t_socket &socketConfig, struct pollfd *clients);
void	parseBuffer(char *buffer, t_info_client &buffClient);

/* srvSocket.cpp */
int		handlePollin(t_socket &socketConfig, struct pollfd *clients, int i, int &client_count);
void	initSocket(t_socket &socketConfig, t_config serverConfig, struct pollfd *clients);
void	handleSocket(t_config serverConfig, t_socket &socketConfig);
