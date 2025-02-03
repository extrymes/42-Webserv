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
# include "code.hpp"

# define MAX_CLIENTS 100

// --- Definitions ---
typedef std::vector<t_server>::iterator servIt;

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
void initAddrInfo(std::vector<t_server> &servers, int i, struct addrinfo *hints, struct addrinfo **res);
std::string readHtml(std::string index, servIt server, std::string code);
std::string httpResponse(std::string file, std::string ext, std::string code);
std::string redir(std::vector<t_location>::iterator &location);
std::string errorPage(int error, servIt server, std::string code);
void handleClientDisconnection(int i, struct pollfd *clients);
void checkEmptyPlace(t_socket &socketConfig, struct pollfd *clients, int server_fd);
void addIndexOrUrl(servIt server, std::vector<std::string> indexes, ClientRequest &clientRequest, std::string &path);
std::string toString(int nbr);
std::string	handleDeleteMethod(std::string file);
bool isMethodAllowed(std::string method, servIt server, ClientRequest &clientRequest);
bool isCGIAllowed(std::string url, servIt server, ClientRequest &clientRequest);

// socket.cpp
std::string checkExt(std::string file);
int handlePollout(t_socket &socketConfig, ClientRequest &clientRequest, int i);
servIt findIf(std::string port, std::vector<t_server> &servers);
std::vector<t_location>::iterator whichLocation(servIt it, ClientRequest &clientRequest, std::string clientUrl, std::string str);
int handlePollin(t_socket &socketConfig, std::vector<t_server> &servers, ClientRequest &clientRequest, int i);
void initSocket(t_socket &socketConfig, std::vector<t_server> &servers);
void handleSocket(std::vector<t_server> &servers, t_socket &socketConfig);
bool isCGIFile(std::string url);
void closeAllFds(t_socket &socketConfig);
void handleGetMethod(servIt server, ClientRequest &clientRequest, std::string clientUrl, std::string file, int i);
void handlePostMethod(servIt server, ClientRequest &clientRequest, std::string clientUrl, std::string file, int i);
void handleDeleteMethod(servIt server, ClientRequest &clientRequest, std::string file, int i);
std::string createGoodUrl(std::string oldUrl);
std::string urlWithoutSlash(std::string location);