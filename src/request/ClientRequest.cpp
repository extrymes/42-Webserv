#include "ClientRequest.hpp"
#include "socket.hpp"
#include "cgiHandler.hpp"

ClientRequest::ClientRequest() {}

ClientRequest::~ClientRequest () {}

void ClientRequest::parseBuffer(char *buffer) {
	std::string line;
	std::stringstream infileBuff(buffer);
	parseFirstLines(infileBuff);
	while (std::getline(infileBuff, line))
		parseHeader(line);
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
	std::string method, url, host, port, tmp;
	std::getline(first, method, ' ');
	if (method != "GET" && method != "POST" && method != "DELETE") {
		std::cout << "method: " << method << std::endl;
		throw std::runtime_error("wrong method");
	}
	_headers["method"] = method;
	std::getline(first, url, ' ');
	_headers["url"] = url;
	std::stringstream second(secondLine);
	if (!second)
		throw std::runtime_error("opening buffer failed");
	std::getline(second, tmp, ' ');
	std::getline(second, host, ':');
	_headers["host"] = host;
	std::getline(second, port);
	_headers["port"] = port;
}

void ClientRequest::parseHeader(std::string line) {
	std::stringstream whichLine(line);
	std::string key, value;
	std::getline(whichLine, key, ':');
	std::getline(whichLine, value);
	_headers[key] = value;
}

const std::string ClientRequest::getValue(std::string key) {
	std::map<std::string, std::string>::iterator it = _headers.find(key);
	if (it == _headers.end())
		return "";
	return it->second;
}

void ClientRequest::setValue(std::string key, std::string value) {
	_headers[key] = value;
}

std::map<std::string, std::string> ClientRequest::getHeaders() const {
	return _headers;
}
