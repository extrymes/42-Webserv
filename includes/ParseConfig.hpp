#pragma once
# include <sstream>
# include <fstream>
# include <cstdlib>
# include <string>
# include <vector>
# include <map>

// --- Structures ---
typedef struct s_server t_server;
typedef struct s_location t_location;

struct s_server {
	std::string host; // Server host
	int port; // Server port
	std::string name; // Server name
	std::string root; // Default root
	std::vector<std::string> indexes; // Index files
	std::map<int, std::string> errorPages; // Error pages
	long clientMaxBodySize; // Client max body size
	std::vector<t_location> locations; // Server routes
};

struct s_location {
	std::string path; // Location path
	std::string root; // Default root
	std::vector<std::string> indexes; // Index files
	std::string autoindex; // Directory listing
	std::string allowedMethods; // Allowed HTTP methods
	std::string cgiExtension; // CGI extension
	std::string uploadSave; // Upload save location
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
		bool parseLine(std::string line, std::string &directive, std::string &args);
		void parseListen(std::string args, std::string &host, int &port);
		void parseServerName(std::string args, std::string &serverName);
		void parseRoot(std::string args, std::string &root);
		void parseIndexes(std::string args, std::vector<std::string> &indexes);
		void parseErrorPage(std::string args, std::map<int, std::string> &errorPages);
		void parseClientMaxBodySize(std::string args, long &clientMaxBodySize);
		void parseLocationPath(std::string args, std::string &path);
		void parseLocationAutoindex(std::string args, std::string &autoindex);
		void parseLocationAllowedMethods(std::string args, std::string &allowedMethods);
		void parseLocationCgiExtension(std::string args, std::string &cgiExtension);
		void parseLocationUploadSave(std::string args, std::string &uploadSave);
		void parseLocationRedirection(std::string args, std::string &redirPath, int &redirCode);
		void trim(std::string &str);
		void checkEndCharacter(std::string line);
		int countArgs(std::string args);
		void error(std::string message);
		std::string _filename;
		std::ifstream _file;
		int _lineId;
};
