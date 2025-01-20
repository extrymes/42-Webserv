/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msimao <msimao@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/18 15:10:01 by sabras            #+#    #+#             */
/*   Updated: 2025/01/20 14:56:05 by msimao           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include "colors.h"
#include "webserv.hpp"
#include "socket.hpp"

int main(int ac, char **av) {
	if (ac != 2) {
		std::cerr << RED "Usage:\n\t./webserv <config_file>" RESET << std::endl;
		return 1;
	}
	try {
		t_config serverConfig;
		readFile(av[1]);
		handle_socket(serverConfig);
	}
	catch(const std::exception& e) {
		std::cerr << RED "Error: " << e.what() << RESET << std::endl;
	}

	return 0;
}
