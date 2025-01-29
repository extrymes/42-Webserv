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
		void setValue(std::string key, std::string value);
		const std::string getValue(std::string key);
		std::map<std::string, std::string> getHeaders() const;
		void setServerResponse(std::string responseServer, int i);
		void clearServerResponse(int i);
		const std::string getServerResponse(int i);
	private:
		std::map<std::string, std::string> _headers;
		std::map<int, std::string> _responseServer;
};
