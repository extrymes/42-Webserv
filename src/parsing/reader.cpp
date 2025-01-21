#include "webserv.hpp"

void readFile(std::string filename) {
	std::ifstream file(filename.c_str());
	if (!file.is_open())
		throw std::runtime_error("cannot open config file");
	std::string line;
	while (std::getline(file, line)) {
		trim(line);
		if (line.empty())
			continue;
		std::string directive, value;
		extractData(line, directive, value);
		std::cout << RED "directive: " << directive << RESET << std::endl;
		std::cout << BLUE "value: " << value << RESET << std::endl;
		std::cout << line << std::endl;
	}
	file.close();
}
