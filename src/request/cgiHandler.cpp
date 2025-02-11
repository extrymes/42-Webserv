#include "cgiHandler.hpp"
#include "socket.hpp"

bool isCGIFile(std::string url) {
	size_t end = url.find_last_of('?');
	if (end == std::string::npos)
		end = url.size();
	if (url.rfind(".py", end) != std::string::npos || url.rfind(".php", end) != std::string::npos)
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
	for (; i < env.size(); ++i) {
		envp[i] = new char[env[i].size() + 1];
		std::strcpy(envp[i], env[i].c_str());
	}
	std::string tmp;
	tmp = "upload_location=" + uploadLocation;
	envp[i] = new char[tmp.size() + 1];
	std::strcpy(envp[i++], tmp.c_str());
	if (body.empty())
		return (envp[i] = NULL, envp);
	tmp = "body=" + body;
	envp[i] = new char[tmp.size() + 1];
	std::strcpy(envp[i++], tmp.c_str());
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

std::string executeCGI(std::string url, std::string root, ssMap headerMap, std::string body, std::string uploadLocation) {
	int	pipefdOut[2];
	int	pipefdIn[2];
	if (pipe(pipefdOut) == -1 || pipe(pipefdIn) == -1)
		throw HttpException(CODE500, "pipe error");
	pid_t pid = fork();
	if (pid < 0)
		throw HttpException(CODE500, "fork error");
	std::string output = "";
	if (pid == 0) {
		// Child process
		close(pipefdOut[0]);
		if (dup2(pipefdOut[1], STDOUT_FILENO) < 0 || dup2(pipefdIn[0], STDIN_FILENO) < 0)
			throw HttpException(CODE500, "Dup2 failed when executing CGI at url: " + url);
		close(pipefdOut[1]);
		close(pipefdIn[0]);
		close(pipefdIn[1]);
		url = root.empty() ? url : root + url;
		char *argv[] = {const_cast<char*>(url.c_str()), NULL};
		if (body.empty())
			body = parseURL(url);
		char **envp = createCGIEnvironment(headerMap, body, uploadLocation);
		execve(url.c_str(), argv, envp);
		freeCGIEnvironment(envp);
		throw HttpException(CODE500, "child process failed");
	} else {
		// Parent process
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
			throw HttpException(CODE500, "CGI script execution failed");
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
