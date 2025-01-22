#include "webserv.hpp"

ParseConfig::ParseConfig(std::string filename, std::vector<t_server> &servers) : _filename(filename), _lineId(0) {
	std::ifstream file(filename.c_str());
	if (!file.is_open())
		throw std::runtime_error("cannot open " + filename);
	std::string line;
	while (std::getline(file, line)) {
		++_lineId;
		line = parseLine(line);
		if (line.empty())
			continue;
		std::string directive, value;
		extractDirectiveValue(line, directive, value);
		std::cout << "directive: " << directive << " | value: " << value << " |" << std::endl;
		if (directive == "server") {
			if (value != "{")
				error("directive \"server\" is not terminated by \"{\"");
			t_server server;
			fillServer(file, server);
			servers.push_back(server);
		} else if (directive == "}")
			continue;
		else
			error("unknown directive \"" + directive + "\"");
	}
	file.close();
}

void ParseConfig::fillServer(std::ifstream &file, t_server &server) {
	std::string line;
	while (std::getline(file, line)) {
		++_lineId;
		line = parseLine(line);
		if (line.empty())
			continue;
		std::string directive, value;
		extractDirectiveValue(line, directive, value);
		std::cout << BLUE << "directive: " << directive << " | value: " << value << " |" << RESET << std::endl;
		if (directive == "listen" && !value.empty())
			extractHostPort(line, server.host, server.port);
		else if (directive == "server_name" && !value.empty())
			server.name = value;
		else if (directive == "error_page" && !value.empty())
			continue; // code for error_page
		else if (directive == "client_max_body_size" && !value.empty())
			continue; // code for client_max_body_size
		else if (directive == "location" && !value.empty()) {
			t_location location;
			location.path = value;
			fillLocation(file, location);
			server.locations.push_back(location);
		} else if (directive == "}")
			break;
		else
			error("unknown directive \"" + directive + "\"");
	}
}

void ParseConfig::fillLocation(std::ifstream &file, t_location &location) {
	std::string line;
	while (std::getline(file, line)) {
		++_lineId;
		line = parseLine(line);
		if (line.empty())
			continue;
		std::string directive, value;
		extractDirectiveValue(line, directive, value);
		std::cout << YELLOW << "directive: " << directive << " | value: " << value << " |" << RESET << std::endl;
		if (directive == "root" && !value.empty())
			location.root = value;
		else if (directive == "index" && !value.empty())
			location.index = value;
		else if (directive == "autoindex" && !value.empty())
			location.autoindex = value;
		else if (directive == "allowed_methods" && !value.empty())
			location.allowedMethods = value;
		else if (directive == "return" && !value.empty())
			location.redirPath = value;
		else if (directive == "}")
			break;
		else
			error("unknown directive \"" + directive + "\"");
	}
}

std::string ParseConfig::parseLine(std::string line) {
	if (line.empty())
		return line;
	trim(line);
	std::string::iterator begin = line.begin(), end = line.end(), it;
	// Remove comment
	for (it = begin; it != end; it++)
		if (*it == ';' || *it == '#')
			break;
	end = it;
	// Check syntax
	if (*begin == '}' && (begin + 1) != end)
		error("syntax error \"" + line + "\"");
	line = std::string(begin, end);
	return line;
}

void ParseConfig::trim(std::string &str) {
	if (str.empty())
		return;
	std::string::iterator begin, end;
	// Remove leading spaces
	for (begin = str.begin(); begin != str.end(); begin++)
		if (!std::isspace(*begin))
			break;
	// Remove trailing spaces
	for (end = str.end(); end != begin; end--)
		if (!std::isspace(*end))
			break;
	str = std::string(begin, end);
}

void ParseConfig::extractDirectiveValue(std::string str, std::string &directive, std::string &value) {
	std::istringstream iss(str);
	// Extract directive
	iss >> directive;
	// Extract value
	std::getline(iss >> std::ws, value);
}

void ParseConfig::extractHostPort(std::string str, std::string &host, int &port) {
	std::istringstream iss(str);
	// Extract host
	std::getline(iss, host, ':');
	// Extract port
	std::string portStr;
	std::getline(iss, portStr);
	port = std::atoi(portStr.c_str());
}

void ParseConfig::error(std::string message) {
	std::stringstream ss;
	ss << _lineId;
	throw std::runtime_error(message + " in " + _filename + ":" + ss.str());
}

ParseConfig::~ParseConfig() {}
