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

		void parseBuffer(char *buffer, int i);
		void parseRequestHost(std::istringstream &infileBuff);
		void parseHeader(std::string line);

		const std::string getValueHeader(std::string key);
		ssMap getHeaderMap() const;
		void setBody(int i, std::string body);
		const std::string getBody(int i);
		const std::string getServerResponse(int i);

		void setValueHeader(std::string key, std::string value);
		void setServerResponse(std::string serverResponse, int i);

		void clearServerResponse(int i);
		void clearBody(int i);
		void clearHeader();

	private:

		ssMap _header;
		isMap _body;
		isMap _serverResponse;
};
