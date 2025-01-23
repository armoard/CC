#ifndef PARSER_H
#define PARSER_H

#include "Executer.h"
#include <vector>
#include <string>

class Parser {
private:
    std::vector<std::string> args; 
    Executer executer;         

public:
    Parser(int argc, char* argv[]); 
    void parseAndExecute();         
};

#endif