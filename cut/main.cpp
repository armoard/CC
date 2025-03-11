#include "../argparse.hpp"
#include "cut.h"

int main(int argc, char* argv[]) {
    argparse::ArgumentParser program("cut_executable");

    program.add_argument("filename")
        .help("File to process. If not provided, standard input will be used.")
        .default_value(std::string(""))  // If empty, use stdin
        .nargs(1);

    program.add_argument("-f", "--fields")
        .help("Specifies the columns to extract (e.g., -f1, -f123, -f3)")
        .default_value(std::string("all"))
        .nargs(1);

    program.add_argument("-d", "--delimiter")
        .help("Specifies the delimiter (default is tabulation)")
        .default_value(std::string("\t"))
        .nargs(1);

    try {
        program.parse_args(argc, argv);
    } catch (const std::runtime_error& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        return 1;
    }

    // retrieve argument values
    std::string filename = program.get<std::string>("filename");
    std::string delimiterStr = program.get<std::string>("-d");
    char delimiter = delimiterStr.empty() ? '\t' : delimiterStr[0];

    // parse -f to get the column numbers
    std::string fieldsStr = program.get<std::string>("-f");
    std::vector<int> fields;

    if (fieldsStr == "all") {
        fields.clear();  // If "all" is provided, select all columns
    } else {
        std::stringstream ss(fieldsStr);
        std::string column;
        
        while (std::getline(ss, column, ',')) { 
            try {
                fields.push_back(std::stoi(column)); 
            } catch (...) {
                std::cerr << "Invalid column number: " << column << std::endl;
                return 1;
            }
        }
    }

    // determine whether to read from file or stdin
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

    Cut cut(fields, delimiter);
    cut.step1(*input_stream);

    return 0;
}