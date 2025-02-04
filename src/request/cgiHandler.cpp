#include "cgiHandler.hpp"

bool isCGIFile(std::string url) {
	if (url.find(".py") != std::string::npos || url.find(".php") != std::string::npos)
		return true;
	return false;
}

char **createCGIEnvironment(ssMap headerMap, std::string body) {
	std::vector<std::string> env;
	for (ssMap::iterator it = headerMap.begin(); it != headerMap.end(); ++it)
		env.push_back(it->first + "=" + it->second);
	// Convert to char array
	char **envp = new char *[env.size() + 1];
	size_t i;
	for (i = 0; i < env.size(); ++i)
		envp[i] = strdup(env[i].c_str());
	envp[i] = strdup(("body=" + body).c_str());
	envp[i + 1] = NULL;
	return envp;
}

std::string executeCGI(std::string url, std::string root, ssMap headerMap, std::string body) {
	int pipefd[2];
	if (pipe(pipefd) == -1)
		throw std::runtime_error("pipe error");
	pid_t pid = fork();
	if (pid < 0)
		throw std::runtime_error("fork error");
	std::string output = "";
	if (pid == 0) {
		// Child process
		close(pipefd[0]);
		dup2(pipefd[1], STDOUT_FILENO);
		close(pipefd[1]);
		url = root.empty() ? url : root + url;
		char *argv[] = {const_cast<char*>(url.c_str()), NULL};
		char **envp = createCGIEnvironment(headerMap, body);
		execve(url.c_str(), argv, envp);
		freeCGIEnvironment(envp);
		throw std::runtime_error("child process failed");
	} else {
		// Parent process
		close(pipefd[1]);
		char buffer[1024];
		int bytesRead;
		while ((bytesRead = read(pipefd[0], buffer, sizeof(buffer))))
			output.append(buffer, bytesRead);
		close(pipefd[0]);
		int status;
		waitpid(pid, &status, 0);
		if (!WIFEXITED(status) || WEXITSTATUS(status) != 0)
			throw std::runtime_error("CGI script execution failed");
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
