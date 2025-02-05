#pragma once
# include <sys/types.h>
# include <sys/socket.h>
# include <unistd.h>
# include <string>
# include <netinet/in.h>
# include <poll.h>
# include <map>

class ClientRequest;

// --- Definitions ---
typedef std::map<std::string, std::string> ssMap;
typedef std::map<int, std::string> isMap;
typedef std::map<int, ClientRequest*> cMap;

// --- Classes ---
class ClientRequest {
	public:

		ClientRequest();
		~ClientRequest();

		void parseBuffer(char *buffer, ssize_t size);
		void parseRequestHost(std::istringstream &infileBuff, ssize_t &i);
		void parseHeader(std::string line);

		const std::string getValueHeader(std::string key);
		ssMap getHeaderMap() const;
		void setBody(std::string body);
		const std::string getBody();
		const std::string getServerResponse();

		void setValueHeader(std::string key, std::string value);
		void setServerResponse(std::string serverResponse);

		void clearServerResponse();
		void clearBody();
		void clearHeader();

	private:

		ssMap	_header;
		std::string	_body;
		std::string	_serverResponse;
};
