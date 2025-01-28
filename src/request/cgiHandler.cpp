#include "cgiHandler.hpp"

bool isCGIFile(std::string url) {
	if (url.find(".py") || url.find(".php"))
		return true;
	return false;
}

char **createCGIEnvironment(std::map<std::string, std::string> headers) {
	std::vector<std::string> env;
	env.push_back("REQUEST_METHOD=" + headers["method"]);
	// env.push_back("QUERY_STRING=");
	env.push_back("SCRIPT_FILENAME=");
	env.push_back("CONTENT_TYPE=" + headers["Content-Type"]);
	env.push_back("CONTENT_LENGTH=" + headers["Content-Length"]);
	// Convert to char array
	char **envp = new char *[env.size() + 1];
	for (size_t i = 0; i < env.size(); ++i)
		envp[i] = strdup(env[i].c_str());
	envp[env.size()] = NULL;
	return envp;
}

std::string executeCGI(std::string url, std::map<std::string, std::string> headers) {
	char **envp = createCGIEnvironment(headers);
	int pipefd[2];
	if (pipe(pipefd) == -1)
		throw std::runtime_error("pipe error");
	pid_t pid = fork();
	if (pid < 0)
		throw std::runtime_error("fork error");
	(void)url;
	(void)envp;
	std::string output = "";
	return output;
}
