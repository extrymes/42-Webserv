# include <sys/types.h>
# include <sys/socket.h>
# include <cstring>
# include <unistd.h>
# include <string>
# include <netinet/in.h>
# include <poll.h>
# include <vector>
# include <iostream>
# include <csignal>
# include <fstream>
# include <sstream>
# include <exception>
# include <algorithm>
# include <fcntl.h>
# include <sstream>
# include <map>
# include <arpa/inet.h>
# include <netdb.h>
# include <cstdio>

int main(void) {
	for (int i = 0; i < 10000; ++i)
		std::cout << "a";
}