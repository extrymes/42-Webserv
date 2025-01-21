#include "webserv.hpp"

Config::Config() {

}

void Config::parseConfigFile(std::string filename, t_config &config) {
	std::ifstream file(filename.c_str());
	if (!file.is_open())
		throw std::runtime_error("cannot open config file");
	std::string line;
	while (std::getline(file, line)) {
		Config::trim(line);
		if (line.empty())
			continue;
		std::string directive, value;
		Config::extractDirectiveValue(line, directive, value);
		std::cout << "directive: " << directive << " | value: " << value << " |" << std::endl;
		if (directive == "server" && value == "{") {
			t_server server;
			Config::fillServer(file, server);
			config.servers.push_back(server);
		} else if (directive == "}") {
			if (!value.empty())
				throw std::runtime_error("unknown token '" + line + "' in config file");
			continue;
		} else
			throw std::runtime_error("unknown directive '" + directive + "' in config file");
	}
	file.close();
}

void Config::fillServer(std::ifstream &file, t_server &server) {
	std::string line;
	while (std::getline(file, line)) {
		Config::trim(line);
		if (line.empty())
			continue;
		std::string directive, value;
		Config::extractDirectiveValue(line, directive, value);
		std::cout << BLUE << "directive: " << directive << " | value: " << value << " |" << RESET << std::endl;
		if (directive == "listen" && !value.empty())
			Config::extractHostPort(line, server.host, server.port);
		else if (directive == "server_name" && !value.empty())
			server.name = value;
		else if (directive == "error_page" && !value.empty())
			continue; // code for error_page
		else if (directive == "client_max_body_size" && !value.empty())
			continue; // code for client_max_body_size
		else if (directive == "location" && !value.empty()) {
			t_location location;
			location.path = value;
			Config::fillLocation(file, location);
			server.locations.push_back(location);
		} else if (directive == "}") {
			if (!value.empty())
				throw std::runtime_error("unknown token '" + line + "' in config file");
			break;
		} else
			throw std::runtime_error("unknown directive '" + directive + "' in config file");
	}
}

void Config::fillLocation(std::ifstream &file, t_location &location) {
	std::string line;
	while (std::getline(file, line)) {
		Config::trim(line);
		if (line.empty())
			continue;
		std::string directive, value;
		Config::extractDirectiveValue(line, directive, value);
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
		else if (directive == "}") {
			if (!value.empty())
				throw std::runtime_error("unknown token '" + line + "' in config file");
			break;
		} else
			throw std::runtime_error("unknown directive '" + directive + "' in config file");
	}
}

void Config::trim(std::string &str) {
	if (str.empty())
		return;
	std::string::iterator start;
	std::string::iterator end;
	// Remove leading and trailing spaces
	for (start = str.begin(); start != str.end(); start++)
		if (!std::isspace(*start))
			break;
	for (end = str.end(); end != start; end--)
		if (!std::isspace(*end))
			break;
	// Remove comment
	std::string::iterator tmp = end;
	for (end = start; end != tmp; end++)
		if (*end == ';' || *end == '#')
			break;
	str = std::string(start, end);
}

void Config::extractDirectiveValue(std::string str, std::string &directive, std::string &value) {
	std::istringstream iss(str);
	// Extract directive
	iss >> directive;
	// Extract value
	std::getline(iss >> std::ws, value);
}

void Config::extractHostPort(std::string str, std::string &host, int &port) {
	std::istringstream iss(str);
	// Extract host
	std::getline(iss, host, ':');
	// Extract port
	std::string portStr;
	std::getline(iss, portStr);
	port = std::atoi(portStr.c_str());
}
