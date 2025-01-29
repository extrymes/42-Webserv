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

		void	parseBuffer(char *buffer);
		void	parseFirstLines(std::stringstream &infileBuff);
		void	parseHeader(std::string line);
		void	parseBody(std::string line);

		const std::string					getValue(std::string key);
		std::map<std::string, std::string>	getHeaders() const;
		const std::string					getServerResponse(int i);
		const std::string					getBody(std::string key);


		void	setValue(std::string key, std::string value);
		void	setServerResponse(std::string responseServer, int i);

		void	clearServerResponse(int i);

	private:

		std::map<std::string, std::string> _headers;
		std::map<int, std::string> _responseServer;
		std::map<std::string, std::string> _body;
};
