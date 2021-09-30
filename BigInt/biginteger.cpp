#include "biginteger.h"

BigInteger::BigInteger(int64_t num) {
    sign = Sign(num);
    while (num) {
        value.push_back(num % base);
        num /= base;
    }
}

std::string BigInteger::toString() const {
    std::string str;
    for (auto digit = value.begin(); digit != value.end(); ++digit) {
        auto str_digit = std::to_string(abs(*digit));
        if (digit + 1 != value.end()) {
            while (str_digit.size() != len_digit) {
                str_digit.insert(0, "0");
            }
        }
        str.insert(0, str_digit);
    }
    switch(sign) {
        case 0:
            return "0";
        case -1:
            str.insert(0, "-");
            break;
    }
    return str;
}

std::ostream& operator<<(std::ostream& os, const BigInteger& obj) {
    os << obj.toString();
    return os;
}

std::istream& operator>>(std::istream& is, BigInteger& obj) {
    std::string str;
    if (is >> str) {
        obj.value.clear();
        uint8_t is_signed = 0;
        switch (str[0]) {
            case '0':
                obj.sign = 0;
                return is;
            case '-':
                obj.sign = -1;
                ++is_signed;
                break;
            default:
                obj.sign = 1;
                break;
        }
        auto abs_size = (str.size() - is_signed); // no sign
        auto num_digits = abs_size / BigInteger::len_digit + (abs_size % BigInteger::len_digit != 0);
        obj.value.resize(num_digits);
        int64_t digit = 0;
        for (size_t i = 1; i <= num_digits; ++i) {
            if (i != num_digits) {
                digit = std::stoll(str.substr(str.size() - i * BigInteger::len_digit, BigInteger::len_digit)) * obj.sign;
            } else {
                digit = std::stoll(str.substr(is_signed, abs_size - (i - 1) * BigInteger::len_digit)) * obj.sign;
            }
            obj.value[i - 1] = digit;
        }
        obj.PopZeros();
    }
    return is;
}

bool operator==(const BigInteger& lhs, const BigInteger& rhs) {
    return lhs.value == rhs.value;
}

bool operator!=(const BigInteger& lhs, const BigInteger& rhs) {
    return lhs.value != rhs.value;
}

bool operator<(const BigInteger& lhs, const BigInteger& rhs) {
    if (lhs.sign == rhs.sign) {
        if (lhs.value.size() == rhs.value.size()) {
            for (size_t i = lhs.value.size(); i-- > 0;) {
                if (lhs.value[i] != rhs.value[i]) {
                    return lhs.value[i] < rhs.value[i];
                }
            }
            return false;
        } else {
            return lhs.sign == 1 ? lhs.value.size() < rhs.value.size() : lhs.value.size() > rhs.value.size();
        }
    } else {
        return lhs.sign < rhs.sign;
    }
}

bool operator>(const BigInteger& lhs, const BigInteger& rhs) {
    return rhs < lhs;
}

bool operator<=(const BigInteger& lhs, const BigInteger& rhs) {
    return (lhs == rhs) || (lhs < rhs);
}

bool operator>=(const BigInteger& lhs, const BigInteger& rhs) {
    return rhs <= lhs;
}

BigInteger operator+(BigInteger lhs, const BigInteger& rhs) {
    lhs += rhs;
    return lhs;
}

BigInteger operator-(BigInteger lhs, const BigInteger& rhs) {
    lhs -= rhs;
    return lhs;
}

BigInteger operator*(BigInteger lhs, const BigInteger& rhs) {
    lhs *= rhs;
    return lhs;
}

BigInteger operator/(BigInteger lhs, const BigInteger& rhs) {
    lhs /= rhs;
    return lhs;
}

BigInteger operator%(BigInteger lhs, const BigInteger& rhs) {
    lhs %= rhs;
    return lhs;
}

BigInteger operator+(const BigInteger& rhs) {
    return rhs;
}

BigInteger operator-(const BigInteger& rhs) {
    BigInteger neg;
    neg.sign = -rhs.sign;
    for (auto digit: rhs.value) {
        neg.value.push_back(-digit);
    }
    return neg;
}

BigInteger& BigInteger::operator+=(const BigInteger& rhs) {
    value.resize(std::max(value.size(), rhs.value.size()));
    for (size_t i = 0; i < rhs.value.size(); ++i) {
        value[i] += rhs.value[i];
    }
    PopZeros();
    if (!value.empty()) {
        sign = Sign(value.back());
    }
    Normalize();
    return *this;
}

BigInteger &BigInteger::operator-=(const BigInteger &rhs) {
    value.resize(std::max(value.size(), rhs.value.size()));
    for (size_t i = 0; i < rhs.value.size(); ++i) {
        value[i] -= rhs.value[i];
    }
    PopZeros();
    if (!value.empty()) {
        sign = Sign(value.back());
    }
    Normalize();
    return *this;
}

BigInteger& BigInteger::operator*=(const BigInteger& rhs) {
    BigInteger mul;
    if (value.size() + rhs.value.size() <= min_len) {
        mul = BigInteger(ToInt() * rhs.ToInt());
    } else {
        size_t half = (std::max(value.size(), rhs.value.size()) + 1) / 2;

        auto left_halves = Split(*this, half);
        auto right_halves = Split(rhs, half);

        auto lhs_sum = left_halves.first + left_halves.second;
        auto rhs_sum = right_halves.first + right_halves.second;

        auto mul_sum = lhs_sum * rhs_sum;
        auto mul_first = left_halves.first * right_halves.first;
        auto mul_second = left_halves.second * right_halves.second;
        auto mul_middle = mul_sum - mul_first - mul_second;

        mul.value.insert(mul.value.end(), mul_first.value.begin(), mul_first.value.end());
        mul.value.insert(mul.value.end(), 2 * half - mul_first.value.size(), 0);
        mul.value.insert(mul.value.end(), mul_second.value.begin(), mul_second.value.end());

        if (mul_middle.value.size() + half > mul.value.size()) {
            mul.value.resize(mul_middle.value.size() + half);
        }
        for (size_t i = 0; i < mul_middle.value.size(); ++i) {
            mul.value[i + half] += mul_middle.value[i];
        }
    }
    mul.sign = sign * rhs.sign;
    mul.Normalize();
    *this = mul;
    return *this;
}

BigInteger& BigInteger::operator/=(const BigInteger &rhs) {
    BigInteger div;
    div.sign = sign / rhs.sign;
    div.value.resize(value.size());
    BigInteger rem;
    auto abs_dividend = this->Abs();
    auto abs_divisor = rhs.Abs();
    for (size_t i = abs_dividend.value.size(); i-- > 0;) {
        rem.value.insert(rem.value.begin(), abs_dividend.value[i]);
        rem.sign = 1;
        rem.PopZeros();
        if (rem >= abs_divisor) {
            if (rem.value.size() == 1) {
                div.value[i] += (rem.value[0] / abs_divisor.value[0]) * div.sign;
                rem.value[0] %= abs_divisor.value[0];
            } else {
                while (rem >= abs_divisor)  {
                    rem -= abs_divisor;
                    div.value[i] += div.sign;
                }
            }
        }
    }
    div.PopZeros();
    *this = div;
    return *this;
}

BigInteger &BigInteger::operator%=(const BigInteger &rhs) {
    BigInteger rem;
    auto abs_dividend = this->Abs();
    auto abs_divisor = rhs.Abs();
    for (size_t i = abs_dividend.value.size(); i-- > 0;) {
        rem.value.insert(rem.value.begin(), abs_dividend.value[i]);
        rem.sign = 1;
        rem.PopZeros();
        if (rem >= abs_divisor) {
            if (rem.value.size() == 1) {
                rem.value[0] %= abs_divisor.value[0];
            } else {
                while (rem >= abs_divisor)  {
                    rem -= abs_divisor;
                }
            }
        }
    }
    rem.PopZeros();
    if (sign == -1) {
        *this = -rem;
    } else {
        *this = rem;
    }
    return *this;
}

BigInteger::operator bool() const {
    return sign;
}

void BigInteger::Normalize() {
    int64_t carry = 0;
    for (auto& digit: value) {
        digit  += carry;
        if (digit * sign < 0) {
            carry = (digit + sign) / base - sign;
            digit -= BigInteger::base * carry;
        } else if (abs(digit) >= BigInteger::base) {
            carry = digit / base;
            digit -= BigInteger::base * carry;
        } else {
            carry = 0;
        }
    }
    if (carry) {
        value.push_back(carry);
    }
    PopZeros();
}

void BigInteger::PopZeros() {
    while (!(value.empty() || value.back())) {
        value.pop_back();
    }
    if (value.empty()) {
        sign = 0;
    }
}

std::pair<BigInteger, BigInteger> BigInteger::Split(const BigInteger& integer, size_t half) {
    BigInteger left;
    BigInteger right;
    left.sign = right.sign = integer.sign;
    left.value.insert(left.value.end(), integer.value.begin(), integer.value.begin() + std::min(half, integer.value.size()));
    right.value.insert(right.value.end(), integer.value.begin() + left.value.size(), integer.value.end());
    left.PopZeros();
    right.PopZeros();
    return std::make_pair(left, right);
}

BigInteger &BigInteger::operator++() {
    operator+=(1);
    return *this;
}

BigInteger BigInteger::operator++(int) {
    BigInteger tmp(*this);
    operator++();
    return tmp;
}
BigInteger &BigInteger::operator--() {
    operator-=(1);
    return *this;
}

BigInteger BigInteger::operator--(int) {
    BigInteger tmp(*this);
    operator--();
    return tmp;
}

BigInteger BigInteger::Abs() const {
    if (sign == -1) {
        return -*this;
    }
    return *this;
}

int64_t BigInteger::ToInt() const {
    int64_t pow_base = 1;
    int64_t int_value = 0;
    for (auto digit: value) {
        int_value += digit * pow_base;
        pow_base *= base;
    }
    return int_value;
}

int8_t BigInteger::Sign(int64_t num) {
    if (num) {
        return num > 0 ? 1 : -1;
    } else {
        return 0;
    }
}


