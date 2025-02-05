#pragma once
# include <iostream>
# include <string>
# include <cstring>
# include <vector>
# include <map>
# include <sys/wait.h>

// --- Definitions ---
typedef std::map<std::string, std::string> ssMap;

// --- Functions ---
bool isCGIFile(std::string url);
char **createCGIEnvironment(ssMap header, std::string body);
void freeCGIEnvironment(char **envp);
std::string executeCGI(std::string url, std::string root, ssMap header, std::string body);
