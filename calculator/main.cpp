#include <iostream>
#include <stack>
#include <vector>
#include <sstream>
#include <cctype>
#include <cmath>  

using namespace std;

int precedence(char op) {
    if (op == '+' || op == '-') return 1;
    if (op == '*' || op == '/' || op == '%') return 2;
    if (op == '^') return 3;
    return 0;
}

bool isLeftAssociative(char op) {
    return (op != '^'); 
}

bool isOperator(char c) {
    return c == '+' || c == '-' || c == '*' || c == '/' || c == '%' || c == '^';
}

vector<string> shuntingYard(const string& expr) {
    stack<char> operatorStack;
    vector<string> output;
    string number;

    for (size_t i = 0; i < expr.length(); ++i) {
        char token = expr[i];

        if (isspace(token)) continue;

        if (isdigit(token) || token == '.' || 
            (token == '-' && (i == 0 || expr[i - 1] == '(' || isOperator(expr[i - 1])))) {
            number += token;
            continue;
        }

        if (!number.empty()) {
            output.push_back(number);
            number.clear();
        }

        if (token == '(') {
            operatorStack.push(token);
        }
        else if (token == ')') {
            while (!operatorStack.empty() && operatorStack.top() != '(') {
                output.push_back(string(1, operatorStack.top()));
                operatorStack.pop();
            }
            if (!operatorStack.empty() && operatorStack.top() == '(')
                operatorStack.pop();
        }
        else if (isOperator(token)) {
            while (!operatorStack.empty() && isOperator(operatorStack.top()) &&
                   ((isLeftAssociative(token) && precedence(token) <= precedence(operatorStack.top())) ||
                    (!isLeftAssociative(token) && precedence(token) < precedence(operatorStack.top())))) {
                output.push_back(string(1, operatorStack.top()));
                operatorStack.pop();
            }
            operatorStack.push(token);
        }
    }

    if (!number.empty()) {
        output.push_back(number);
    }

    while (!operatorStack.empty()) {
        output.push_back(string(1, operatorStack.top()));
        operatorStack.pop();
    }

    return output;
}

float calculate(const vector<string>& rpn) {
    stack<float> values;

    for (const string& token : rpn) {
        try {
            size_t pos;
            float num = stof(token, &pos);
            if (pos == token.length()) {
                values.push(num);
                continue;
            }
        } catch (...) {}

        if (token.length() == 1 && isOperator(token[0])) {
            if (values.size() < 2) {
                cerr << "Error: Invalid expression.\n";
                return 0;
            }

            float b = values.top(); values.pop();
            float a = values.top(); values.pop();

            switch (token[0]) {
                case '+': values.push(a + b); break;
                case '-': values.push(a - b); break;
                case '*': values.push(a * b); break;
                case '/': values.push(a / b); break;
                case '%': values.push(fmod(a, b)); break;
                case '^': values.push(pow(a, b)); break;
                default:
                    cerr << "Error: Unsupported operator '" << token << "'\n";
                    return 0;
            }
        } else {
            cerr << "Error: Invalid token '" << token << "'\n";
            return 0;
        }
    }

    if (values.size() != 1) {
        cerr << "Error: Too many operands.\n";
        return 0;
    }

    return values.top();
}

int main() {
    string expression;
    cout << "Enter expression:\n> ";
    getline(cin, expression);

    vector<string> rpn = shuntingYard(expression);

    cout << "Postfix (RPN): ";
    for (const string& token : rpn) cout << token << " ";
    cout << endl;

    float result = calculate(rpn);
    cout << "Result: " << result << endl;

    return 0;
}