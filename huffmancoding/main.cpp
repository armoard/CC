#include "compressor.h"


int main(int argc, char* argv[]){
    std::string fileName = argv[1];
    Compressor counter(fileName); 
    counter.countFreq();
    counter.heapify();
    return 0;
}