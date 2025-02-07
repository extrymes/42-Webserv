#include "socket.hpp"

std::string checkExt(std::string file) {
	const char *ext = strrchr(file.c_str(), '.');
	if (!ext)
		return "text/html";
	std::string str = ext;
	if (str == ".js")
		return "application/javascript";
	else if (str == ".css")
		return "text/css";
	else if (str == ".html")
		return "text/html";
	else
		return "text/plain";
}

bool isError(std::string &index) {
	int nb = atoi(index.c_str());
	if (index.size() == 3 && (nb >= 300 && nb <= 527))
		return true;
	return false;
}

std::string redir(locIt &location) {
	std::string httpResponse = "HTTP/1.1 " + location->redirCode + "\r\n";
	httpResponse += "Location: " + location->redirPath + "\r\n";
	httpResponse += "\r\n";
	return httpResponse;
}

std::string httpResponse(std::string file, std::string ext, std::string code) {
	std::string httpResponse = "HTTP/1.1 " + code + "\r\n";
	httpResponse += "Content-Type: " + ext + "\r\n";
	httpResponse += "Content-Length: " + toString(file.length()) + "\r\n";
	httpResponse += "Connection: close\r\n";
	httpResponse += "\r\n";
	if (!file.empty())
		httpResponse += file;
	return httpResponse;
}

std::string errorHtml(std::string code) {
	std::string nb(code, 3);
	std::string file =
	"<!DOCTYPE html>\n"
		"<html lang=\"en\">\n"
		"<head>\n"
		"	<meta charset=\"UTF-8\">\n"
		"	<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
		"	<title>Title</title>\n"
		"	<style>\n"
		"		body {\n"
		"			font-family: Arial, sans-serif;\n"
		"			text-align: center;\n"
		"			padding: 50px;\n"
		"			background-color: #f8f9fa;\n"
		"		}\n"
		"		h1 {\n"
		"			font-size: 3em;\n"
		"			color: #e74c3c;\n"
		"		}\n"
		"		p {\n"
		"			font-size: 1.5em;\n"
		"			color: #333;\n"
		"		}\n"
		"	</style>\n"
		"</head>\n"
		"<body>\n"
		"	<h1>Error " + code + "</h1>\n"
		"	<p>Oops! Something went wrong (HTTP Error " + nb + ").</p>\n"
		"</body>\n"
		"</html>";
		return httpResponse(file, "text/html", code);
}

std::string errorPage(servIt server, std::string code) {
	std::string nb(code, 0, 3);
	std::cout << nb << std::endl;
	int err = std::atoi(nb.c_str());
	isMap::iterator errNum = server->errorPages.find(err);
	if (errNum != server->errorPages.end())
		return (readHtml(errNum->second, server, code, ""));
	std::string file = errorHtml(code);
	return file;
}

std::string	displayDirectory(std::string index) {
	std::string	display;
	size_t	found = index.find_first_of("/") + 1;
	std::string newIndex(index, found);
	// std::cout << newIndex << std::endl;
	DIR* dir = opendir(index.c_str());
	struct dirent* entry;
	while ((entry = readdir(dir)) != NULL) {
		std::string dirName(entry->d_name);
		display += "<p><a href=" + dirName + ">" + dirName + "</a>\n" + "</p>";
	}
	closedir(dir);
	return display;
}

std::string handleAutoIndex(std::string code, std::string index, servIt server, std::string clientUrl) {
	locIt location;
	for(location = server->locations.begin(); location != server->locations.end(); ++location) {
		if (location->path == clientUrl)
			break;
	}
	if ((location != server->locations.end() && location->autoindex != "on") || (location == server->locations.end() && server->autoindex != "on"))
		return (errorPage(server, CODE403));
	std::string nb(code, 3);
	std::string file =
	"<!DOCTYPE html>\n"
	"<html lang=\"en\">\n"
	"<head>\n"
	"	<meta charset=\"UTF-8\">\n"
	"	<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
	"	<title>Title</title>\n"
	"	<style>\n"
	"		body {\n"
	"			font-family: Arial, sans-serif;\n"
	"			text-align: center;\n"
	"			padding: 50px;\n"
	"			background-color: #f8f9fa;\n"
	"		}\n"
	"		h1 {\n"
	"			font-size: 3em;\n"
	"			color:rgb(4, 85, 123);\n"
	"		}\n"
	"	</style>\n"
	"</head>\n"
	"<body>\n"
	"	<h1>Index of " + index + "</h1>\n"
	"	<div>\n"
	"		" + displayDirectory(index) + "\n"
	"	</div>\n"
	"</body>\n"
	"</html>";
	return httpResponse(file, "text/html", code);
}

std::string readHtml(std::string index, servIt server, std::string code, std::string clientUrl) {
	std::string	line;
	std::string	finalFile;

	if (isError(index))
		return errorPage(server, code);
	int isDir = open(index.c_str(), O_DIRECTORY);
	if (isDir > 0)
		return (close(isDir), handleAutoIndex(code, index, server, clientUrl));
	std::ifstream	infile(index.c_str());
	if (!infile)
		return errorPage(server, CODE404);
	while (std::getline(infile, line))
		finalFile += line + "\n";
	infile.close();
	return httpResponse(finalFile, checkExt(index), code);
}
