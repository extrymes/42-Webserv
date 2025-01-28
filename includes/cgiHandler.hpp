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
std::string executeCGI(std::string url, std::map<std::string, std::string> headers);
