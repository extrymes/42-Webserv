#include "socket.hpp"

RequestClient::RequestClient() {}

RequestClient::~RequestClient () {}

void RequestClient::parseBuffer(char *buffer) {
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
	std::getline(first, _method, ' ');
	std::getline(first, _url, ' ');
	std::stringstream second(secondLine);
	if (!second)
		throw std::runtime_error("opening buffer failed");
	std::string tmp;
	std::getline(second, tmp, ' ');
	std::getline(second, _host, ':');
	std::getline(second, _port);
	if (_method != "GET" && _method != "POST" && _method != "DELETE") {
		std::cout << "method: " << _method << std::endl;
		throw std::runtime_error("wrong method");
	}
}

void RequestClient::parseHeader(std::string line) {
	std::stringstream whichLine(line);
	std::string firstPartLine;
	std::string lastPartLine;
	std::getline(whichLine, firstPartLine, ':');
	std::getline(whichLine, lastPartLine);
	if (firstPartLine == "User-Agent")
		_userAgent = lastPartLine;
	else if (firstPartLine == "Accept")
		_accept = lastPartLine;
	else if (firstPartLine == "Accept-Language")
		_acceptLanguage = lastPartLine;
	else if (firstPartLine == "Accept-Encoding")
		_acceptEncoding = lastPartLine;
	else if (firstPartLine == "Connection")
		_connection = lastPartLine;
	else if (firstPartLine == "Upgrade-Insecure-Requests")
		_upgradeInsecureRequests = lastPartLine;
	else if (firstPartLine == "Sec-Fetch-Dest")
		_secFetchDest = lastPartLine;
	else if (firstPartLine == "Sec-Fetch-Mode")
		_secFetchMode = lastPartLine;
	else if (firstPartLine == "Sec-Fetch-Site")
		_secFetchSite = lastPartLine;
	else if (firstPartLine == "Sec-Fetch-User")
		_secFetchUser = lastPartLine;
	else if (firstPartLine == "Priority")
		_priority = lastPartLine;
	else if (firstPartLine == "ty")
		_ty = lastPartLine;
}

std::string RequestClient::getMethod() const {
	return _method;
}

std::string RequestClient::getUrl() const {
	return _url;
}

std::string RequestClient::getPort() const {
	return _port;
}

std::string RequestClient::getHost() const {
	return _host;
}

std::string RequestClient::getResponseServer() const {
	return _responseServer;
}

void RequestClient::setResponseServer(std::string const &response) {
	_responseServer = response;
}

void RequestClient::setUrl(std::string const &url) {
	_url = url;
}


// buff = GET / HTTP/1.1
// Host: localhost:8082
// User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:128.0) Gecko/20100101 Firefox/128.0
// Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/png,image/svg+xml,*/*;q=0.8
// Accept-Language: en-US,en;q=0.5
// Accept-Encoding: gzip, deflate, br, zstd
// Connection: keep-alive
// Upgrade-Insecure-Requests: 1
// Sec-Fetch-Dest: document
// Sec-Fetch-Mode: navigate
// Sec-Fetch-Site: none
// Sec-Fetch-User: ?1
// Priority: u=0, i


// buff = GET /style.css HTTP/1.1
// Host: localhost:8082
// User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:128.0) Gecko/20100101 Firefox/128.0
// Accept: text/css,*/*;q=0.1
// Accept-Language: en-US,en;q=0.5
// Accept-Encoding: gzip, deflate, br, zstd
// Connection: keep-alive
// Referer: http://localhost:8082/
// Sec-Fetch-Dest: style
// Sec-Fetch-Mode: no-cors
// Sec-Fetch-Site: same-origin
// Priority: u=2

//  document
// Sec-Fetch-Mode: navigate
// Sec-Fetch-Site: none
// Sec-Fetch-User: ?1
// Priority: u=0, i


// buff = GET /main.js HTTP/1.1
// Host: localhost:8082
// User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:128.0) Gecko/20100101 Firefox/128.0
// Accept: */*
// Accept-Language: en-US,en;q=0.5
// Accept-Encoding: gzip, deflate, br, zstd
// Connection: keep-alive
// Referer: http://localhost:8082/
// Sec-Fetch-Dest: script
// Sec-Fetch-Mode: no-cors
// Sec-Fetch-Site: same-origin

//  same-origin
// Priority: u=2

//  document
// Sec-Fetch-Mode: navigate
// Sec-Fetch-Site: none
// Sec-Fetch-User: ?1
// Priority: u=0, i
// ty: u=0, i
