#include "parser.h"
#include <iostream>

Parser::Parser(int argc, char* argv[]) : executer(argc > 1 ? argv[2] : "") {
    // argv to string 
    for (int i = 0; i < argc; ++i) {
        args.push_back(argv[i]);
    }
}


void Parser::parseAndExecute(){
    if (args.size() < 2 ){
        std::cerr << "Usage: " << args[0] << " <file> <command>" << std::endl;
        return;
    }

    if (args.size() == 2){
        executer.noArgs();
        return;
    }

    std::string command = args[1];

    if(command == "-c"){
        executer.countBytes();
        return;
    }
    else if (command == "-l")
    {
        //todo
    }

 
}