#include <iostream>
#include <numeric>
#include <stdexcept>

class Rational {
private:
    int numerator;
    int denominator;

    void defined() {
        if (denominator== 0) {
            throw std::invalid_argument("Denominator cannot be zero");
        }

        if (denominator< 0) {
            numerator= -numerator;
            denominator = -denominator;
        }

        int d = std::gcd(numerator, denominator);
        numerator /= d;
        denominator /= d;
    }

public:
    Rational(int numerator = 0, int denominator = 1)
        : numerator(numerator), denominator(denominator) {
        defined();
    }

    int numerator() const { return numerator; }
    int denominator() const { return denominator; }

    Rational operator+(const Rational& other) const {
        int num = numerator * other.denominator + other.numerator * denominator;
        int den = denominator * other.denominator;
        return Rational(num, den);
    }

    Rational operator-(const Rational& other) const {
        int num = numerator * other.denominator- other.numerator * denominator;
        int den = denominator * other.denominator;
        return Rational(num, den);
    }

    Rational operator*(const Rational& other) const {
        int num = numerator * other.numerator;
        int den = denominator * other.denominator;
        return Rational(num, den);
    }

    Rational operator/(const Rational& other) const {
        if (other.numerator == 0) {
            throw std::domain_error("Division by zero is not allowed");
        }
        int num = numerator * other.denominator;
        int den = denominator* other.numerator;
        return Rational(num, den);
    }

    friend std::ostream& operator<<(std::ostream& os, const Rational& r) {
        if (r.denominator == 1) {
            os << r.numerator;
        } else {
            os << r.numerator << '/' << r.denominator;
        }
        return os;
    }
};

int main() {
    Rational a(1, 2);
    Rational b(3, 4);

    std::cout << "a = " << a << '\n';
    std::cout << "b = " << b << '\n';
    std::cout << "a + b = " << (a + b) << '\n';
    std::cout << "a - b = " << (a - b) << '\n';
    std::cout << "a * b = " << (a * b) << '\n';
    std::cout << "a / b = " << (a / b) << '\n';

    Rational c(8, 12);
    std::cout << "c = " << c << " (auto-reduced from 8/12)\n";

    return 0;
}