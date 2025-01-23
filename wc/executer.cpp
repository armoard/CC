#include "executer.h"
#include <fstream>
#include <iostream>
#include <sstream> 
#include <locale>
#include <codecvt>
#include <vector>

Executer::Executer(std::string fileName) : fileName(fileName) {}

void Executer::noArgs() {
    int lines = countLines();
    int words = countWords();
    int bytes = countBytes();
    int chars = countChars();

    std::vector<int> results = {lines, words, bytes, chars};

    for (const auto& result : results) {
        std::cout << result << " ";
    }
    std::cout << fileName << std::endl;
}

int Executer::countLines() {
    std::ifstream file(fileName);

    if (!file.is_open()) {
        std::cerr << "Error: Unable to open file: " << fileName << std::endl;
        return -1;
    }
    int lineCount = countLines(file);
    file.close();
    return lineCount; 
}

int Executer::countLines(std::istream& in) { //override 
    std::string line;
    int lineCount = 0;

    while (std::getline(in, line)) {
        lineCount++;
    }

    return lineCount;
}


int Executer::countWords() {
    std::ifstream file(fileName);

    if (!file.is_open()) {
        std::cerr << "Error: Unable to open file: " << fileName << std::endl;
        return -1;
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
    
    return wordCount;
}

int Executer::countChars() {
    std::ifstream file(fileName, std::ios::binary);

    if (!file.is_open()) {
        std::cerr << "Error: Unable to open file: " << fileName << std::endl;
        return -1;
    }

    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>()); 

    file.close();

    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter; // object to convert from utf-8 to utf-16 
    std::wstring wide_content = converter.from_bytes(content); 

    int charCount = wide_content.length();

    return charCount;
}

int Executer::countBytes() {
    std::ifstream file(fileName, std::ios::binary);

    if (!file.is_open()) {
        std::cerr << "Error: Unable to open file: " << fileName << std::endl;
        return -1;
    }

    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    file.close();

    int byteCount = content.length();

    return byteCount;
}
