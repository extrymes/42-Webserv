#pragma once
# include <string>
# include <vector>
# include <map>
# include <fstream>

// --- Structures ---
typedef struct s_server t_server;
typedef struct s_location t_location;

struct s_server {
	std::string host; // Server host
	int port; // Server post
	std::string name; // Server name
	std::map<int, std::string> errorPages; // Error pages
	long clientMaxBodySize; // Client max body size
	std::vector<t_location> locations; // Server routes
};

struct s_location {
	std::string path; // Location path
	std::string root; // Default root
	std::string index; // Index files
	std::string autoindex; // Directory listing
	std::string allowedMethods; // Allowed HTTP methods
	std::string redirPath; // HTTP redirection
	int redirCode; // HTTP redirection code
};

// --- Classes ---
class ParseConfig {
	public:
		ParseConfig(std::string filename, std::vector<t_server> &servers);
		~ParseConfig();
	private:
		void fillServer(t_server &server);
		void fillLocation(t_location &location);
		bool parseLine(std::string line, std::string &directive, std::string &value);
		void parseListen(std::string value, std::string &host, int &port);
		void parseServerName(std::string value, std::string &serverName);
		void parseErrorPage(std::string value, std::map<int, std::string> &errorPages);
		void parseClientMaxBodySize(std::string value, long &clientMaxBodySize);
		void parseLocationPath(std::string value, std::string &path);
		void trim(std::string &str);
		void error(std::string message);
		std::string _filename;
		std::ifstream _file;
		int _lineId;
};
