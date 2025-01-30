#include "ClientRequest.hpp"
#include "socket.hpp"
#include "cgiHandler.hpp"

ClientRequest::ClientRequest() {}

ClientRequest::~ClientRequest () {}

void ClientRequest::parseBuffer(char *buffer) {
	int	flag = 0;
	std::string line;
	std::stringstream infileBuff(buffer);
	parseFirstLines(infileBuff);
	while (std::getline(infileBuff, line) ) {
		if (line.size() == 1 && getValue("method") == "POST") {
			flag = 1;
			continue;
		}
		flag == 0 ? parseHeader(line) : parseBody(line);
	}
}

void ClientRequest::parseFirstLines(std::stringstream &infileBuff) {
	std::string	firstLine, secondLine;
	if (!infileBuff)
		throw std::runtime_error("opening buffer failed");
	std::getline(infileBuff, firstLine);
	std::getline(infileBuff, secondLine);
	std::stringstream first(firstLine);
	if (!first)
		throw std::runtime_error("opening buffer failed");
	std::string method, url, host, port, tmp, protocol;
	std::getline(first, method, ' ');
	if (method != "GET" && method != "POST" && method != "DELETE") {
		std::cout << "method = " << method << std::endl;
		throw std::runtime_error("wrong method");
	}
	std::getline(first, url, ' ');
	std::getline(first, protocol);
	std::stringstream second(secondLine);
	if (!second)
		throw std::runtime_error("opening buffer failed");
	std::getline(second, tmp, ' ');
	std::getline(second, host, ':');
	std::getline(second, port);
	_headers["method"] = method;
	_headers["url"] = url;
	_headers["protocol"] = protocol;
	_headers["host"] = host;
	_headers["port"] = port;
}

void ClientRequest::parseHeader(std::string line) {
	std::stringstream whichLine(line);
	std::string key, value;
	std::getline(whichLine, key, ':');
	std::getline(whichLine, value);
	_headers[key] = value;
}

void ClientRequest::parseBody(std::string line) {
	std::stringstream body(line);
	std::string key, value;
	while(getline(body, key, '=')) {
		getline(body, value, '&');
		std::cout << "key = " << key << " and value = " << value << std::endl;
		_body[key] = value;
	}
}

const std::string ClientRequest::getValue(std::string key) {
	std::map<std::string, std::string>::iterator it = _headers.find(key);
	if (it == _headers.end())
		return "";
	return it->second;
}

std::map<std::string, std::string> ClientRequest::getHeaders() const {
	return _headers;
}

const std::string ClientRequest::getServerResponse(int i) {
	std::map<int, std::string>::iterator it = _responseServer.find(i);
	return it->second;
}

const std::string ClientRequest::getBody(std::string key) {
	std::map<std::string, std::string>::iterator it = _body.find(key);
	if (it == _body.end())
		return "";
	return it->second;
}

void ClientRequest::setValue(std::string key, std::string value) {
	_headers[key] = value;
}


void ClientRequest::setServerResponse(std::string responseServer, int i) {
	_responseServer[i] = responseServer;
}

void ClientRequest::clearServerResponse(int i) {
    _responseServer.erase(i);
}
