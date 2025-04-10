#include <filesystem>
#include <fstream>
#include <iostream>
#include <regex>
#include <string>

void step1(std::ifstream& input){
    std::string line;
    std::regex re("");
    while (getline(input,line)){
        if (std::regex_search(line,re)){
            std::cout << line << std::endl;
        }
    }
}
void step2(std::ifstream& input,const std::string letra){
    std::string line;
    std::string pattern = "[" + letra + "]";
    std::regex re(pattern);
    while (getline(input,line)){
        if (std::regex_search(line,re)){
            std::cout << line << std::endl;
        }
    }
}
void step3y4(const std::string& loquequeresbuscar, const std::string& path, bool excluir) {
    std::regex re("[" + loquequeresbuscar + "]");
    
    for (const auto& entry : std::filesystem::recursive_directory_iterator(path)) {
        if (!entry.is_regular_file()) continue;
        if (entry.path().extension() != ".txt") continue;

        std::ifstream file(entry.path());
        if (!file.is_open()) continue;

        std::string line;
        while (std::getline(file, line)) {
            bool match = std::regex_search(line, re);
            if ((match && !excluir) || (!match && excluir)) {
                std::cout << entry.path().string() << ":" << line << std::endl;
            }
        }
    }
}

void step5(const std::string& option, std::ifstream& input) {
    std::string line;
    std::regex re;

    if (option == "\\d") {
        re = std::regex("\\d+");  
    }
    else if (option == "\\w") {
        re = std::regex("[a-zA-Z]+");  
    }
    else {
        std::cerr << "cualquiera" << std::endl;
        return;
    }

    while (std::getline(input, line)) {
        if (std::regex_search(line, re)) {
            std::cout << line << std::endl;
        }
    }
}
void step6(const std::string& pattern, std::ifstream& input) {
    std::regex re(pattern);  
    std::string line;

    while (std::getline(input, line)) {
        if (std::regex_search(line, re)) {
            std::cout << line << std::endl;
        }
    }
}
void finalStep(const std::string& pattern, std::ifstream& input, bool ignoreCase) {
    std::regex re;

    if (ignoreCase) {
        re = std::regex(pattern, std::regex_constants::icase);
    } else {
        re = std::regex(pattern);
    }

    std::string line;
    while (std::getline(input, line)) {
        if (std::regex_search(line, re)) {
            std::cout << line << std::endl;
        }
    }
}

int main (int argc, char* argv[]){
    // only need to parse args here and call respective func
    // this was a quick one 
}