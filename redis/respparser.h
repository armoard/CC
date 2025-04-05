#pragma once
#include <string>
#include <vector>
#include <memory>
#include <variant>
#include <stdexcept>

struct RESPError {
    std::string message;
};

enum class RESPStringType {
    SIMPLE,
    BULK
};

struct RESPString {
    std::string value;
    RESPStringType type;
};

struct RESPArray;  

using RESPValue = std::variant<
    RESPString,
    int,
    std::shared_ptr<RESPArray>,
    RESPError,
    std::monostate
>;

struct RESPArray {
    std::vector<RESPValue> values;
};

class RESPParser {
public:
    RESPValue deserialize(const std::string& input) {
        size_t pos = 0;
        return parseRESP(input, pos);
    }

    std::string serialize(const RESPValue& input) {
        return serializeRESP(input);
    }

private:
    RESPValue parseRESP(const std::string& input, size_t& pos) {
        if (pos >= input.length()) throw std::runtime_error("Unexpected end of input");

        char prefix = input[pos];
        switch (prefix) {
            case '+': return parseRESPString(input, pos);
            case '-': return parseRESPError(input, pos);
            case ':': return parseRESPInt(input, pos);
            case '$': return parseRESPBulkString(input, pos);
            case '*': return parseRESPArray(input, pos);
            default:
                throw std::runtime_error(std::string("Invalid RESP type: ") + prefix);
        }
    }

    std::string readLine(const std::string& input, size_t& pos) {
        std::string line;

        while (pos < input.size() && input[pos] != '\r') {
            line += input[pos++];
        }

        if (pos + 1 >= input.size() || input[pos] != '\r' || input[pos + 1] != '\n') {
            throw std::runtime_error("Invalid RESP line ending");
        }

        pos += 2;
        return line;
    }

    RESPValue parseRESPString(const std::string& input, size_t& pos) {
        pos++;
        std::string content = readLine(input, pos);
        return RESPString{content, RESPStringType::SIMPLE};
    }

    RESPValue parseRESPError(const std::string& input, size_t& pos) {
        pos++;
        std::string message = readLine(input, pos);
        return RESPError{message};
    }

    RESPValue parseRESPInt(const std::string& input, size_t& pos) {
        pos++;
        std::string numStr = readLine(input, pos);
        return std::stoi(numStr);
    }

    RESPValue parseRESPBulkString(const std::string& input, size_t& pos) {
        pos++;
        std::string lengthStr = readLine(input, pos);
        int length = std::stoi(lengthStr);

        if (length == -1) {
            return std::monostate{};
        }

        std::string content = input.substr(pos, length);
        pos += length;
        pos += 2;  // Skip CRLF

        return RESPString{content, RESPStringType::BULK};
    }

    RESPValue parseRESPArray(const std::string& input, size_t& pos) {
        pos++;
        std::string lengthStr = readLine(input, pos);
        int length = std::stoi(lengthStr);

        if (length == -1) {
            return std::monostate{};
        }

        std::shared_ptr<RESPArray> content = std::make_shared<RESPArray>();
        for (int i = 0; i < length; i++) {
            content->values.push_back(parseRESP(input, pos));
        }

        return content;
    }

    std::string serializeRESP(const RESPValue& value) {
        return std::visit([this](auto&& val) -> std::string {
            using T = std::decay_t<decltype(val)>;

            if constexpr (std::is_same_v<T, RESPString>) {
                return serializeSTRING(val);
            } else if constexpr (std::is_same_v<T, int>) {
                return serializeINT(val);
            } else if constexpr (std::is_same_v<T, RESPError>) {
                return serializeERROR(val);
            } else if constexpr (std::is_same_v<T, std::monostate>) {
                return "$-1\r\n";
            } else if constexpr (std::is_same_v<T, std::shared_ptr<RESPArray>>) {
                return serializeARRAY(*val);
            } else {
                throw std::runtime_error("Unknown RESP type");
            }
        }, value);
    }

    std::string serializeSTRING(const RESPString& s) {
        if (s.type == RESPStringType::SIMPLE) {
            return "+" + s.value + "\r\n";
        } else {
            return "$" + std::to_string(s.value.length()) + "\r\n" + s.value + "\r\n";
        }
    }

    std::string serializeINT(int value) {
        return ":" + std::to_string(value) + "\r\n";
    }

    std::string serializeERROR(const RESPError& e) {
        return "-" + e.message + "\r\n";
    }

    std::string serializeARRAY(const RESPArray& array) {
        std::string output;
        output += "*";
        output += std::to_string(array.values.size());
        output += "\r\n";

        for (const auto& ptr : array.values) {
            const RESPValue& item = ptr;
            output += serializeRESP(item);
        }

        return output;
    }
};
