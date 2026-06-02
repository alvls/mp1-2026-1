#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>

class WeightConverter {
public:
    enum class Unit {
        Kilogram,
        ApothecaryPound,
        TroyOunce,
        Pud
    };

private:
    double kilograms_;

    static double kilogramsPerUnit(Unit unit) {
        switch (unit) {
            case Unit::Kilogram:
                return 1.0;
            case Unit::ApothecaryPound:
                return 0.3732417216;  // 1 apothecary pound = 0.3732417216 kg
            case Unit::TroyOunce:
                return 0.0311034768;  // 1 troy ounce = 0.0311034768 kg
            case Unit::Pud:
                return 16.3804964;    // 1 pud = 16.3804964 kg
            default:
                throw std::invalid_argument("Unsupported weight unit");
        }
    }

public:
    // Default constructor: 0 kilograms
    WeightConverter() : kilograms_(0.0) {}

    // Constructor with initial value in kilograms
    explicit WeightConverter(double kilograms) {
        setKilograms(kilograms);
    }

    // 1) Set current weight in kilograms
    void setKilograms(double kilograms) {
        if (kilograms < 0.0) {
            throw std::invalid_argument("Weight cannot be negative");
        }
        kilograms_ = kilograms;
    }

    // 2) Get current weight in kilograms
    double getKilograms() const {
        return kilograms_;
    }

    // 3) Get current weight in selected unit
    double getInUnit(Unit unit) const {
        return kilograms_ / kilogramsPerUnit(unit);
    }

    static std::string unitToString(Unit unit) {
        switch (unit) {
            case Unit::Kilogram:
                return "kilogram";
            case Unit::ApothecaryPound:
                return "apothecary pound";
            case Unit::TroyOunce:
                return "troy ounce";
            case Unit::Pud:
                return "pud";
            default:
                return "unknown";
        }
    }

    // Console output support
    friend std::ostream& operator<<(std::ostream& os, const WeightConverter& value) {
        os << std::fixed << std::setprecision(4) << value.kilograms_ << " kg";
        return os;
    }
};

int main() {
    WeightConverter weight(5.0);

    std::cout << "Current weight: " << weight << '\n';
    std::cout << "In kilograms: " << weight.getKilograms() << '\n';

    std::cout << "In apothecary pounds: "
              << weight.getInUnit(WeightConverter::Unit::ApothecaryPound) << '\n';
    std::cout << "In troy ounces: " << weight.getInUnit(WeightConverter::Unit::TroyOunce) << '\n';
    std::cout << "In pud: " << weight.getInUnit(WeightConverter::Unit::Pud) << '\n';

    weight.setKilograms(12.75);
    std::cout << "After setKilograms(12.75): " << weight << '\n';

    return 0;
}