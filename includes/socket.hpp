#pragma once
# include <sys/types.h>
# include <sys/socket.h>
# include <cstring>
# include <unistd.h>
# include <string>
# include <netinet/in.h>
# include <poll.h>
# include <vector>
# include <iostream>
# include <csignal>
# include "ParseConfig.hpp"
# include "ClientRequest.hpp"
# include "signal.hpp"
# include "colors.h"
# include <fstream>
# include <sstream>
# include <exception>
# include <algorithm>
# include <fcntl.h>
# include <sstream>
# include <map>
# include <arpa/inet.h>
# include <netdb.h>
# include <cstdio>

# define MAX_CLIENTS 100

// --- Structures ---
typedef struct s_socket t_socket;
typedef struct s_config t_config;
typedef struct s_info_client t_info_client;

struct s_socket {
	std::vector<int> serverFd;
	struct sockaddr_in clientAddr;
	socklen_t clientLen;
	int clientCount;
	struct pollfd clients[MAX_CLIENTS];
};

// --- Functions ---
// socketUtils.hpp
void init_addrinfo(std::vector<t_server> servers, int i, struct addrinfo *hints, struct addrinfo **res);
std::string readHtml(std::string index, std::vector<t_server>::iterator server);
std::string httpResponse(std::string file, std::string ext, std::string code);
void handleClientDisconnection(int i, struct pollfd *clients);
void checkEmptyPlace(t_socket &socketConfig, struct pollfd *clients, int server_fd);
void addIndexOrUrl(std::vector<t_server>::iterator server, std::vector<std::string> indexes, ClientRequest &clientRequest, std::string &path);
std::string toString(int nbr);
std::string	removeFirstSlash (std::string str);
std::string	handleDeleteMethod(std::string file);

// socket.cpp
std::string checkExt(std::string file);
int handlePollout(t_socket &socketConfig, ClientRequest &clientRequest, int i);
std::vector<t_server>::iterator findIf(std::string port, std::vector<t_server> &servers);
std::vector<t_location>::iterator whichLocation(std::vector<t_server>::iterator it, ClientRequest &clientRequest, std::string clientUrl);
int handlePollin(t_socket &socketConfig, std::vector<t_server> servers, ClientRequest &clientRequest, int i);
void initSocket(t_socket &socketConfig, std::vector<t_server> servers);
void handleSocket(std::vector<t_server> servers, t_socket &socketConfig);
bool isCGIFile(std::string url);
