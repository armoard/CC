#ifndef __PARSER_H__
#define __PARSER_H__
#include <vector>
#include <iostream>
#include <string>
#include "lexer.h"

class Parser {
public:
    Parser(std::vector<Token*> input) {
        tokens = input;
        current = 0;
    }
    Token* check() {
        if (current < tokens.size()) {
             Token* output = tokens[current];
             return output;
        }
        return nullptr;
    }

    Token* advance() {
        if (current < tokens.size()) {  
            Token* output = tokens[current];
            current++;
            return output;
        }
        return nullptr;
    }
    bool match(TokenType type) {
        if (check() && check()->type == type) {
            advance();
            return true;
        }
        return false;
    }
    bool parseJSON() {
        if (match(TokenType::LEFT_BRACE)) {
            return parseObject();
        } 
        else if (match(TokenType::LEFT_BRACKET)) {
            return parseArray();
        } 
        else {
            return error("Expected '{' or '[' at the start of the JSON.");
        }
    }
    bool parseObject() {
        if (match(TokenType::RIGHT_BRACE)) return true;  // `{}` empty

        do {
            if (!parsePair()) return false;
        } while (match(TokenType::COMMA));
        
        if (!match(TokenType::RIGHT_BRACE)) return error("Expected '}' at the end of the object.");
        
        return true;
    }

    bool parsePair() {
        if (!match(TokenType::STRING)) return error("Expected a STRING as key.");
        if (!match(TokenType::COLON)) return error("Expected ':' after key.");
        return parseValue();
    }
    bool parseValue() {
        if (match(TokenType::STRING) || match(TokenType::NUMBER) || 
            match(TokenType::TRUE_T) || match(TokenType::FALSE_T) || match(TokenType::NULL_T)) {
            return true;
        } 
        else if (match(TokenType::LEFT_BRACE)) {
            return parseObject();
        } 
        else if (match(TokenType::LEFT_BRACKET)) {
            return parseArray();
        } 
        else {
            return error("Unexpected value.");
        }
    }
    bool parseArray() {
        if (match(TokenType::RIGHT_BRACKET)) return true;  // `[]` empty

        do {
            if (!parseValue()) return false;
        } while (match(TokenType::COMMA));  

        if (!match(TokenType::RIGHT_BRACKET)) return error("Expected ']' at the end of the array.");
        
        return true;
    }
    bool error(const std::string& message) {
    std::cerr << "Error: " << message << " at token " 
              << (check() ? check()->value : "EOF")
              << " at position " << current << std::endl;
    return false;  
    }

private:
    int current;  
    std::vector<Token*> tokens;

};
#endif