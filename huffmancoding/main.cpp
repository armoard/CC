#include "compressor.h"


int main(int argc, char* argv[]){
    std::string fileName = argv[1];
    std::string outputName = argv[2];
    Compressor compressor(fileName);
    compressor.compress(outputName);
    return 0;
}