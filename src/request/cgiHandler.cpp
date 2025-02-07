#include "cgiHandler.hpp"

bool isCGIFile(std::string url) {
	if (url.find(".py") != std::string::npos || url.find(".php") != std::string::npos)
		return true;
	return false;
}

char **createEnvWithBody(ssMap headerMap, std::string body) {
	std::vector<std::string> env;
	for (ssMap::iterator it = headerMap.begin(); it != headerMap.end(); ++it)
		env.push_back(it->first + "=" + it->second);
	// Convert to char array
	char **envp = new char *[env.size() + 2];
	size_t i = 0;
	for (; i < env.size(); ++i)
		envp[i] = strdup(env[i].c_str());
	if (body.empty())
		return (envp[i] = NULL, envp);
	envp[i] = strdup(("body=" + body).c_str());
	envp[i + 1] = NULL;
	return envp;
}

char **createEnvWithArgs(ssMap headerMap, std::vector<std::string> args) {
	std::vector<std::string> env;
	for (ssMap::iterator it = headerMap.begin(); it != headerMap.end(); ++it)
		env.push_back(it->first + "=" + it->second);
	for (std::vector<std::string>::iterator it = args.begin(); it != args.end(); ++it)
		env.push_back(*it);
	// Convert to char array
	char **envp = new char *[env.size() + 1];
	size_t i = 0;
	for (; i < env.size(); ++i)
		envp[i] = strdup(env[i].c_str());
	envp[i] = NULL;
	return envp;
}

std::string parseURL(std::string &url) {
	std::string body;
	// Find query params in URL
	size_t i = url.find_last_of('?');
	if (i == std::string::npos)
		return body;
	// Update body with query params
	body = url.substr(i + 1);
	// Remove query params from url
	url = url.substr(0, i);
	return body;
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
		char **envp;
		if (body.empty()) {
			body = parseURL(url);
			envp = createEnvWithBody(headerMap, body);
		} else
			envp = createEnvWithBody(headerMap, body);
		int i = 0;
		while (envp[i])
			std::cerr << envp[i++] << std::endl;
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
