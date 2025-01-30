#pragma once
# include <iostream>
# include <string>
# include <cstring>
# include <vector>
# include <map>
# include <sys/wait.h>

// --- Functions ---
bool isCGIFile(std::string url);
char **createCGIEnvironment(std::map<std::string, std::string> headers);
void freeCGIEnvironment(char **envp);
std::string executeCGI(std::string url, std::string root, std::map<std::string, std::string> headers);
