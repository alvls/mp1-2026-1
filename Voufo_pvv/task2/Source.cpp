#include <cstdint>
#include <iostream>
#include <stdexcept>

class LongNumber {
private:
    
    int32_t high_;
    uint32_t low_;

    void setFromInt64(int64_t value) {
        high_ = static_cast<int32_t>(value >> 32);
        low_ = static_cast<uint32_t>(value & 0xFFFFFFFFULL);
    }

public:
    // Constructors
    LongNumber() : high_(0), low_(0) {}

    LongNumber(int64_t value) {
        setFromInt64(value);
    }

    LongNumber(int32_t highPart, uint32_t lowPart)
        : high_(highPart), low_(lowPart) {}

    // Convert back to normal signed 64-bit for calculations/output
    int64_t toInt64() const {
        int64_t high64 = static_cast<int64_t>(high_) << 32;
        int64_t low64 = static_cast<int64_t>(static_cast<uint64_t>(low_));
        return high64 | low64;
    }

    // Accessors for the two 32-bit storage parts
    int32_t highPart() const { return high_; }
    uint32_t lowPart() const { return low_; }

    // 5 arithmetic operations: +, -, *, /, %
    LongNumber operator+(const LongNumber& other) const {
        return LongNumber(this->toInt64() + other.toInt64());
    }

    LongNumber operator-(const LongNumber& other) const {
        return LongNumber(this->toInt64() - other.toInt64());
    }

    LongNumber operator*(const LongNumber& other) const {
        return LongNumber(this->toInt64() * other.toInt64());
    }

    LongNumber operator/(const LongNumber& other) const {
        if (other.toInt64() == 0) {
            throw std::domain_error("Division by zero is not allowed");
        }
        return LongNumber(this->toInt64() / other.toInt64());
    }

    LongNumber operator%(const LongNumber& other) const {
        if (other.toInt64() == 0) {
            throw std::domain_error("Modulo by zero is not allowed");
        }
        return LongNumber(this->toInt64() % other.toInt64());
    }

    // Console output support
    friend std::ostream& operator<<(std::ostream& os, const LongNumber& value) {
        os << value.toInt64();
        return os;
    }
};

int main() {
    LongNumber a(5000000000LL);
    LongNumber b(-12);

    std::cout << "a = " << a << '\n';
    std::cout << "b = " << b << '\n';

    std::cout << "a + b = " << (a + b) << '\n';
    std::cout << "a - b = " << (a - b) << '\n';
    std::cout << "a * b = " << (a * b) << '\n';
    std::cout << "a / b = " << (a / b) << '\n';
    std::cout << "a % b = " << (a % b) << '\n';

    // Show internal 32-bit storage parts
    std::cout << "a highPart = " << a.highPart() << ", lowPart = " << a.lowPart() << '\n';

    return 0;
}