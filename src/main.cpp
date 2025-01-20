/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sabras <sabras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/18 15:10:01 by sabras            #+#    #+#             */
/*   Updated: 2025/01/20 11:13:43 by sabras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include "webserv.hpp"

int main(int ac, char **av) {
	if (ac != 2) {
		std::cerr << RED "Usage:\n\t./webserv <config_file>" RESET << std::endl;
		return 1;
	}
	try {
		readFile(av[1]);
	} catch (std::exception &e) {
		std::cerr << RED "Error: " << e.what() << RESET << std::endl;
	}
	return 0;
}
