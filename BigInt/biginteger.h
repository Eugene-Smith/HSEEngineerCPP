#include <iostream>
#include <vector>
#include <string>

class BigInteger {
public:
    BigInteger() = default;
    BigInteger(const BigInteger& other) = default;
    BigInteger& operator=(const BigInteger& other) = default;
    BigInteger(BigInteger&& other) = default;
    BigInteger& operator=(BigInteger&& other) = default;
    ~BigInteger() = default;

    BigInteger(int64_t num);

    std::string toString() const;
    explicit operator bool() const;

    friend std::ostream& operator<<(std::ostream& os, const BigInteger& obj);
    friend std::istream& operator>>(std::istream& is, BigInteger& obj);

    friend bool operator==(const BigInteger& lhs, const BigInteger& rhs);
    friend bool operator!=(const BigInteger& lhs, const BigInteger& rhs);
    friend bool operator<(const BigInteger& lhs, const BigInteger& rhs);
    friend bool operator>(const BigInteger& lhs, const BigInteger& rhs);
    friend bool operator<=(const BigInteger& lhs, const BigInteger& rhs);
    friend bool operator>=(const BigInteger& lhs, const BigInteger& rhs);

    friend BigInteger operator+(BigInteger lhs, const BigInteger& rhs);
    friend BigInteger operator-(BigInteger lhs, const BigInteger& rhs);

    BigInteger& operator++();
    BigInteger operator++(int);
    BigInteger& operator--();
    BigInteger operator--(int);

    friend BigInteger operator*(BigInteger lhs, const BigInteger& rhs);
    friend BigInteger operator/(BigInteger lhs, const BigInteger& rhs);
    friend BigInteger operator%(BigInteger lhs, const BigInteger& rhs);

    friend BigInteger operator+(const BigInteger& rhs);
    friend BigInteger operator-(const BigInteger& rhs);

    BigInteger& operator+=(const BigInteger& rhs);
    BigInteger& operator-=(const BigInteger& rhs);
    BigInteger& operator*=(const BigInteger& rhs);
    BigInteger& operator/=(const BigInteger& rhs);
    BigInteger& operator%=(const BigInteger& rhs);

    BigInteger Abs() const;
    int64_t ToInt() const;

private:
    void Normalize();
    void PopZeros();
    int8_t Sign(int64_t num);
    std::pair<BigInteger, BigInteger> Split(const BigInteger& integer, size_t half);

    int8_t sign = 0;
    std::vector<int64_t> value;
    const static int64_t base = 100;
    const static uint8_t len_digit = 2;
    const static uint8_t min_len = 2;
};

