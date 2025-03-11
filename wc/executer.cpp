#include "executer.h"
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <vector>
#include <algorithm>
#include <string>
#include <codecvt>
#include <locale>

Executer::Executer(std::string fileName) : fileName(fileName) {}

void Executer::noArgs() {
    int lines = countLines();
    int words = countWords();
    int chars = countChars();
    int bytes = countBytes();
    
    std::cout << lines << " " << words << " " << chars << " " << bytes;
    if (!fileName.empty()) std::cout << " " << fileName;
    std::cout << std::endl;
}

int Executer::countLines() {
    if (fileName.empty()) {
        return countLines(std::cin);  
    }

    std::ifstream file(fileName);
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open file: " << fileName << std::endl;
        return -1;
    }
    return countLines(file);
}

int Executer::countLines(std::istream& in) { 
    return std::count(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>(), '\n');
}

int Executer::countWords() {
    std::istream* input;
    std::ifstream file;

    if (!fileName.empty()) {
        file.open(fileName);
        if (!file.is_open()) {
            std::cerr << "Error: Unable to open file: " << fileName << std::endl;
            return -1;
        }
        input = &file;
    } else {
        input = &std::cin;
    }

    return std::distance(std::istream_iterator<std::string>(*input), std::istream_iterator<std::string>());
}
int Executer::countChars() {
    std::istream* input;
    std::ifstream file;

    if (!fileName.empty()) {
        file.open(fileName, std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "Error: Unable to open file: " << fileName << std::endl;
            return -1;
        }
        input = &file;
    } else {
        input = &std::cin;
    }


    std::string content((std::istreambuf_iterator<char>(*input)), std::istreambuf_iterator<char>());

    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    std::wstring wide_content = converter.from_bytes(content);

    return wide_content.length();  
}

int Executer::countBytes() {
    std::istream* input;
    std::ifstream file;

    if (!fileName.empty()) {
        file.open(fileName, std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "Error: Unable to open file: " << fileName << std::endl;
            return -1;
        }
        input = &file;
    } else {
        input = &std::cin;
    }

    return std::distance(std::istreambuf_iterator<char>(*input), std::istreambuf_iterator<char>());
}