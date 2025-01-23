#include "executer.h"
#include <fstream>
#include <iostream>
#include <sstream> 
#include <locale>
#include <codecvt>

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
    std::ifstream file(fileName);

    if (!file.is_open()) {
        std::cerr << "Error: Unable to open file: " << fileName << std::endl;
        return;
    }
    int wordCount = 0;
    std::string line;
    while (std::getline(file, line)){
        std::stringstream ss(line);
        std::string word;
        while (ss >> word) {  
            wordCount++;  
        }
    }
    file.close();
    std::cout << wordCount << " words in " << fileName << std::endl;
}

void Executer::countChars() {
    std::ifstream file(fileName, std::ios::binary);

    if (!file.is_open()) {
        std::cerr << "Error: Unable to open file: " << fileName << std::endl;
        return;
    }

    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>()); 

    file.close();

    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter; // object to convert from utf-8 to utf-16 
    std::wstring wide_content = converter.from_bytes(content); 

    int charCount = wide_content.length();

    std::cout << charCount << " " << fileName << std::endl;
}

void Executer::countBytes() {
    std::ifstream file(fileName, std::ios::binary);

    if (!file.is_open()) {
        std::cerr << "Error: Unable to open file: " << fileName << std::endl;
        return;
    }

    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    file.close();

    std::cout << content.length() << " " << fileName << std::endl;
}
