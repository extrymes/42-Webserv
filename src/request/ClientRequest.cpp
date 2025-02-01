#include "ClientRequest.hpp"
#include "socket.hpp"
#include "cgiHandler.hpp"

ClientRequest::ClientRequest() {}

ClientRequest::~ClientRequest () {}

void ClientRequest::parseBuffer(char *buffer) {
	int	flag = 0;
	std::string line;
	std::istringstream infileBuff(buffer);
	parseRequestHost(infileBuff);
	while (std::getline(infileBuff, line) ) {
		if (line.size() == 1 && getValueHeader("method") == "POST") {
			flag = 1;
			continue;
		}
		flag == 0 ? parseHeader(line) : parseBody(line);
	}
}

void ClientRequest::parseRequestHost(std::istringstream &infileBuff) {
	std::string	requestLine, hostLine, method, url, protocol, host, port, tmp;
	if (!infileBuff)
		throw std::runtime_error("opening buffer failed");
	std::getline(infileBuff, requestLine);
	std::istringstream iss(requestLine);
	iss >> method, iss >> url, iss >> protocol;
	if (method.empty() || url.empty() || protocol.empty())
		std::runtime_error("invalid HTTP header");
	// if (method != "GET" && method != "POST" && method != "DELETE") {
	// 	std::cout << "buffer = " << infileBuff.str() << std::endl;
	// 	throw std::runtime_error("invalid HTTP method");
	// }
	_headers["method"] = method;
	_headers["url"] = url;
	_headers["protocol"] = protocol;
	std::getline(infileBuff, hostLine);
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

void ClientRequest::parseBody(std::string line) {
	std::istringstream iss(line);
	std::string key, value;
	while (getline(iss, key, '=')) {
		getline(iss, value, '&');
		_body[key] = value;
	}
}

const std::string ClientRequest::getValueHeader(std::string key) {
	ssMap::iterator it = _headers.find(key);
	if (it == _headers.end())
		return "";
	return it->second;
}

ssMap ClientRequest::getHeaders() const {
	return _headers;
}

ssMap ClientRequest::getBody() const {
	return _body;
}

const std::string ClientRequest::getServerResponse(int i) {
	isMap::iterator it = _serverResponse.find(i);
	return it->second;
}

const std::string ClientRequest::getValueBody(std::string key) {
	ssMap::iterator it = _body.find(key);
	if (it == _body.end())
		return "";
	return it->second;
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

void ClientRequest::clearBody() {
	_body.clear();
}

void ClientRequest::clearHeader() {
	_headers.clear();
}

void ClientRequest::clearBuff() {
	clearBody();
	clearHeader();
}
