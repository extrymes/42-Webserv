#include "webserv.hpp"

void trim(std::string &str) {
	std::string::iterator start;
	std::string::iterator end;
	for (start = str.begin(); start != str.end(); start++)
		if (!std::isspace(*start))
			break;
	for (end = str.end(); end != str.begin(); end--)
		if (!std::isspace(*end))
			break;
	str = std::string(start, end);
}
