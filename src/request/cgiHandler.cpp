#include "cgiHandler.hpp"

bool isCGIFile(std::string url) {
	if (url.find(".py") != std::string::npos || url.find(".php") != std::string::npos)
		return true;
	return false;
}

char **createCGIEnvironment(ssMap headerMap, std::string body, std::string uploadLocation) {
	std::vector<std::string> env;
	for (ssMap::iterator it = headerMap.begin(); it != headerMap.end(); ++it)
		env.push_back(it->first + "=" + it->second);
	// Convert to char array
	char **envp = new char *[env.size() + 3];
	size_t i = 0;
	for (; i < env.size(); ++i)
		envp[i] = strdup(env[i].c_str());
	envp[i] = strdup(("upload_location=" + uploadLocation).c_str());
	if (body.empty())
		return (envp[i] = NULL, envp);
	envp[i] = strdup(("body=" + body).c_str());
	envp[i + 1] = NULL;
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

std::string executeCGI(std::string url, std::string root, ssMap headerMap, std::string body, std::string uploadLocation) {
	int	pipefdOut[2];
	int	pipefdIn[2];
	if (pipe(pipefdOut) == -1 || pipe(pipefdIn) == -1)
		throw std::runtime_error("pipe error");
	pid_t pid = fork();
	if (pid < 0)
		throw std::runtime_error("fork error");
	std::string output = "";
	if (pid == 0) {
		// Child process
		close(pipefdOut[0]);
		if (dup2(pipefdOut[1], STDOUT_FILENO) < 0 || dup2(pipefdIn[0], STDIN_FILENO) < 0)
			throw std::logic_error("Dup2 failed when executing CGI at url: " + url);
		close(pipefdOut[1]);
		close(pipefdIn[0]);
		close(pipefdIn[1]);
		url = root.empty() ? url : root + url;
		char *argv[] = {const_cast<char*>(url.c_str()), NULL};
		char **envp = createCGIEnvironment(headerMap, body, uploadLocation);
		execve(url.c_str(), argv, envp);
		freeCGIEnvironment(envp);
		throw std::runtime_error("child process failed");
	} else {
		// Parent process
		for(int i = 0; i < (int)body.size(); ++i)
			std::cout << body[i];
		write(pipefdIn[1], body.c_str(), body.size());
		close(pipefdIn[0]);
		close(pipefdIn[1]);
		close(pipefdOut[1]);
		char buffer[1024];
		int bytesRead;
		while ((bytesRead = read(pipefdOut[0], buffer, sizeof(buffer))))
			output.append(buffer, bytesRead);
		close(pipefdOut[0]);
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
