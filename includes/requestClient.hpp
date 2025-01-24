#pragma once
# include <sys/types.h>
# include <sys/socket.h>
# include <unistd.h>
# include <string>
# include <netinet/in.h>
# include "webserv.hpp"
# include <poll.h>

class RequestClient {
	public:
		RequestClient();
		~RequestClient();
		void parseBuffer(char *buffer);
		void parseFirstLines(std::stringstream &infileBuff);
		void parseHeader(std::string line);
		void setResponseServer(std::string const &response);
		std::string getMethod() const;
		std::string getUrl() const;
		std::string getPort() const;
		std::string getHost() const;
		std::string getResponseServer() const;
	private:
		std::string _method;
		std::string _url;
		std::string _host;
		std::string _port;
		std::string _responseServer;
		std::string _userAgent;
		std::string _accept;
		std::string _acceptLanguage;
		std::string _acceptEncoding;
		std::string _connection;
		std::string _upgradeInsecureRequests;
		std::string _secFetchDest;
		std::string _secFetchMode;
		std::string _secFetchSite;
		std::string _secFetchUser;
		std::string _priority;
		std::string _ty;
};
