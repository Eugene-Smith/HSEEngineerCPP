#pragma once
#include <string>
#include <queue>
#include <stack>
#include <sstream>
#include <unordered_map>

template <typename T>
T Exec(const T& a, const T& b, const char operation) {
    switch (operation) {
        case '+':
            return a + b;
        case '-':
            return a - b;
        case '*':
            return a * b;
        case '/':
            return a / b;
        case '%':
            return a % b;
    }
}

template <typename T>
T Exec(const T& a, const char operation) {
    switch (operation) {
        case '+':
            return +a;
        case '-':
            return -a;
    }
}

template <typename T>
void Exec(std::stack<T>& output_stack, const char operation) {
    auto b = output_stack.top();
    output_stack.pop();
    if (output_stack.empty()) {
        output_stack.push(Exec(b, operation));
    } else {
        auto a = output_stack.top();
        output_stack.pop();
        output_stack.push(Exec(a, b, operation));
    }
}

// Shunting-yard algorithm
template <typename T>
T Calculate(const std::string& expression) {
    std::stack<char> operator_stack;
    std::stack<T> output_stack;
    std::unordered_map<char, size_t> operator_precedence = {
            {'(', '1'},
            {')', '1'},
            {'*', '3'},
            {'/', '3'},
            {'%', '3'},
            {'+', '4'},
            {'-', '4'}
    };
    for (size_t i = 0; i < expression.size(); ++i) {
        if (std::isdigit(expression[i])) {
            // parse number
            std::stringstream ss;
            do {
                ss << expression[i];
                ++i;
            } while (std::isdigit(expression[i]));
            T number;
            ss >> number;
            output_stack.push(number);
            --i;
        } else if (expression[i] == '(') {
            operator_stack.push(expression[i]);
        } else if (expression[i] == ')') {
            while (operator_stack.top() != '(') {
                Exec(output_stack, operator_stack.top());
                operator_stack.pop();
            }
            operator_stack.pop();
        } else if (operator_precedence.contains(expression[i])) {
            char op = expression[i];
            while (
                   !operator_stack.empty()
                   && operator_stack.top() != '('
                   && operator_precedence[operator_stack.top()] <= operator_precedence[op]
            ) {
                Exec(output_stack, operator_stack.top());
                operator_stack.pop();
            }
            operator_stack.push(op);
        }
    }
    while (!operator_stack.empty()) {
        Exec(output_stack, operator_stack.top());
        operator_stack.pop();
    }
    return output_stack.top();
}
