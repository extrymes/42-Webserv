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

std::string redir(std::vector<t_location>::iterator &location) {
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

std::string errorPage(int error, servIt server, std::string code) {
	std::string err;
	isMap::iterator errNum = server->errorPages.find(error);
	if (errNum == server->errorPages.end())
		err = toString(error);
	else
		return (readHtml(errNum->second, server, code));
	std::string file =
	"<!DOCTYPE html>\n"
		"<html lang=\"en\">\n"
		"<head>\n"
		"	<meta charset=\"UTF-8\">\n"
		"	<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
		"	<title>Error " + err + "</title>\n"
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
		"	<p>Oops! Something went wrong (HTTP Error " + err + ").</p>\n"
		"</body>\n"
		"</html>";
	return httpResponse(file, "text/html", code);
}

std::string readHtml(std::string index, servIt server, std::string code) {
	std::string	line;
	std::string	finalFile;

	if (isError(index))
		return errorPage(std::atoi(index.c_str()), server, code);
	int isDir = open(index.c_str(), O_DIRECTORY);
	if (isDir > 0)
		return (close(isDir), errorPage(404, server, CODE404));
	std::ifstream	infile(index.c_str());
	if (!infile)
		return errorPage(404, server, CODE404);
	while (std::getline(infile, line))
		finalFile += line + "\n";
	infile.close();
	return httpResponse(finalFile, checkExt(index), code);
}
