#include <iostream>
#include "server.h"
int main(int argc, char* argv[]){
    try {
        Server server(atoi(argv[1])); 
        server.run();         
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return -1;
    }
    return 0;
}