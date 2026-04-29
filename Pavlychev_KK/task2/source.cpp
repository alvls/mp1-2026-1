#include <iostream>
#include <cmath>
#include <locale.h>

class Polinom {
private:
    int stepen;
    double koefficienty[13];

public:
    Polinom() {
        stepen = 0;
        for (int i = 0; i <= 12; ++i) {
            koefficienty[i] = 0.0;
        }
    }

    explicit Polinom(int deg) {
        if (deg < 0 || deg > 12) {
            std::cerr << "Ошибка: степень должна быть от 0 до 12" << std::endl;
            stepen = 0;
        }
        else {
            stepen = deg;
        }

        for (int i = 0; i <= 12; ++i) {
            koefficienty[i] = 0.0;
        }
    }

    Polinom(const Polinom& other) {
        stepen = other.stepen;
        for (int i = 0; i <= 12; ++i) {
            koefficienty[i] = other.koefficienty[i];
        }
    }

    Polinom& operator=(const Polinom& other) {
        if (this != &other) {
            stepen = other.stepen;
            for (int i = 0; i <= 12; ++i) {
                koefficienty[i] = other.koefficienty[i];
            }
        }
        return *this;
    }

    ~Polinom() {}

    void setStepen(int deg) {
        if (deg < 0 || deg > 12) {
            std::cerr << "Ошибка: степень должна быть от 0 до 12" << std::endl;
            return;
        }
        stepen = deg;
    }

    void setKoefficient(int step, double znachenie) {
        if (step < 0 || step > 12) {
            std::cerr << "Ошибка: степень монома должна быть от 0 до 12" << std::endl;
            return;
        }
        koefficienty[step] = znachenie;

        if (step > stepen && znachenie != 0) {
            stepen = step;
        }
        while (stepen > 0 && koefficienty[stepen] == 0) {
            stepen--;
        }
    }

    int getStepen() const {
        return stepen;
    }

    double getKoefficient(int step) const {
        if (step < 0 || step > 12) {
            std::cerr << "Ошибка: степень вне допустимого диапазона" << std::endl;
            return 0.0;
        }
        return koefficienty[step];
    }

    double vychislit(double x) const {
        double rezultat = 0.0;
        for (int i = 0; i <= stepen; ++i) {
            rezultat += koefficienty[i] * std::pow(x, i);
        }
        return rezultat;
    }

    Polinom proizvodnaya() const {
        if (stepen == 0) {
            Polinom proizv(0);
            proizv.setKoefficient(0, 0.0);
            return proizv;
        }

        Polinom proizv(stepen - 1);
        for (int i = 1; i <= stepen; ++i) {
            proizv.setKoefficient(i - 1, koefficienty[i] * i);
        }
        return proizv;
    }

    void print() const {
        bool pervyy = true;

        for (int i = stepen; i >= 0; --i) {
            if (koefficienty[i] == 0.0) continue;

            double koef = koefficienty[i];

            if (pervyy) {
                if (koef < 0) std::cout << "-";
                pervyy = false;
            }
            else {
                if (koef > 0) std::cout << " + ";
                else std::cout << " - ";
            }

            double absKoef = std::abs(koef);

            if (i == 0) {
                std::cout << absKoef;
            }
            else if (i == 1) {
                if (absKoef == 1.0) std::cout << "x";
                else std::cout << absKoef << "x";
            }
            else {
                if (absKoef == 1.0) std::cout << "x^" << i;
                else std::cout << absKoef << "x^" << i;
            }
        }

        if (pervyy) {
            std::cout << "0";
        }
        std::cout << std::endl;
    }
};

int main() {
    setlocale(LC_ALL, "Ru_ru.UTF8");
    Polinom p(3);

    p.setKoefficient(3, 2.0);
    p.setKoefficient(2, 0.0);
    p.setKoefficient(1, -3.0);
    p.setKoefficient(0, 5.0);

    std::cout << "Полином: ";
    p.print();

    std::cout << "Степень: " << p.getStepen() << std::endl;
    std::cout << "Коэффициент при x^2: " << p.getKoefficient(2) << std::endl;
    std::cout << "Значение при x=2: " << p.vychislit(2) << std::endl;

    Polinom proizv = p.proizvodnaya();
    std::cout << "Производная: ";
    proizv.print();

    Polinom p2 = p;
    std::cout << "Копия: ";
    p2.print();

    Polinom p3;
    p3 = p;
    std::cout << "После присваивания: ";
    p3.print();
    system("pause");
    return 0;
}