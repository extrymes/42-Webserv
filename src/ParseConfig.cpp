#include "webserv.hpp"

ParseConfig::ParseConfig(std::string filename, std::vector<t_server> &servers) : _filename(filename), _lineId(0) {
	_file.open(filename.c_str());
	if (!_file.is_open())
		throw std::runtime_error("cannot open " + filename);
	std::string line;
	while (std::getline(_file, line)) {
		++_lineId;
		std::string directive, value;
		if (!parseLine(line, directive, value))
			continue;
		if (directive == "server") {
			if (value != "{")
				error("directive \"" + directive + "\" is not terminated by \"{\"");
			t_server server;
			fillServer(server);
			servers.push_back(server);
		} else if (directive == "}")
			continue;
		else
			error("unknown directive \"" + directive + "\"");
	}
	_file.close();
}

void ParseConfig::fillServer(t_server &server) {
	std::string line;
	while (std::getline(_file, line)) {
		++_lineId;
		std::string directive, value;
		if (!parseLine(line, directive, value))
			continue;
		if (directive == "listen")
			parseListen(value, server.host, server.port);
		else if (directive == "server_name") {
			if (value.empty())
				error("invalid number of arguments in \"" + directive + "\" directive");
			server.name = value;
		}
		else if (directive == "error_page" && !value.empty())
			continue; // code for error_page
		else if (directive == "client_max_body_size" && !value.empty())
			continue; // code for client_max_body_size
		else if (directive == "location" && !value.empty()) {
			t_location location;
			location.path = value;
			fillLocation(location);
			server.locations.push_back(location);
		} else if (directive == "}")
			break;
		else
			error("unknown directive \"" + directive + "\"");
	}
}

void ParseConfig::fillLocation(t_location &location) {
	std::string line;
	while (std::getline(_file, line)) {
		++_lineId;
		std::string directive, value;
		if (!parseLine(line, directive, value))
			continue;
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

bool ParseConfig::parseLine(std::string line, std::string &directive, std::string &value) {
	if (line.empty())
		return false;
	trim(line);
	std::string::iterator begin = line.begin(), end = line.end(), it;
	// Remove comment
	for (it = begin; it != end; it++)
		if (*it == ';' || *it == '#')
			break;
	end = it;
	if (begin == end)
		return false;
	// Check syntax
	if (*begin == '}' && (begin + 1) != end)
		error("syntax error \"" + line + "\"");
	// Create new line
	line = std::string(begin, end);
	std::istringstream iss(line);
	// Extract directive
	iss >> directive;
	// Extract value
	std::getline(iss, value);
	trim(value);
	return true;
}

void ParseConfig::parseListen(std::string value, std::string &host, int &port) {
	if (value.empty())
		error("invalid number of arguments in \"listen\" directive");
	std::istringstream issValue(value);
	std::string portStr;
	// Extract host and port
	std::getline(issValue, host, ':');
	std::getline(issValue, portStr);
	// Check if host is valid
	if (host.empty() || portStr.empty())
		error("invalid host \"" + value + "\"");
	std::vector<std::string> splitedHost;
	std::istringstream issHost(host);
	std::string segment;
	while (std::getline(issHost, segment, '.'))
		splitedHost.push_back(segment);
	if (splitedHost.size() != 4)
		error("invalid host \"" + value + "\"");
	std::vector<std::string>::iterator it;
	char *end;
	for (it = splitedHost.begin(); it != splitedHost.end(); it++) {
		if (it->empty())
			error("invalid host \"" + value + "\"");
		int byte = std::strtol(it->c_str(), &end, 10);
		if (end != it->c_str() + it->length() || byte < 0 || byte > 255)
			error("invalid host \"" + value + "\"");
	}
	// Check if port is valid
	port = std::strtol(portStr.c_str(), &end, 10);
	if (end != portStr.c_str() + portStr.length() || port < 1 || port > 65535)
		error("invalid port in \"" + value + "\"");
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

void ParseConfig::error(std::string message) {
	std::stringstream ss;
	ss << _lineId;
	throw std::runtime_error(message + " in " + _filename + ":" + ss.str());
}

ParseConfig::~ParseConfig() {}
