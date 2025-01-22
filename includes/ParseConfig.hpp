#pragma once
# include <string>
# include <vector>
# include <map>

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
		void fillServer(std::ifstream &file, t_server &server);
		void fillLocation(std::ifstream &file, t_location &location);
		std::string parseLine(std::string line);
		void trim(std::string &str);
		void extractDirectiveValue(std::string str, std::string &directive, std::string &value);
		void extractHostPort(std::string str, std::string &host, int &port);
		void error(std::string message);
		std::string _filename;
		int _lineId;
};
