#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>

class LengthConverter {
public:
    enum class Unit {
        Meter,
        Foot,
        Yard,
        Arshin,
        Sazhen
    };

private:
    double meters_;

    static double metersPerUnit(Unit unit) {
        switch (unit) {
            case Unit::Meter:
                return 1.0;
            case Unit::Foot:
                return 0.3048;      // 1 foot = 0.3048 m
            case Unit::Yard:
                return 0.9144;      // 1 yard = 0.9144 m
            case Unit::Arshin:
                return 0.7112;      // 1 arshin = 0.7112 m
            case Unit::Sazhen:
                return 2.1336;      // 1 sazhen = 2.1336 m
            default:
                throw std::invalid_argument("Unsupported length unit");
        }
    }

public:
    // Default constructor: 0 meters
    LengthConverter() : meters_(0.0) {}

    // Constructor with initial value in meters
    explicit LengthConverter(double meters) {
        setMeters(meters);
    }

    // 1) Set current length in meters
    void setMeters(double meters) {
        if (meters < 0.0) {
            throw std::invalid_argument("Length cannot be negative");
        }
        meters_ = meters;
    }

    // 2) Get current length in meters
    double getMeters() const {
        return meters_;
    }

    // 3) Get current length in a selected unit
    double getInUnit(Unit unit) const {
        return meters_ / metersPerUnit(unit);
    }

    static std::string unitToString(Unit unit) {
        switch (unit) {
            case Unit::Meter:
                return "meter";
            case Unit::Foot:
                return "foot";
            case Unit::Yard:
                return "yard";
            case Unit::Arshin:
                return "arshin";
            case Unit::Sazhen:
                return "sazhen";
            default:
                return "unknown";
        }
    }

    // Console output support
    friend std::ostream& operator<<(std::ostream& os, const LengthConverter& value) {
        os << std::fixed << std::setprecision(4) << value.meters_ << " m";
        return os;
    }
};

int main() {
    LengthConverter length(10.0);

    std::cout << "Current length: " << length << '\n';
    std::cout << "In meters: " << length.getMeters() << '\n';

    std::cout << "In foot: " << length.getInUnit(LengthConverter::Unit::Foot) << '\n';
    std::cout << "In yard: " << length.getInUnit(LengthConverter::Unit::Yard) << '\n';
    std::cout << "In arshin: " << length.getInUnit(LengthConverter::Unit::Arshin) << '\n';
    std::cout << "In sazhen: " << length.getInUnit(LengthConverter::Unit::Sazhen) << '\n';

    length.setMeters(25.5);
    std::cout << "After setMeters(25.5): " << length << '\n';

    return 0;
}