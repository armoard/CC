#include "parser.h"
#include <iostream>

Parser::Parser(int argc, char* argv[]) : executer(argc == 2 ? argv[1] : (argc > 2 ? argv[2] : "")) {
    // argv to string 
    for (int i = 0; i < argc; ++i) {
        args.push_back(argv[i]);
    }
}

void Parser::parseAndExecute() {
    if (args.size() == 1) {
        std::cout << executer.countLines(std::cin) << std::endl;
        return;
    }

    if (args.size() == 2) {
        executer.noArgs();
        return;
    }

    std::string command = args[1];

    if (command == "-c") {
        std::cout << executer.countBytes() << std::endl;
        return;
    } else if (command == "-l") {
        std::cout << executer.countLines() << std::endl;
        return;
    } else if (command == "-w") {
       std::cout << executer.countWords() << std::endl;
        return;
    } else if (command == "-m") {
        std::cout << executer.countChars() << std::endl;
        return;
    }
    else {
        std::cerr << "Invalid command." << std::endl;
    }
}