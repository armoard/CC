#include "server.h"
// this is unfinished unoptimized and need to redo most of things
int main(){
    server server(6379); 
    server.run();
    return 0;
}