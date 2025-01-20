/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   reader.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sabras <sabras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/20 10:47:43 by sabras            #+#    #+#             */
/*   Updated: 2025/01/20 12:57:24 by sabras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

void readFile(std::string filename) {
	std::ifstream file(filename.c_str());
	if (!file.is_open())
		throw std::runtime_error("cannot open config file");
	std::string line;
	while (std::getline(file, line)) {
		if (line.empty())
			continue;
		trim(line);
		if (line[0] == '#')
			continue;
		std::cout << line << std::endl;
	}
	file.close();
}
