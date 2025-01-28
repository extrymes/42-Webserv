#include "socket.hpp"

RequestClient::RequestClient() {}

RequestClient::~RequestClient () {}

void RequestClient::parseBuffer(char *buffer) {
	// std::cout << "buffer = " << buffer << std::endl;
	std::string line;
	std::stringstream infileBuff(buffer);
	parseFirstLines(infileBuff);
	while(std::getline(infileBuff, line))
		parseHeader(line);
}

void RequestClient::parseFirstLines(std::stringstream &infileBuff) {
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
		std::cout << "method: " << method << std::endl;
		throw std::runtime_error("wrong method");
	}
	_data["method"] = method;
	std::getline(first, url, ' ');
	_data["url"] = url;
	std::getline(first, protocol);
	_data["protocol"] = protocol;
	std::stringstream second(secondLine);
	if (!second)
		throw std::runtime_error("opening buffer failed");
	std::getline(second, tmp, ' ');
	std::getline(second, host, ':');
	_data["host"] = host;
	std::getline(second, port);
	_data["port"] = port;
}

void RequestClient::parseHeader(std::string line) {
	std::stringstream whichLine(line);
	std::string key;
	std::string value;
	std::getline(whichLine, key, ':');
	std::getline(whichLine, value);
	_data[key] = value;
}

std::string RequestClient::getValue(std::string key) {
	std::map<std::string, std::string>::iterator it = _data.find(key);
	if (it == _data.end())
		return "";
	return it->second;
}

void RequestClient::setValue(std::string key, std::string value) {
	_data[key] = value;
}

void RequestClient::setResponseServer(std::string responseServer, int i) {
	_responseServer[i] = responseServer;
}

std::string RequestClient::getResponseServer(int i) {
	std::map<int, std::string>::iterator it = _responseServer.find(i);
	return it->second;
}
