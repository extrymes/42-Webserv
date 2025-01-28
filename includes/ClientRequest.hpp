#pragma once
# include <sys/types.h>
# include <sys/socket.h>
# include <unistd.h>
# include <string>
# include <netinet/in.h>
# include <poll.h>
# include <map>

// --- Classes ---
class ClientRequest {
	public:

		ClientRequest();
		~ClientRequest();
		void parseBuffer(char *buffer);
		void parseFirstLines(std::stringstream &infileBuff);
		void parseHeader(std::string line);
		const std::string getValue(std::string key);
		void setValue(std::string key, std::string value);
		std::map<std::string, std::string> getHeaders() const;
	private:
		std::map<std::string, std::string> _headers;
};
