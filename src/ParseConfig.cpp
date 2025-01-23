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
			if (!value.empty())
				error("syntax error \"" + line + "\"");
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
		else if (directive == "server_name")
			parseServerName(value, server.name);
		else if (directive == "error_page")
			parseErrorPage(value, server.errorPages);
		else if (directive == "client_max_body_size")
			parseClientMaxBodySize(value, server.clientMaxBodySize);
		else if (directive == "location") {
			t_location location;
			parseLocationPath(value, location.path);
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
		if (directive == "root")
			parseLocationRoot(value, location.root);
		else if (directive == "index")
			parseLocationIndex(value, location.index);
		else if (directive == "autoindex")
			parseLocationAutoindex(value, location.autoindex);
		else if (directive == "allowed_methods")
			parseLocationAllowedMethods(value, location.allowedMethods);
		else if (directive == "return")
			parseLocationRedirection(value, location.redirPath, location.redirCode);
		else if (directive == "}")
			break;
		else
			error("unknown directive \"" + directive + "\"");
	}
}

bool ParseConfig::parseLine(std::string line, std::string &directive, std::string &value) {
	if (line.empty())
		return false;
	std::string::iterator begin = line.begin(), end = line.end(), it;
	// Remove comment
	it = begin;
	while (it != end && *it != '#')
		++it;
	end = it;
	if (begin == end)
		return false;
	line.assign(begin, end);
	// Trim line
	trim(line);
	if (line.empty())
		return false;
	// Check line syntax
	if (line.find('}') != std::string::npos && line.size() != 1)
		error("syntax error \"" + line + "\"");
	// Extract directive
	std::istringstream iss(line);
	iss >> directive;
	// Check end character
	if (directive == "server" || directive == "location") {
		if (line[line.size() - 1] != '{')
			error("directive \"" + directive + "\" is not terminated by opening \"{\"");
	} else if (directive != "}") {
		if (line[line.size() - 1] != ';')
			error("line is not terminated by \";\"");
	}
	// Remove end character and trim line
	line = line.substr(0, line.size() - 1);
	trim(line);
	// Extract directive and value
	std::istringstream iss2(line);
	iss2 >> directive;
	std::getline(iss2 >> std::ws, value);
	// Check directive syntax
	for (it = directive.begin(); it != directive.end(); it++)
		if (!std::isalpha(*it) && *it != '_')
			error("syntax error \"" + line + "\"");
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
	// Convert port to number
	port = std::strtol(portStr.c_str(), &end, 10);
	// Check if port is valid
	if (end != portStr.c_str() + portStr.length() || port < 1 || port > 65535)
		error("invalid port in \"" + value + "\"");
}

void ParseConfig::parseServerName(std::string value, std::string &serverName) {
	if (value.empty())
		error("invalid number of arguments in \"listen\" directive");
	// Check if server name is valid
	std::string::iterator it;
	for (it = value.begin(); it != value.end(); it++)
		if (!std::isalnum(*it) && *it != '.')
			error("invalid server name \"" + value + "\"");
	serverName = value;
}

void ParseConfig::parseErrorPage(std::string value, std::map<int, std::string> &errorPages) {
	std::istringstream iss(value);
	std::string codeStr, path, rest;
	// Extract error code and path
	iss >> codeStr, iss >> path, iss >> rest;
	if (codeStr.empty() || path.empty() || !rest.empty())
		error("invalid number of arguments in \"error_page\" directive");
	// Convert error code to number
	char *end;
	int code = std::strtol(codeStr.c_str(), &end, 10);
	// Check if error code is valid
	if (end != codeStr.c_str() + codeStr.length())
		error("invalid error code \"" + codeStr + "\"");
	if (code < 300 || code > 599)
		error("error code must be between 300 and 599");
	// Check if path is valid
	std::ifstream file(path.c_str());
	if (!file.good())
		error("invalid path \"" + path + "\"");
	errorPages.insert(std::pair<int, std::string>(code, path));
}

void ParseConfig::parseClientMaxBodySize(std::string value, long &clientMaxBodySize) {
	if (value.empty())
		error("invalid number of arguments in \"client_max_body_size\" directive");
	// Convert value to number
	char *end;
	clientMaxBodySize = std::strtol(value.c_str(), &end, 10);
	// Check if value is valid
	if (end != value.c_str() + value.size())
		error("invalid value \"" + value + "\"");
}

void ParseConfig::parseLocationPath(std::string value, std::string &path) {
	std::istringstream iss(value);
	std::string rest;
	// Extract path
	iss >> path, iss >> rest;
	if (path.empty() || !rest.empty())
		error("invalid number of arguments in \"location\" directive");
}

void ParseConfig::parseLocationRoot(std::string value, std::string &root) {
	if (value.empty())
		error("invalid number of arguments in \"root\" directive");
	root = value;
}

void ParseConfig::parseLocationIndex(std::string value, std::string &index) {
	if (value.empty())
		error("invalid number of arguments in \"index\" directive");
	index = value;
}

void ParseConfig::parseLocationAutoindex(std::string value, std::string &autoindex) {
	if (value.empty())
		error("invalid number of arguments in \"autoindex\" directive");
	autoindex = value;
}

void ParseConfig::parseLocationAllowedMethods(std::string value, std::string &allowedMethods) {
	if (value.empty())
		error("invalid number of arguments in \"allowed_methods\" directive");
	std::istringstream iss(value);
	std::string method;
	while (iss >> method)
		if (method != "GET" && method != "POST" && method != "DELETE")
			error("invalid method \"" + method + "\"");
	allowedMethods = value;
}

void ParseConfig::parseLocationRedirection(std::string value, std::string &redirPath, int &redirCode) {
	std::istringstream iss(value);
	std::string codeStr, rest;
	iss >> codeStr, iss >> redirPath, iss >> rest;
	if (codeStr.empty() || redirPath.empty() || !rest.empty())
		error("invalid number of arguments in \"return\" directive");
	// Convert redirection code to number
	char *end;
	redirCode = std::strtol(codeStr.c_str(), &end, 10);
	// Check if redirection code is valid
	if (end != codeStr.c_str() + codeStr.size() || codeStr.size() != 3)
		error("invalid redirection code \"" + codeStr + "\"");
}

void ParseConfig::trim(std::string &str) {
	if (str.empty())
		return;
	std::string::iterator begin = str.begin(), end = str.end();
	// Remove leading spaces
	while (begin != end && std::isspace(*begin))
		++begin;
	// Remove trailing spaces
	if (begin != end) {
		do {
			--end;
		} while (begin != end && std::isspace(*end));
		++end;
	}
	str.assign(begin, end);
}

void ParseConfig::error(std::string message) {
	std::stringstream ss;
	ss << _lineId;
	throw std::runtime_error(message + " in " + _filename + ":" + ss.str());
}

ParseConfig::~ParseConfig() {}
