
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>   
#include <unistd.h>  
#include <iostream>  
#include "respparser.h"
#include "commandprocessor.h"

class server{
    private:
        int port;
        int server_fd;
        std::shared_ptr<std::unordered_map<std::string, RESPValue>> store =
        std::make_shared<std::unordered_map<std::string, RESPValue>>();
        std::vector<std::thread> threads;
        std::mutex store_mutex;
        void handleClient(int client_fd){
            char buffer[1024];
            RESPParser parser;
            commandprocessor processor(store, &store_mutex);
        
            while (true) {
                memset(buffer, 0, sizeof(buffer));
                ssize_t bytes = recv(client_fd, buffer, sizeof(buffer), 0);
        
                if (bytes <= 0) {
                    std::cout << "Client disconnected or error occurred.\n";
                    break;
                }
        
                std::string input(buffer);
                std::cout << "Message received: " << input << std::endl;
        
                try {
                    RESPValue command = parser.deserialize(input);
                    std::string response = parser.serialize(processor.process(command));
                    send(client_fd, response.c_str(), response.size(), 0);
                } catch (const std::exception& ex) {
                    std::string error = "-ERR " + std::string(ex.what()) + "\r\n";
                    send(client_fd, error.c_str(), error.size(), 0);
                }
            }
        
            close(client_fd);
        }
    public:
        server(int port) : port(port){
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
        void run(){
            while (true) {
                sockaddr_in client_addr{};
                socklen_t client_len = sizeof(client_addr);
                int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
    
                if (client_fd == -1) {
                    std::cerr << "Error in accept\n";
                    continue;
                }
    
                std::cout << "Client connected from " << inet_ntoa(client_addr.sin_addr) << "\n";
    

                threads.emplace_back(&server::handleClient, this, client_fd);
            }
        }
        ~server(){
            for (auto& t : threads) {
                if (t.joinable()) t.join();
            }
        }
};