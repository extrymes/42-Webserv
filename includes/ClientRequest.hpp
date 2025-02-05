#pragma once
# include <sys/types.h>
# include <sys/socket.h>
# include <unistd.h>
# include <string>
# include <netinet/in.h>
# include <poll.h>
# include <map>

// --- Definitions ---
typedef std::map<std::string, std::string> ssMap;
typedef std::map<int, std::string> isMap;

// --- Classes ---
class ClientRequest {
	public:

		ClientRequest();
		~ClientRequest();

		void parseBuffer(char *buffer, ssize_t size);
		void parseRequestHost(std::istringstream &infileBuff, ssize_t &i);
		void parseHeader(std::string line);

		const std::string getValueHeader(std::string key);
		ssMap getHeaders() const;
		std::string getBodyString() const;
		const std::string getServerResponse(int i);

		void setValueHeader(std::string key, std::string value);
		void setServerResponse(std::string serverResponse, int i);

		void clearServerResponse(int i);
		void clearHeader();

	private:

		ssMap _headers;
		isMap _serverResponse;
		std::string	_bodyString;
};
