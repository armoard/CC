#include "server.h"

int main(){
    server server(6379); 
    server.run();
    return 0;
}