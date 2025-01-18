/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sabras <sabras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/18 15:10:01 by sabras            #+#    #+#             */
/*   Updated: 2025/01/18 15:26:14 by sabras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include "colors.h"

int main(int ac, char **) {
	if (ac != 2) {
		std::cerr << RED "Usage:\n\t./webserv <config_file>" RESET << std::endl;
		return 1;
	}
	return 0;
}
