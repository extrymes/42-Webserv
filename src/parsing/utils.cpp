#include "webserv.hpp"

void trim(std::string &str) {
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
		if (*end == '#')
			break;
	str = std::string(start, end);
}

void extractData(std::string str, std::string &directive, std::string &value) {
	// Extract directive
	std::istringstream iss(str);
	iss >> directive;
	// Extract value
	std::getline(iss >> std::ws, value);
}
