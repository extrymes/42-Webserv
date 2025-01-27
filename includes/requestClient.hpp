#pragma once
# include <sys/types.h>
# include <sys/socket.h>
# include <unistd.h>
# include <string>
# include <netinet/in.h>
# include <poll.h>
# include <map>

class RequestClient {
	public:

		RequestClient();
		~RequestClient();
		void parseBuffer(char *buffer);
		void parseFirstLines(std::stringstream &infileBuff);
		void parseHeader(std::string line);
		std::string getValue(std::string key);
		void setValue(std::string key, std::string value);

	private:
		std::map<std::string, std::string> _data;
};
