#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <chrono>

class LoadBalancer {
public:
    LoadBalancer(int port) : port(port) {
        lb_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (lb_fd == -1) {
            throw std::runtime_error("Error creating Load Balancer socket");
        }

        sockaddr_in lb_addr{};
        lb_addr.sin_family = AF_INET;
        lb_addr.sin_addr.s_addr = INADDR_ANY;
        lb_addr.sin_port = htons(port);

        if (bind(lb_fd, (struct sockaddr*)&lb_addr, sizeof(lb_addr)) == -1) {
            throw std::runtime_error("Error in bind");
        }

        if (listen(lb_fd, 5) == -1) {
            throw std::runtime_error("Error in listen");
        }

        std::cout << "Load Balancer running on port " << port << "...\n";
        
        // start the health check in a separate thread
        std::thread(&LoadBalancer::healthCheck, this).detach();
    }

    void run() {
        while (true) {
            sockaddr_in client_addr{};
            socklen_t client_len = sizeof(client_addr);
            int client_fd = accept(lb_fd, (struct sockaddr*)&client_addr, &client_len);

            if (client_fd == -1) {
                std::cerr << "Error in accept\n";
                continue;
            }

            std::cout << "Client connected, redirecting...\n";
            std::thread(&LoadBalancer::forwardRequest, this, client_fd).detach(); //passes instance since fowardrequest uses private members from loadbalancer
        }
    }

private:
    int lb_fd;
    int port;
    std::vector<std::string> all_servers = { "127.0.0.1:8081", "127.0.0.1:8082"}; 
    std::vector<std::string> backend_servers;  
    std::mutex servers_mutex;

    void forwardRequest(int client_fd) {
        std::lock_guard<std::mutex> lock(servers_mutex);
        if (backend_servers.empty()) {
            std::cerr << "No available servers\n";
            close(client_fd);
            return;
        }

        // select backend using Round Robin
        static int server_index = 0;
        std::string server = backend_servers[server_index];
        server_index = (server_index + 1) % backend_servers.size();

        std::string ip = server.substr(0, server.find(":"));
        int port = std::stoi(server.substr(server.find(":") + 1));

        int backend_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (backend_fd == -1) {
            std::cerr << "Error creating backend socket\n";
            close(client_fd);
            return;
        }

        sockaddr_in backend_addr{};
        backend_addr.sin_family = AF_INET;
        backend_addr.sin_port = htons(port);
        inet_pton(AF_INET, ip.c_str(), &backend_addr.sin_addr);

        if (connect(backend_fd, (struct sockaddr*)&backend_addr, sizeof(backend_addr)) == -1) {
            std::cerr << "Error connecting to backend\n";
            close(client_fd);
            close(backend_fd);
            return;
        }

        char buffer[4096];  
        int bytes_received;
        while ((bytes_received = recv(client_fd, buffer, sizeof(buffer), 0)) > 0) {
            send(backend_fd, buffer, bytes_received, 0);
        } //receive from client to backend
        
        while ((bytes_received = recv(backend_fd, buffer, sizeof(buffer), 0)) > 0) {
            send(client_fd, buffer, bytes_received, 0);
        }  //from backend to client

        close(client_fd);
        close(backend_fd);
    }

    void healthCheck() {
        while (true) {
            std::this_thread::sleep_for(std::chrono::seconds(10)); // run every 10 seconds

            std::vector<std::string> active_servers;
            {   // use braces for scope lock. when this block goes out of scope, the lock is released, 
                // allowing other threads to access backend_servers safely. 
                std::lock_guard<std::mutex> lock(servers_mutex);
                for (const auto& server : all_servers) {
                    std::string ip = server.substr(0, server.find(":"));
                    int port = std::stoi(server.substr(server.find(":") + 1));

                    int test_fd = socket(AF_INET, SOCK_STREAM, 0);
                    if (test_fd == -1) continue;

                    sockaddr_in server_addr{};
                    server_addr.sin_family = AF_INET;
                    server_addr.sin_port = htons(port);
                    inet_pton(AF_INET, ip.c_str(), &server_addr.sin_addr);

                    if (connect(test_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
                        std::cerr << "HealthCheck failed for " << server << " - Error: " << strerror(errno) << std::endl;
                    } else {
                        active_servers.push_back(server);
                    }
                    close(test_fd);
                }
            }

            {
                std::lock_guard<std::mutex> lock(servers_mutex);
                backend_servers = active_servers; 
            }

            std::cout << "Active servers: ";
            for (const auto& s : backend_servers) std::cout << s << " ";
            std::cout << "\n";
        }
    }
};