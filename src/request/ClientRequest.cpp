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
	if (method != "GET" && method != "POST" && method != "DELETE")
		throw std::runtime_error("invalid HTTP method");
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
		// std::cout << "key = " << key << " and value = " << value << std::endl;
		_body[key] = value;
	}
}

const std::string ClientRequest::getValueHeader(std::string key) {
	std::map<std::string, std::string>::iterator it = _headers.find(key);
	if (it == _headers.end())
		return "";
	return it->second;
}

std::map<std::string, std::string> ClientRequest::getHeaders() const {
	return _headers;
}

std::map<std::string, std::string> ClientRequest::getBody() const {
	return _body;
}

const std::string ClientRequest::getServerResponse(int i) {
	std::map<int, std::string>::iterator it = _responseServer.find(i);
	return it->second;
}

const std::string ClientRequest::getValueBody(std::string key) {
	std::map<std::string, std::string>::iterator it = _body.find(key);
	if (it == _body.end())
		return "";
	return it->second;
}

void ClientRequest::setValueHeader(std::string key, std::string value) {
	_headers[key] = value;
}


void ClientRequest::setServerResponse(std::string responseServer, int i) {
	_responseServer[i] = responseServer;
}

void ClientRequest::clearServerResponse(int i) {
	_responseServer.erase(i);
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
