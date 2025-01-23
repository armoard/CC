#include <iostream>
#include <fstream>
#include <string>

#include "parser.h"

int main(int argc, char* argv[]){
    Parser parser(argc,argv);
    parser.parseAndExecute();
    return 0;
}
