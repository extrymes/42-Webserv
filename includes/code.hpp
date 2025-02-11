#pragma once
# include "socket.hpp"
// Success responses (2xx)
#define CODE200 "200 OK"
#define CODE201 "201 Created"
#define CODE202 "202 Accepted"
#define CODE204 "204 No Content"

// Redirections (3xx)
#define CODE301 "301 Moved Permanently"
#define CODE302 "302 Found"
#define CODE303 "303 See Other"
#define CODE304 "304 Not Modified"
#define CODE307 "307 Temporary Redirect"
#define CODE308 "308 Permanent Redirect"

// Client errors (4xx)
#define CODE400 "400 Bad Request"
#define CODE401 "401 Unauthorized"
#define CODE403 "403 Forbidden"
#define CODE404 "404 Not Found"
#define CODE405 "405 Method Not Allowed"
#define CODE408 "408 Request Timeout"
#define CODE409 "409 Conflict"
#define CODE410 "410 Gone"
#define CODE411 "411 Length Required"
#define CODE413 "413 Payload Too Large"
#define CODE414 "414 URI Too Long"
#define CODE415 "415 Unsupported Media Type"
#define CODE429 "429 Too Many Requests"

// Server erros (5xx)
#define CODE500 "500 Internal Server Error"
#define CODE501 "501 Not Implemented"
#define CODE502 "502 Bad Gateway"
#define CODE503 "503 Service Unavailable"
#define CODE504 "504 Gateway Timeout"
#define CODE505 "505 HTTP Version Not Supported"

typedef std::vector<t_server>::iterator servIt;

class HttpServerException : public std::exception {
public:
	HttpServerException(const servIt server, const std::string& codeMsg, const std::string& message)
		: _server(server), _codeMsg(codeMsg), _message(message) {}

	servIt getServ() const {
		return _server;
	}

	std::string getCodeMsg() const {
		return _codeMsg;
	}

	virtual const char* what() const throw() {
		return _message.c_str();
	}

	virtual ~HttpServerException() throw() {}

private:
	servIt	_server;
	std::string	_codeMsg;
	std::string	_message;
};

class HttpException : public std::exception {
public:
	HttpException(const std::string& codeMsg, const std::string& message)
		: _codeMsg(codeMsg), _message(message) {}

	std::string getCodeMsg() const {
		return _codeMsg;
	}

	virtual const char* what() const throw() {
		return _message.c_str();
	}

	virtual ~HttpException() throw() {}

private:
	std::string	_codeMsg;
	std::string	_message;
};
