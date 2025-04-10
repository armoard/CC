#include <fstream>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <iomanip>

void defaultBehavior(std::istream& input, std::ostream& output) {
    std::string lastline, current;
    bool first = true;
    while (getline(input, current)) {
        if (first || current != lastline) {
            output << current << '\n';
        }
        lastline = current;
        first = false;
    }
}

void printLines(const std::unordered_map<std::string, int>& map,
                const std::unordered_set<std::string>& flags,
                std::ostream& output) {
    bool showCount = flags.count("-c");
    bool onlyDupes = flags.count("-d");
    bool onlyUniques = flags.count("-u");

    for (const auto& [line, count] : map) {
        bool shouldPrint = true;
        if (onlyDupes && count < 2) {
            shouldPrint = false;
        } else if (onlyUniques && count != 1) {
            shouldPrint = false;
        }

        if (shouldPrint) {
            if (showCount) {
                output << std::setw(7) << count << " " << line << '\n';
            } else {
                output << line << '\n';
            }
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: ./uniq [-c] [-d] [-u] input.txt [output.txt]\n";
        return 1;
    }

    std::unordered_set<std::string> flags;
    std::string inputFile, outputFile;

    // parse flags and files
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-c" || arg == "-d" || arg == "-u") {
            flags.insert(arg);
        } else if (inputFile.empty()) {
            inputFile = arg;
        } else if (outputFile.empty()) {
            outputFile = arg;
        } else {
            std::cerr << "Too many file arguments.\n";
            return 1;
        }
    }

    // input stream
    std::ifstream fileInput;
    std::istream* inputStream = &std::cin;
    if (!inputFile.empty() && inputFile != "-") {
        fileInput.open(inputFile);
        if (!fileInput.is_open()) {
            std::cerr << "Could not open input file: " << inputFile << '\n';
            return 1;
        }
        inputStream = &fileInput;
    }
    //output stream
    std::ofstream fileOutput;
    std::ostream* outputStream = &std::cout;
    if (!outputFile.empty() && outputFile != "-") {
        fileOutput.open(outputFile);
        if (!fileOutput.is_open()) {
            std::cerr << "Could not open output file: " << outputFile << '\n';
            return 1;
        }
        outputStream = &fileOutput;
    }

    if (flags.empty()) {
        defaultBehavior(*inputStream, *outputStream);
    } else {
        std::unordered_map<std::string, int> map;
        std::string line;
        while (getline(*inputStream, line)) {
            map[line]++;
        }
        printLines(map, flags, *outputStream);
    }

    return 0;
}
