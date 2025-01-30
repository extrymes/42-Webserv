#include "cgiHandler.hpp"

bool isCGIFile(std::string url) {
	if (url.find(".py") != std::string::npos || url.find(".php") != std::string::npos)
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

std::string executeCGI(std::string url, std::string root, std::map<std::string, std::string> headers, std::map<std::string, std::string> body) {
	(void)body;
	char **envp = createCGIEnvironment(headers);
	int pipefd[2];
	if (pipe(pipefd) == -1)
		throw std::runtime_error("pipe error");
	pid_t pid = fork();
	if (pid < 0)
		throw std::runtime_error("fork error");
	std::string output = "";
	if (pid == 0) {
		// Child process
		std::cerr << "child" << std::endl;
		dup2(pipefd[1], STDOUT_FILENO);
		std::cerr << "child1" << std::endl;
		close(pipefd[0]);
		std::cerr << "child2" << std::endl;
		url = root.empty() ? url : root + url;
		char **argv = { NULL };
		execve(url.c_str(), argv, envp);
		// std::cerr << "child3" << std::endl;
		freeCGIEnvironment(envp);
		throw std::runtime_error("error in child process");
	} else {
		// Parent process
		std::cerr << "parent" << std::endl;
		close(pipefd[1]);
		std::cerr << "parent1" << std::endl;
		char buffer[1024];
		int bytesRead;
		while ((bytesRead = read(pipefd[0], buffer, sizeof(buffer)))) {
			std::cerr << "ici" << std::endl;
			buffer[bytesRead] = '\0';
			output += buffer;
		}
		close(pipefd[0]);
		std::cerr << "parent2" << std::endl;
		waitpid(pid, NULL, 0);
		std::cerr << "output" << output << std::endl;
		return output;
	}
}

void freeCGIEnvironment(char **envp) {
	if (!envp)
		return;
	for (int i = 0; envp[i]; ++i)
		delete[] envp[i];
	delete[] envp;
}
