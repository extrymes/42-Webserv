#include "ClientRequest.hpp"
#include "socket.hpp"
#include "cgiHandler.hpp"

ClientRequest::ClientRequest() {}

ClientRequest::~ClientRequest () {}

void ClientRequest::parseBuffer(char *buffer, ssize_t size) {
	ssize_t j = 0;
	std::string line;
	std::istringstream infileBuff(buffer);
	if (!_header.empty()) {
		std::string strBuff = buffer;
		for(ssize_t i = _body.size(); i < std::atoi(getValueHeader("Content-Length").c_str()); ++i) {
			_body += strBuff[j];
			++j;
		}
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
		std::string strBuff = buffer;
		while (j < size && strBuff.compare(j, 4, "\r\n\r\n") != 0)
			j++;
		j += 4;
		for (; j < size; j++)
			_body += buffer[j];
	}
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

void ClientRequest::setBody(std::string body) {
	_body = body;
}

const std::string ClientRequest::getBody() {
	return _body;
}

const std::string ClientRequest::getServerResponse() {
	return _serverResponse;
}

void ClientRequest::setValueHeader(std::string key, std::string value) {
	_header[key] = value;
}

void ClientRequest::setServerResponse(std::string serverResponse) {
	_serverResponse = serverResponse;
}

void ClientRequest::clearServerResponse() {
	_serverResponse.clear();
}

void ClientRequest::clearHeader() {
	_header.clear();
}

void ClientRequest::clearBody() {
	_body = "";
}
