#pragma once
# include <string>
# include <vector>
# include <map>

// --- Structures ---
typedef struct s_config t_config;
typedef struct s_server t_server;
typedef struct s_location t_location;

struct s_config {
	std::vector<t_server> servers;
};

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
class Config {
	public:
		static void parseConfigFile(std::string filename, t_config &config);
		static void fillServer(std::ifstream &file, t_server &server);
		static void fillLocation(std::ifstream &file, t_location &location);
		static void trim(std::string &str);
		static void extractDirectiveValue(std::string str, std::string &directive, std::string &value);
		static void extractHostPort(std::string str, std::string &host, int &port);
	private:
		Config();
		~Config();
};
