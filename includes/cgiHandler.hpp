#pragma once
# include <unistd.h>
# include <iostream>
# include <sstream>
# include <sstream>
# include <string>
# include <cstring>
# include <vector>
# include <map>
# include <sys/wait.h>
# include <ctime>

// --- Definitions ---
typedef std::map<std::string, std::string> ssMap;

// --- Functions ---
bool isCGIFile(std::string url);
char **createCGIEnvironment(ssMap headerMap, std::string body, std::string uploadLocation);
std::string parseURL(std::string &url);
void freeCGIEnvironment(char **envp);
std::string executeCGI(std::string url, std::string root, ssMap header, std::string body, std::string uploadLocation);
