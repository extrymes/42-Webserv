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

struct s_info_client {
	std::string method;
	std::string	url;
	std::string host;
	std::string responseServer;
};

struct s_socket {
	int server_fd;
	struct sockaddr_in server_addr;
	int client_fd;
	struct sockaddr_in client_addr;
	socklen_t client_len;
	s_info_client buffClient;
};



/* utilsSocket.hpp */
sockaddr_in init_sockaddr_in(std::vector<t_server> servers);
std::string readHtml(std::string &index, std::vector<t_server> servers, std::string ext);
void		handleDeconnexionClient(int i, struct pollfd *clients);
void		checkEmptyPlace(t_socket &socketConfig, struct pollfd *clients);
void		parseBuffer(char *buffer, t_info_client &buffClient);

/* srvSocket.cpp */
std::string checkExt(std::string file);
int 		handlePollout(t_socket &socketConfig, struct pollfd *clients, int i, std::vector<t_server> servers);
int			handlePollin(t_socket &socketConfig, struct pollfd *clients, int i, int &client_count, std::vector<t_server> servers);
void		initSocket(t_socket &socketConfig, std::vector<t_server> servers, struct pollfd *clients);
void 		handleSocket(std::vector<t_server> servers, t_socket &socketConfig);