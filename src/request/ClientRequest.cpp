#include "ClientRequest.hpp"
#include "socket.hpp"
#include "cgiHandler.hpp"

ClientRequest::ClientRequest() {}

ClientRequest::~ClientRequest () {}

void ClientRequest::parseBuffer(char *buffer, ssize_t size, int i) {
	ssize_t j = 0;
	std::string line;
	std::istringstream infileBuff(buffer);
	if (!_header.empty()) {
		while (std::getline(infileBuff, line) )
			_body[i].append(line);
		return;
	}
	parseRequestHost(infileBuff, j);
	while (std::getline(infileBuff, line)) {
		j += line.size();
		if (line.size() == 1 && getValueHeader("method") == "POST")
			break;
		parseHeader(line);
	}
	if (getValueHeader("method") == "POST") {
		for (; j < size; j++)
			_body[i] += buffer[j];
	}
	// std::cout << "_body[i] = " << _body[i] << std::endl;
}

void ClientRequest::parseRequestHost(std::istringstream &infileBuff, ssize_t &j) {
	std::string	requestLine, hostLine, method, url, protocol, host, port, tmp;
	if (!infileBuff)
		throw std::runtime_error("opening buffer failed");
	std::getline(infileBuff, requestLine);
	j += requestLine.size();
	std::istringstream iss(requestLine);
	iss >> method, iss >> url, iss >> protocol;
	if (method.empty() || url.empty() || protocol.empty())
		std::runtime_error("invalid HTTP header");
	_header["method"] = method;
	_header["url"] = url;
	_header["protocol"] = protocol;
	std::getline(infileBuff, hostLine);
	j += hostLine.size();
	std::istringstream iss2(hostLine);
	iss2 >> tmp, iss2 >> std::ws;
	std::getline(iss2, host, ':');
	std::getline(iss2, port);
	_header["host"] = host;
	_header["port"] = port;
}

void ClientRequest::parseHeader(std::string line) {
	std::istringstream iss(line);
	std::string key, value;
	std::getline(iss, key, ':');
	std::getline(iss >> std::ws, value);
	_header[key] = value;
}

const std::string ClientRequest::getValueHeader(std::string key) {
	ssMap::iterator it = _header.find(key);
	if (it == _header.end())
		return "";
	return it->second;
}

ssMap ClientRequest::getHeaderMap() const {
	return _header;
}

void ClientRequest::setBody(int i, std::string body) {
	_body[i] = body;
}

const std::string ClientRequest::getBody(int i) {
	isMap::iterator it = _body.find(i);
	return it->second;
}

const std::string ClientRequest::getServerResponse(int i) {
	isMap::iterator it = _serverResponse.find(i);
	return it->second;
}

void ClientRequest::setValueHeader(std::string key, std::string value) {
	_header[key] = value;
}

void ClientRequest::setServerResponse(std::string serverResponse, int i) {
	_serverResponse[i] = serverResponse;
}

void ClientRequest::clearServerResponse(int i) {
	_serverResponse.erase(i);
}

void ClientRequest::clearHeader() {
	_header.clear();
}

void ClientRequest::clearBody(int i) {
	_body.erase(i);
}
