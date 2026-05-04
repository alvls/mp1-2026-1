#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>

class TemperatureConverter {
public:
    enum class Unit {
        Celsius,
        Fahrenheit,
        Kelvin,
        Rankine
    };

private:
    double celsius_;

public:
    // Default constructor: 0°C
    TemperatureConverter() : celsius_(0.0) {}

    // Constructor with initial Celsius value
    explicit TemperatureConverter(double celsius) {
        setCelsius(celsius);
    }

    // 1) Set current temperature in Celsius
    void setCelsius(double celsius) {
        // Physical lower bound: absolute zero = -273.15°C
        if (celsius < -273.15) {
            throw std::invalid_argument("Temperature cannot be below absolute zero (-273.15°C)");
        }
        celsius_ = celsius;
    }

    // 2) Get current temperature in Celsius
    double getCelsius() const {
        return celsius_;
    }

    // 3) Get current temperature in selected unit
    double getInUnit(Unit unit) const {
        switch (unit) {
            case Unit::Celsius:
                return celsius_;
            case Unit::Fahrenheit:
                return celsius_ * 9.0 / 5.0 + 32.0;
            case Unit::Kelvin:
                return celsius_ + 273.15;
            case Unit::Rankine:
                return (celsius_ + 273.15) * 9.0 / 5.0;
            default:
                throw std::invalid_argument("Unsupported temperature unit");
        }
    }

    static std::string unitToString(Unit unit) {
        switch (unit) {
            case Unit::Celsius:
                return "Celsius";
            case Unit::Fahrenheit:
                return "Fahrenheit";
            case Unit::Kelvin:
                return "Kelvin";
            case Unit::Rankine:
                return "Rankine";
            default:
                return "unknown";
        }
    }

    // Console output support
    friend std::ostream& operator<<(std::ostream& os, const TemperatureConverter& value) {
        os << std::fixed << std::setprecision(2) << value.celsius_ << " °C";
        return os;
    }
};

int main() {
    TemperatureConverter temp(25.0);

    std::cout << "Current temperature: " << temp << '\n';
    std::cout << "In Celsius: " << temp.getCelsius() << " °C\n";
    std::cout << "In Fahrenheit: " << temp.getInUnit(TemperatureConverter::Unit::Fahrenheit) << " °F\n";
    std::cout << "In Kelvin: " << temp.getInUnit(TemperatureConverter::Unit::Kelvin) << " K\n";
    std::cout << "In Rankine: " << temp.getInUnit(TemperatureConverter::Unit::Rankine) << " °R\n";

    temp.setCelsius(-10.5);
    std::cout << "After setCelsius(-10.5): " << temp << '\n';

    return 0;
}