#ifndef EXECUTER_H
#define EXECUTER_H

#include <string>

class Executer {
private:
    std::string fileName; 
public:
    Executer(std::string fileName);

    void noArgs();
    void countBytes();
    void countWords();
    void countLines();
};

#endif