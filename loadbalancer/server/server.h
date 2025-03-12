#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <vector>
#include <thread>

class Server {
private:
    int server_fd;
    int port;
    std::vector<std::thread> threads; // 

    // client handling in a separate thread
    static void handleClient(int client_fd, int port) {
        char buffer[1024];
        memset(buffer, 0, sizeof(buffer));

        // receive data from client
        recv(client_fd, buffer, sizeof(buffer), 0);
        std::cout << "Message received: " << buffer << std::endl;

        std::string response = 
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/plain\r\n"
            "Connection: close\r\n";

        std::string body = "Message received from port " + std::to_string(port) + "\n";
        response += "Content-Length: " + std::to_string(body.size()) + "\r\n\r\n" + body;


        send(client_fd, response.c_str(), response.size(), 0);
        close(client_fd);
    }

public:
    Server(int port) : port(port) {
        server_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (server_fd == -1) {
            throw std::runtime_error("Error creating socket");
        }

        sockaddr_in server_addr{};
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = INADDR_ANY;
        server_addr.sin_port = htons(port);

        if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
            throw std::runtime_error("Error in bind");
        }

        if (listen(server_fd, 5) == -1) {
            throw std::runtime_error("Error in listen");
        }

        std::cout << "Server started on port " << port << "...\n";
    }

    // method to accept connections
    void run() {
        while (true) {
            sockaddr_in client_addr{};
            socklen_t client_len = sizeof(client_addr);
            int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);

            if (client_fd == -1) {
                std::cerr << "Error in accept\n";
                continue;
            }

            std::cout << "Client connected from " << inet_ntoa(client_addr.sin_addr) << "\n";

            // create a thread to handle the client and pass the port
            threads.emplace_back(handleClient, client_fd, port);
        }
    }

    ~Server() {
        close(server_fd);
        for (auto& t : threads) {
            if (t.joinable()) {
                t.join(); // wait for threads to finish before destroying the object
            }
        }
    }
};
