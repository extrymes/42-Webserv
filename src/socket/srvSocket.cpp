/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   srvSocket.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msimao <msimao@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/20 13:46:38 by msimao            #+#    #+#             */
/*   Updated: 2025/01/20 14:51:59 by msimao           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "socket.hpp"
#include "webserv.hpp"

sockaddr_in init_sockaddr_in(t_config serverConfig) {
	sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(PORT);
	return (server_addr);
}

void handle_socket(t_config serverConfig)
{
	int server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd < 0)
		throw std::runtime_error("socket fail");
	sockaddr_in server_addr = init_sockaddr_in(serverConfig);
	if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
		throw std::runtime_error("bind fail");
	if (listen(server_fd, 5) < 0)
		throw std::runtime_error("listen fail");
}
