#include "webserv.hpp"

ParseConfig::ParseConfig(std::string filename, std::vector<t_server> &servers) : _filename(filename), _lineId(0) {
	_file.open(filename.c_str());
	if (!_file.is_open())
		throw std::runtime_error("cannot open " + filename);
	std::string line;
	while (std::getline(_file, line)) {
		++_lineId;
		std::string directive, args;
		if (!parseLine(line, directive, args))
			continue;
		if (directive == "server") {
			if (!args.empty())
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
		std::string directive, args;
		if (!parseLine(line, directive, args))
			continue;
		if (directive == "listen")
			parseListen(args, server.host, server.port);
		else if (directive == "server_name")
			parseServerName(args, server.name);
		else if (directive == "error_page")
			parseErrorPage(args, server.errorPages);
		else if (directive == "client_max_body_size")
			parseClientMaxBodySize(args, server.clientMaxBodySize);
		else if (directive == "location") {
			t_location location;
			parseLocationPath(args, location.path);
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
		std::string directive, args;
		if (!parseLine(line, directive, args))
			continue;
		if (directive == "root")
			parseLocationRoot(args, location.root);
		else if (directive == "index")
			parseLocationIndex(args, location.index);
		else if (directive == "autoindex")
			parseLocationAutoindex(args, location.autoindex);
		else if (directive == "allowed_methods")
			parseLocationAllowedMethods(args, location.allowedMethods);
		else if (directive == "return")
			parseLocationRedirection(args, location.redirPath, location.redirCode);
		else if (directive == "cgi_extension")
			parseLocationCgiExtension(args, location.cgiExtension);
		else if (directive == "upload_save")
			parseLocationUploadSave(args, location.uploadSave);
		else if (directive == "}")
			break;
		else
			error("unknown directive \"" + directive + "\"");
	}
}

bool ParseConfig::parseLine(std::string line, std::string &directive, std::string &args) {
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
	// Check closed bracket
	if (line == "}")
		return (directive = line, true);
	// Check end character
	checkEndCharacter(line);
	// Remove end character and trim line
	line = line.substr(0, line.size() - 1);
	trim(line);
	// Extract directive and args
	std::istringstream iss(line);
	iss >> directive;
	std::getline(iss >> std::ws, args);
	// Check directive syntax
	for (it = directive.begin(); it != directive.end(); it++)
		if (!std::isalpha(*it) && *it != '_')
			error("syntaxe error \"" + line + "\"");
	return true;
}

void ParseConfig::parseListen(std::string args, std::string &host, int &port) {
	if (countArgs(args) != 1)
		error("invalid number of arguments in \"listen\" directive");
	std::istringstream iss(args);
	std::string portStr;
	// Extract host
	std::getline(iss, host, ':');
	std::getline(iss, portStr);
	// Check if host is valid
	if (host.empty())
		error("invalid host \"" + args + "\"");
	char *end;
	if (host != "localhost") {
		std::vector<std::string> splitedHost;
		std::istringstream iss2(host);
		std::string segment;
		while (std::getline(iss2, segment, '.'))
			splitedHost.push_back(segment);
		if (splitedHost.size() != 4)
			error("invalid host \"" + args + "\"");
		std::vector<std::string>::iterator it;
		for (it = splitedHost.begin(); it != splitedHost.end(); it++) {
			if (it->empty())
				error("invalid host \"" + args + "\"");
			int byte = std::strtol(it->c_str(), &end, 10);
			if (end != it->c_str() + it->length() || byte < 0 || byte > 255)
				error("invalid host \"" + args + "\"");
		}
	}
	// Extract port
	std::getline(iss, portStr);
	if (portStr.empty())
		error("missing port in \"" + args + "\"");
	// Convert port to number
	port = std::strtol(portStr.c_str(), &end, 10);
	// Check if port is valid
	if (end != portStr.c_str() + portStr.length() || port < 1 || port > 65535)
		error("invalid port in \"" + args + "\"");
}

void ParseConfig::parseServerName(std::string args, std::string &serverName) {
	if (countArgs(args) != 1)
		error("invalid number of arguments in \"server_name\" directive");
	// Check if server name is valid
	std::string::iterator it;
	for (it = args.begin(); it != args.end(); it++)
		if (!std::isalnum(*it) && *it != '.')
			error("invalid server name \"" + args + "\"");
	serverName = args;
}

void ParseConfig::parseErrorPage(std::string args, std::map<int, std::string> &errorPages) {
	if (countArgs(args) != 2)
		error("invalid number of arguments in \"error_page\" directive");
	std::istringstream iss(args);
	std::string codeStr, path;
	// Extract error code and path
	iss >> codeStr, iss >> path;
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

void ParseConfig::parseClientMaxBodySize(std::string args, long &clientMaxBodySize) {
	if (countArgs(args) != 1)
		error("invalid number of arguments in \"client_max_body_size\" directive");
	// Convert value to number
	char *end;
	clientMaxBodySize = std::strtol(args.c_str(), &end, 10);
	// Check if value is valid
	if (end != args.c_str() + args.size())
		error("invalid value \"" + args + "\"");
}

void ParseConfig::parseLocationPath(std::string args, std::string &path) {
	if (countArgs(args) != 1)
		error("invalid number of arguments in \"location\" directive");
	path = args;
}

void ParseConfig::parseLocationRoot(std::string args, std::string &root) {
	if (countArgs(args) != 1)
		error("invalid number of arguments in \"root\" directive");
	// Remove slash at begin
	if (args[0] == '/')
		args = args.substr(1);
	root = args;
}

void ParseConfig::parseLocationIndex(std::string args, std::string &index) {
	if (countArgs(args) < 1)
		error("invalid number of arguments in \"index\" directive");
	index = args;
}

void ParseConfig::parseLocationAutoindex(std::string args, std::string &autoindex) {
	if (countArgs(args) != 1)
		error("invalid number of arguments in \"autoindex\" directive");
	autoindex = args;
}

void ParseConfig::parseLocationAllowedMethods(std::string args, std::string &allowedMethods) {
	if (countArgs(args) < 1)
		error("invalid number of arguments in \"allowed_methods\" directive");
	std::istringstream iss(args);
	std::string method;
	while (iss >> method)
		if (method != "GET" && method != "POST" && method != "DELETE")
			error("invalid method \"" + method + "\"");
	allowedMethods = args;
}

void ParseConfig::parseLocationCgiExtension(std::string args, std::string &cgiExtension) {
	if (countArgs(args) != 1)
		error("invalid number of arguments in \"cgi_extension\" directive");
	if (args != ".php" && args != ".py")
		error("invalid extension \"" + args + "\"");
	cgiExtension = args;
}

void ParseConfig::parseLocationUploadSave(std::string args, std::string &uploadSave) {
	if (countArgs(args) != 1)
		error("invalid number of arguments in \"upload_save\" directive");
	uploadSave = args;
}

void ParseConfig::parseLocationRedirection(std::string args, std::string &redirPath, int &redirCode) {
	if (countArgs(args) != 2)
		error("invalid number of arguments in \"return\" directive");
	std::istringstream iss(args);
	std::string codeStr, rest;
	// Extract redirection code and path
	iss >> codeStr, iss >> redirPath;
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

void ParseConfig::checkEndCharacter(std::string line) {
	// Extract directive
	std::istringstream iss(line);
	std::string directive;
	iss >> directive;
	// Check end character
	if (directive == "server" || directive == "location") {
		if (line[line.size() - 1] != '{')
			error("directive \"" + directive + "\" is not terminated by opening \"{\"");
	} else if (directive != "}") {
		if (line[line.size() - 1] != ';')
			error("line is not terminated by \";\"");
	}
}

int ParseConfig::countArgs(std::string args) {
	std::istringstream iss(args);
	std::string arg;
	int count = 0;
	while (iss >> arg)
		++count;
	return count;
}

void ParseConfig::error(std::string message) {
	std::stringstream ss;
	ss << _lineId;
	throw std::runtime_error(message + " in " + _filename + ":" + ss.str());
}


ParseConfig::~ParseConfig() {}
