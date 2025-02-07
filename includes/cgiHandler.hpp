#pragma once
# include <iostream>
# include <sstream>
# include <string>
# include <cstring>
# include <vector>
# include <map>
# include <sys/wait.h>

// --- Definitions ---
typedef std::map<std::string, std::string> ssMap;

// --- Functions ---
bool isCGIFile(std::string url);
char **createEnvWithBody(ssMap headerMap, std::string body);
char **createEnvWithArgs(ssMap headerMap, std::vector<std::string> args);
std::string parseURL(std::string &url);
void freeCGIEnvironment(char **envp);
std::string executeCGI(std::string url, std::string root, ssMap header, std::string body);
std::string executeCGI(std::string url, std::string root, ssMap header, std::string body);
