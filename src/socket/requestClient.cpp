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
	std::string method, url, host, port, tmp;
	std::getline(first, method, ' ');
	if (method != "GET" && method != "POST" && method != "DELETE") {
		std::cout << "method: " << method << std::endl;
		throw std::runtime_error("wrong method");
	}
	_data["method"] = method;
	std::getline(first, url, ' ');
	_data["url"] = url;
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
