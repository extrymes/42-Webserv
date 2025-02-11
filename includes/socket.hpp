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
# include <dirent.h>
#include <sys/stat.h>

# define MAX_CLIENTS 100

// --- Definitions ---
typedef std::vector<t_server>::iterator servIt;
typedef std::vector<t_location>::iterator locIt;

// --- Structures ---
typedef struct s_socket t_socket;
typedef struct s_config t_config;
typedef struct s_info_client t_info_client;

struct s_socket {
	std::vector<int> serverFd;
	struct sockaddr_in clientAddr;
	socklen_t clientLen;
	int clientCount;
	struct pollfd *clients;
};

// --- Functions ---

// serverRespone.cpp
std::string readHtml(std::string index, servIt server, std::string code, std::string clientUrl);
std::string httpResponse(std::string file, std::string ext, std::string code);
std::string redir(locIt &location);
std::string errorPage(servIt server, std::string code);
std::string checkExt(std::string file);
std::string errorHtml(std::string code);

// socketUtils.cpp
void initAddrInfo(std::vector<t_server> &servers, int i, struct addrinfo *hints, struct addrinfo **res);
void handleClientDisconnection(int i, struct pollfd *clients, cMap &clientMap);
void checkEmptyPlace(t_socket &socketConfig, cMap &clientMap, int server_fd);
void addIndexOrUrl(servIt server, std::vector<std::string> indexes, ClientRequest *clientRequest, std::string &path, locIt &location);
std::string toString(int nbr);
std::string	handleDeleteMethod(std::string file);
bool isMethodAllowed(std::string method, servIt server, ClientRequest *clientRequest, std::string clientUrl);
bool isCGIAllowed(std::string url, servIt server, ClientRequest *clientRequest);
std::string	uploadLocation(servIt server, ClientRequest *clientRequest);
std::string removeFirstSlash(std::string str);

// socket.cpp
int handlePollout(t_socket &socketConfig, cMap &clientMap, int i);
servIt findIf(std::string port, std::vector<t_server> &servers);
std::string createGoodUrl(std::string oldUrl);
std::string urlWithoutSlash(std::string location);
locIt whichLocation(servIt it, ClientRequest *clientRequest, std::string clientUrl, std::string str);
std::string	createUrl(servIt server, ClientRequest *clientRequest, std::string &clientUrl, locIt &location);
int checkLenBody(ClientRequest *clientRequest, servIt server, ssize_t size);
int handlePollin(t_socket &socketConfig, std::vector<t_server> &servers, cMap &clientMap, int i);
void handleGetMethod(servIt server, locIt location, ClientRequest *clientRequest, std::string clientUrl, std::string file);
void handlePostMethod(servIt server, locIt location, ClientRequest *clientRequest, std::string clientUrl, std::string file);
void handleDeleteMethod(servIt server, ClientRequest *clientRequest, std::string file);
void initSocket(t_socket &socketConfig, std::vector<t_server> &servers);
void handleSocket(std::vector<t_server> &servers, t_socket &socketConfig);
void closeAllFds(t_socket &socketConfig, cMap &clientMap);
