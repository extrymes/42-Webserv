#include "ClientRequest.hpp"
#include "socket.hpp"
#include "cgiHandler.hpp"

ClientRequest::ClientRequest() {}

ClientRequest::~ClientRequest () {}

void ClientRequest::parseBuffer(char *buffer, ssize_t size) {
	// int	flag = 0;
	ssize_t i = 0;
	(void)size;
	std::string line;
	std::istringstream infileBuff(buffer);
	if (!_headers.empty()) {
		while (std::getline(infileBuff, line) ) {
			_bodyString += line;
		}
		return ;
	}
	parseRequestHost(infileBuff, i);
	while (std::getline(infileBuff, line)) {
		i += line.size();
		if (line.size() == 1 && getValueHeader("method") == "POST")
			break;
		parseHeader(line);
	}
	for (; i < size; i++)
		_bodyString += buffer[i];
	// std::cout << "bodystring = " << _bodyString << std::endl;
}

void ClientRequest::parseRequestHost(std::istringstream &infileBuff, ssize_t &i) {
	std::string	requestLine, hostLine, method, url, protocol, host, port, tmp;
	if (!infileBuff)
		throw std::runtime_error("opening buffer failed");
	std::getline(infileBuff, requestLine);
	i += requestLine.size();
	std::istringstream iss(requestLine);
	iss >> method, iss >> url, iss >> protocol;
	if (method.empty() || url.empty() || protocol.empty())
		std::runtime_error("invalid HTTP header");
	_headers["method"] = method;
	_headers["url"] = url;
	_headers["protocol"] = protocol;
	std::getline(infileBuff, hostLine);
	i += hostLine.size();
	std::istringstream iss2(hostLine);
	iss2 >> tmp, iss2 >> std::ws;
	std::getline(iss2, host, ':');
	std::getline(iss2, port);
	_headers["host"] = host;
	_headers["port"] = port;
}

void ClientRequest::parseHeader(std::string line) {
	std::istringstream iss(line);
	std::string key, value;
	std::getline(iss, key, ':');
	std::getline(iss >> std::ws, value);
	_headers[key] = value;
}

// void ClientRequest::parseBody(std::string line) {

// 	std::istringstream iss(line);
// 	std::string key, value;
// 	while (iss) {
// 		getline(iss, key, '=');
// 		getline(iss, value, '&');
// 		// std::cout << "key = " << key << std::endl;
// 		// std::cout << "value = " << value << std::endl;
// 		_body[key] = value;
// 	}
// }

const std::string ClientRequest::getValueHeader(std::string key) {
	ssMap::iterator it = _headers.find(key);
	if (it == _headers.end())
		return "";
	return it->second;
}

ssMap ClientRequest::getHeaders() const {
	return _headers;
}

const std::string ClientRequest::getServerResponse(int i) {
	isMap::iterator it = _serverResponse.find(i);
	return it->second;
}

std::string ClientRequest::getBodyString() const {
	return _bodyString;
}

void ClientRequest::setValueHeader(std::string key, std::string value) {
	_headers[key] = value;
}

void ClientRequest::setServerResponse(std::string serverResponse, int i) {
	_serverResponse[i] = serverResponse;
}

void ClientRequest::clearServerResponse(int i) {
	_serverResponse.erase(i);
}

void ClientRequest::clearHeader() {
	_headers.clear();
}