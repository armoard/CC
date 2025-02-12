#ifndef __LEXER_H__
#define __LEXER_H__

#include <string>
#include <vector>
#include <iostream>
#include <cctype>  

enum class TokenType {
    LEFT_BRACE, RIGHT_BRACE, 
    LEFT_BRACKET, RIGHT_BRACKET, 
    COMMA, COLON, 
    STRING, NUMBER, TRUE_T, FALSE_T, NULL_T, 
    END_OF_FILE, INVALID 
};

struct Token {
    TokenType type;
    std::string value;
    Token(TokenType type, std::string value) : type(type), value(value) {}
};

class Lexer {
public:
    Lexer(std::string input) {
        source = input;
        size = input.size();
        cursor = 0;
        current = (size > 0) ? input.at(cursor) : '\0';
    }

    char advance() {
        char output = current;
        if (cursor < size - 1) {
            cursor++;
            current = source[cursor];
        } else {
            cursor = size;  
            current = '\0';
        }
        std::cout << "Advance: Cursor moved to " << cursor << " | Current Char: '" << current << "'" << std::endl; 
        return output;
    }

    char check(int offset = 0) {
        return (cursor + offset < size) ? source[cursor + offset] : '\0';
    }

    void skip() {
        while (isspace(check())) advance();
    }

    Token* stringToken() {
        advance(); // skip first "
        std::string value;
        while (check() != '"' && check() != '\0') {
            value += advance();
        }
        if (check() == '"') {
            advance();
            return new Token(TokenType::STRING, value);
        }
        return new Token(TokenType::INVALID, "Unterminated String");
    }

    Token* numberToken() {
        std::string value;
        while (isdigit(check()) || check() == '.' || check() == '-') {
            value += advance();
        }
        return new Token(TokenType::NUMBER, value);
    }

    Token* keywordToken(const std::string &expected, TokenType type) {
        std::string detected;
        for (char c : expected) {
            if (check() != c) return new Token(TokenType::INVALID, detected);
            detected += advance();
        }
        return new Token(type, expected);
    }

    std::vector<Token*> tokenize() {
        std::vector<Token*> tokens;
        skip();

        while (cursor < size) {
            char current = check();
            std::cout << "Cursor: " << cursor << " | Current Char: '" << current << "'" << std::endl;

            if (current == '{') tokens.push_back(new Token(TokenType::LEFT_BRACE, std::string(1, advance())));
            else if (current == '}') tokens.push_back(new Token(TokenType::RIGHT_BRACE, std::string(1, advance())));
            else if (current == '[') tokens.push_back(new Token(TokenType::LEFT_BRACKET, std::string(1, advance())));
            else if (current == ']') tokens.push_back(new Token(TokenType::RIGHT_BRACKET, std::string(1, advance())));
            else if (current == ',') tokens.push_back(new Token(TokenType::COMMA, std::string(1, advance())));
            else if (current == ':') tokens.push_back(new Token(TokenType::COLON, std::string(1, advance())));
            else if (current == '"') tokens.push_back(stringToken());
            else if (isdigit(current) || current == '-') tokens.push_back(numberToken());
            else if (current == 't') tokens.push_back(keywordToken("true", TokenType::TRUE_T));
            else if (current == 'f') tokens.push_back(keywordToken("false", TokenType::FALSE_T));
            else if (current == 'n') tokens.push_back(keywordToken("null", TokenType::NULL_T));
            else {
                std::cerr << "Error: Unknown Character '" << current << "' at position " << cursor << std::endl;
                tokens.push_back(new Token(TokenType::INVALID, std::string(1, current)));
                advance();  
            }
            skip();  
    }

    tokens.push_back(new Token(TokenType::END_OF_FILE, ""));
    return tokens;
}

private:
    std::string source;
    int cursor;
    int size;
    char current;
};

#endif
