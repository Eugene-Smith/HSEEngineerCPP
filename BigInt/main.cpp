#include <iostream>
#include "biginteger.h"
#include "tools.h"

int main() {
    std::string expression;
    getline(std::cin, expression);
    std::cout <<  Calculate<BigInteger>(expression) << std::endl;
    return 0;
}
