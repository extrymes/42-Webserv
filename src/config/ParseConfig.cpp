#include "ParseConfig.hpp"

ParseConfig::ParseConfig(std::string filename, std::vector<t_server> &servers) : _filename(filename), _lineId(0) {
	if (filename.size() < 5 || filename.substr(filename.size() - 5) != ".conf")
		throw std::runtime_error("invalid file extension");
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
			if (server.host.empty())
				error("missing directive \"listen\"");
			servers.push_back(server);
		} else if (directive == "}")
			error("syntax error \"" + line + "\"");
		else
			error("unknown directive \"" + directive + "\"");
	}
	_file.close();
	if (servers.size() == 0)
		error("missing block \"server\"");
	size_t i, j;
	for (i = 0; i < servers.size(); ++i) {
		for (j = i + 1; j < servers.size(); ++j) {
			if (servers[i].host == servers[j].host && servers[i].port == servers[j].port)
				throw std::runtime_error("at least two servers have the same host");
		}
	}
}

void ParseConfig::fillServer(t_server &server) {
	std::string line;
	bool closed = false;
	server.clientMaxBodySize = 1024;
	while (std::getline(_file, line)) {
		++_lineId;
		std::string directive, args;
		if (!parseLine(line, directive, args))
			continue;
		if (directive == "listen")
			parseListen(args, server.host, server.port);
		else if (directive == "server_name")
			parseServerName(args, server.name);
		else if (directive == "root")
			parseRoot(args, server.root);
		else if (directive == "index")
			parseIndex(args, server.indexes);
		else if (directive == "error_page")
			parseErrorPage(args, server.errorPages);
		else if (directive == "client_max_body_size")
			parseClientMaxBodySize(args, server.clientMaxBodySize);
		else if (directive == "location") {
			t_location location;
			parseLocationPath(args, location.path);
			fillLocation(location);
			server.locations.push_back(location);
		} else if (directive == "}") {
			closed = true;
			break;
		} else
			error("unknown directive \"" + directive + "\"");
	}
	if (!closed)
		error("block \"server\" is not closed by \"}\"");
}

void ParseConfig::fillLocation(t_location &location) {
	std::string line;
	bool closed = false;
	while (std::getline(_file, line)) {
		++_lineId;
		std::string directive, args;
		if (!parseLine(line, directive, args))
			continue;
		if (directive == "root")
			parseRoot(args, location.root);
		else if (directive == "index")
			parseIndex(args, location.indexes);
		else if (directive == "autoindex")
			parseLocationAutoindex(args, location.autoindex);
		else if (directive == "allowed_methods")
			parseLocationAllowedMethods(args, location.allowedMethods);
		else if (directive == "return")
			parseLocationRedirection(args, location.redirCode, location.redirPath);
		else if (directive == "cgi_extension")
			parseLocationCgiExtension(args, location.cgiExtension);
		else if (directive == "upload_save")
			parseLocationUploadSave(args, location.uploadSave);
		else if (directive == "}") {
			closed = true;
			break;
		} else
			error("unknown directive \"" + directive + "\"");
	}
	if (!closed)
		error("block \"location\" is not closed by \"}\"");
}

bool ParseConfig::parseLine(std::string line, std::string &directive, std::string &args) {
	if (line.empty())
		return false;
	// Remove comment
	size_t end = line.find_first_of('#');
	if (end != std::string::npos)
		line = line.substr(0, end);
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
	for (std::string::iterator it = directive.begin(); it != directive.end(); it++)
		if (!std::isalpha(*it) && *it != '_')
			error("syntax error \"" + line + "\"");
	return true;
}

void ParseConfig::parseListen(std::string args, std::string &host, int &port) {
	if (countArgs(args) != 1)
		error("invalid number of arguments in \"listen\" directive");
	std::istringstream iss(args);
	std::string portStr;
	// Extract host
	std::getline(iss, host, ':');
	// Check if host is valid
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
			if (end != it->c_str() + it->size() || byte < 0 || byte > 255)
				error("invalid host \"" + args + "\"");
		}
	} else
		host = "127.0.0.1";
	// Extract port
	std::getline(iss, portStr);
	if (portStr.empty())
		error("missing port in \"" + args + "\"");
	// Convert port to number
	port = std::strtol(portStr.c_str(), &end, 10);
	// Check if port is valid
	if (end != portStr.c_str() + portStr.size() || port < 1 || port > 65535)
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

void ParseConfig::parseRoot(std::string args, std::string &root) {
	if (countArgs(args) != 1)
		error("invalid number of arguments in \"root\" directive");
	// Remove slash at begin
	if (args[0] == '/')
		args = args.substr(1);
	root = args;
}

void ParseConfig::parseIndex(std::string args, std::vector<std::string> &indexes) {
	if (countArgs(args) < 1)
		error("invalid number of arguments in \"index\" directive");
	std::istringstream iss(args);
	std::string index;
	while (iss >> index)
		indexes.push_back(index);
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
	if (end != codeStr.c_str() + codeStr.size())
		error("invalid error code \"" + codeStr + "\"");
	if (code < 300 || code > 599)
		error("error code must be between 300 and 599");
	// Check if path is valid
	std::ifstream file(path.c_str());
	if (!file.good())
		error("invalid path \"" + path + "\"");
	errorPages[code] = path;
}

void ParseConfig::parseClientMaxBodySize(std::string args, long &clientMaxBodySize) {
	if (countArgs(args) != 1)
		error("invalid number of arguments in \"client_max_body_size\" directive");
	// Check unit
	char unit = args[args.size() - 1];
	if (unit == 'M' || unit == 'K')
		args = args.substr(0, args.size() - 1);
	// Convert value to number
	char *end;
	clientMaxBodySize = std::strtol(args.c_str(), &end, 10);
	// Check if value is valid
	if (end != args.c_str() + args.size() || clientMaxBodySize < 0)
		error("invalid value \"" + args + "\"");
	if (unit == 'K')
		clientMaxBodySize *= 1024;
	else if (unit == 'M')
		clientMaxBodySize *= std::pow(1024, 2);
	if (clientMaxBodySize > std::pow(1024, 3) * 10)
		error("invalid value \"" + args + "\"");
}

void ParseConfig::parseLocationPath(std::string args, std::string &path) {
	if (countArgs(args) != 1)
		error("invalid number of arguments in \"location\" directive");
	// Add slash at end
	if (args[args.size() - 1] != '/')
		args.append("/");
	path = args;
}

void ParseConfig::parseLocationAutoindex(std::string args, std::string &autoindex) {
	if (countArgs(args) != 1)
		error("invalid number of arguments in \"autoindex\" directive");
	if (args != "on" && args != "off")
		error("invalid value \"" + args + "\"");
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

void ParseConfig::parseLocationRedirection(std::string args, std::string &redirCode, std::string &redirPath) {
	if (countArgs(args) != 2)
		error("invalid number of arguments in \"return\" directive");
	std::istringstream iss(args);
	// Extract redirection code and path
	iss >> redirCode, iss >> redirPath;
	// Convert redirection code to number
	char *end;
	std::strtol(redirCode.c_str(), &end, 10);
	// Check if redirection code is valid
	if (end != redirCode.c_str() + redirCode.size() || redirCode.size() != 3)
		error("invalid redirection code \"" + redirCode + "\"");
}

void ParseConfig::trim(std::string &str) {
	if (str.empty())
		return;
	// Remove leading and trailing spaces
	size_t begin = str.find_first_not_of(" \t"), end = str.find_last_not_of(" \t");
	if (begin == std::string::npos) {
		str.clear();
		return;
	}
	str = str.substr(begin, end - begin + 1);
}

void ParseConfig::checkEndCharacter(std::string line) {
	// Extract directive
	std::istringstream iss(line);
	std::string directive;
	iss >> directive;
	// Remove semicolon at end
	if (directive[directive.size() - 1] == ';')
		directive = directive.substr(0, directive.size() - 1);
	// Check end character
	if (directive == "server" || directive == "location") {
		if (line[line.size() - 1] != '{')
			error("block \"" + directive + "\" is not opened by \"{\"");
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
