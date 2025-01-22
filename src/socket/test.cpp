#include <iostream>
#include <poll.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>

#define PORT 8080
#define MAX_CLIENTS 100

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    // Création du socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0) {
        perror("Socket failed");
        return 1;
    }

    // Configuration de l'adresse
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Liaison du socket
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        return 1;
    }

    // Écoute
    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        return 1;
    }

    // Tableau pour `poll()`
    struct pollfd clients[MAX_CLIENTS];
    memset(clients, 0, sizeof(clients));

    // Initialisation du serveur dans le tableau
    clients[0].fd = server_fd;
    clients[0].events = POLLIN;

    int client_count = 1;

    std::cout << "Server is listening on port " << PORT << std::endl;

    while (true) {
        // Appel à `poll()`
        int activity = poll(clients, client_count, -1); // Timeout infini

        if (activity < 0) {
            perror("Poll error");
            break;
        }

        // Vérifier les événements
        for (int i = 0; i < client_count; ++i) {
            if (clients[i].revents & POLLIN) {
                if (clients[i].fd == server_fd) {
                    // Nouvelle connexion
                    new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
                    if (new_socket < 0) {
                        perror("Accept failed");
                        continue;
                    }
                    std::cout << "New connection: " << new_socket << std::endl;

                    // Ajouter au tableau
                    clients[client_count].fd = new_socket;
                    clients[client_count].events = POLLIN;
                    client_count++;
                } else {
                    // Lecture des données
                    char buffer[1024] = {0};
                    int valread = read(clients[i].fd, buffer, sizeof(buffer));
                    if (valread <= 0) {
                        // Déconnexion
                        std::cout << "Client disconnected: " << clients[i].fd << std::endl;
                        close(clients[i].fd);
                        clients[i] = clients[--client_count]; // Remplacement par le dernier
                    } else {
                        std::cout << "Message received: " << buffer << std::endl;
                        send(clients[i].fd, buffer, strlen(buffer), 0);
                    }
                }
            }
        }
    }

    close(server_fd);
    return 0;
}
