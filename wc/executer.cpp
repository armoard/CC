#include "executer.h"
#include <iostream>
#include <fstream>
#define BUFFERSIZE 4096

Executer::Executer(std::string fileName) : fileName(fileName) {}


void Executer::noArgs() {
    std::cout << "todo." << std::endl;
}
void Executer::countLines() {
    std::ifstream file(fileName);

    if(!file.is_open()){
        std::cerr << "Error: Unable to open file: " << fileName << std::endl;
        return;
    }

    std::string line;
    int lineCount = 0;
    while (std::getline(file,line)){
        lineCount+=1;
    }
    file.close();

    std::cout << lineCount <<  " " << fileName << std::endl;

}
void Executer::countWords() {
    std::cout << "todo." << std::endl;
}

void Executer::countBytes(){
    std::ifstream file(fileName,std::ios::binary);

    if (!file.is_open()) {        
        std::cerr << "Error: Unable to open file: " << fileName << std::endl;
        return;
    }

    char buffer[BUFFERSIZE];
    int byteCount = 0;
    
    while(file.read(buffer,BUFFERSIZE)){
        byteCount+=file.gcount();
    }
    byteCount += file.gcount();

    file.close();
    std::cout << byteCount << " " << fileName << std::endl;

}