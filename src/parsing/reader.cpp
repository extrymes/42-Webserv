#include "webserv.hpp"

void readFile(std::string filename) {
	std::ifstream file(filename.c_str());
	if (!file.is_open())
		throw std::runtime_error("cannot open config file");
	std::string line;
	while (std::getline(file, line)) {
		if (line.empty())
			continue;
		trim(line);
		if (line[0] == '#')
			continue;
		std::istringstream iss(line);
		std::string directive;
		iss >> directive;
		std::cout << BLUE "directive: " << directive << RESET << std::endl;
		std::cout << line << std::endl;
	}
	file.close();
}
