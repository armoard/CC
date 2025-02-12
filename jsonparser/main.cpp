#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "lexer.h"
#include "parser.h"

std::string tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::LEFT_BRACE: return "LEFT_BRACE";
        case TokenType::RIGHT_BRACE: return "RIGHT_BRACE";
        case TokenType::LEFT_BRACKET: return "LEFT_BRACKET";
        case TokenType::RIGHT_BRACKET: return "RIGHT_BRACKET";
        case TokenType::COMMA: return "COMMA";
        case TokenType::COLON: return "COLON";
        case TokenType::STRING: return "STRING";
        case TokenType::NUMBER: return "NUMBER";
        case TokenType::TRUE_T: return "TRUE";
        case TokenType::FALSE_T: return "FALSE";
        case TokenType::NULL_T: return "NULL";
        case TokenType::END_OF_FILE: return "END_OF_FILE";
        case TokenType::INVALID: return "INVALID";
        default: return "UNKNOWN";
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Error: No filename provided.\n";
        return 1;
    }

    std::string fileName = argv[1];
    std::ifstream file(fileName);
    if (!file) {
        std::cerr << "Error: Unable to open file: " << fileName << std::endl;
        return 1;
    }

    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();

    if (content.empty()) {
        std::cerr << "Error: File is empty." << std::endl;
        return 1;
    }

    std::cout << "File Content: " << content << std::endl;

    Lexer lexer(content);
    std::vector<Token*> tokens = lexer.tokenize();

    std::cout << "Tokens Generated:\n";
    for (Token* token : tokens) {
        std::cout << "Token: " << token->value << " (" << tokenTypeToString(token->type) << ")\n";
    }

    Parser parser(tokens);
    if (parser.parseJSON()) {
        std::cout << "JSON valid." << std::endl;
    } else {
        std::cout << "JSON invalid." << std::endl;
    }

    for (Token* token : tokens) delete token;

    return 0;
}
