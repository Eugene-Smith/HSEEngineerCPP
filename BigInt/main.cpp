#include <iostream>
#include "biginteger.h"

std::string exec(const BigInteger& a, const BigInteger& b, const std::string& operation) {
    if (operation == "+") return (a + b).toString();
    if (operation == "-") return (a - b).toString();
    if (operation == "*") return (a * b).toString();
    if (operation == "/") return (a / b).toString();
    if (operation == "%") return (a % b).toString();
    if (operation == "<") return a < b ? "true" : "false";
    if (operation == ">") return a > b ? "true" : "false";
    if (operation == "<=") return a <= b ? "true" : "false";
    if (operation == ">=") return a >= b ? "true" : "false";
    if (operation == "==") return a == b ? "true" : "false";
    return "Undefined operation";
}

int main() {
    auto a = BigInteger();
    auto b = BigInteger();
    std::string operation;
    std::cin >> a;
    std::cin >> operation;
    std::cin >> b;
    std::cout << exec(a, b, operation) << std::endl;
    return 0;
}
