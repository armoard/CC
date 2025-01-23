#ifndef EXECUTER_H
#define EXECUTER_H

#include <string>

class Executer {
private:
    std::string fileName; 
public:
    Executer(std::string fileName);

    void noArgs();
    int countBytes();
    int countWords();
    int countLines();
    int countLines(std::istream& in);
    int countChars();

};

#endif