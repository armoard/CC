#include "../argparse.hpp"
#include "sort.h"

int main(int argc, char* argv[]) {
    argparse::ArgumentParser program("cut_executable");

    program.add_argument("filename")
        .help("File to process. If not provided, standard input will be used.")
        .default_value(std::string(""));  

    program.add_argument("-o", "--option")
        .help("Select type of sort that is going to be applied to file input")
        .default_value(std::string("n"));  

    
    program.add_argument("-u", "--unique")
        .help("Only preserve unique words in file input")
        .default_value(false)
        .implicit_value(true);

    try {
        program.parse_args(argc, argv);
    } catch (const std::runtime_error& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        return 1;
    }

    std::string filename = program.get<std::string>("filename");
    std::string sortOption = program.get<std::string>("-o"); 
    bool unique = program.get<bool>("-u");

    std::istream* input_stream;
    std::ifstream file;

    if (filename.empty()) {
        input_stream = &std::cin;
    } else {
        file.open(filename);
        if (!file) {
            std::cerr << "Error: Could not open file " << filename << std::endl;
            return 1;
        }
        input_stream = &file;
    
    }

    sorter s(*input_stream, unique, sortOption);
    s.start();
    return 0;
   
}