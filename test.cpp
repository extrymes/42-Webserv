#include "includes/ClientRequest.hpp"
#include "includes/socket.hpp"
#include "includes/cgiHandler.hpp"


int main(int argc, char const *argv[])
{
	for (size_t i = 0; i < 10000; i++)
	{
		std::cout << "a";
	}
	return 0;
}
