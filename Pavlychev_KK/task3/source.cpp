#include <iostream>
#include <cmath>
#include <locale.h>
#include <cstdlib>

class IntegralCalculator {
private:
    double (*func)(double);
    double a;
    double b;
    int n;
    int method;

public:
    IntegralCalculator()
        : func(NULL), a(0.0), b(1.0), n(1000), method(3) {
        func = defaultFunc;
    }

    IntegralCalculator(double (*f)(double), double low, double high, int seg = 1000)
        : func(f), a(low), b(high), n(seg), method(3) {
        if (seg <= 0) {
            std::cout << "Ошибка: n > 0\n";
            n = 1000;
        }
        if (low >= high) {
            std::cout << "Ошибка: a < b\n";
            a = 0.0;
            b = 1.0;
        }
    }

    IntegralCalculator(const IntegralCalculator& other)
        : func(other.func), a(other.a), b(other.b), n(other.n), method(other.method) {}

    ~IntegralCalculator() {}

    IntegralCalculator& operator=(const IntegralCalculator& other) {
        if (this != &other) {
            func = other.func;
            a = other.a;
            b = other.b;
            n = other.n;
            method = other.method;
        }
        return *this;
    }

    void setFunc(double (*f)(double)) {
        if (!f) {
            std::cout << "Ошибка: функция не задана\n";
        }
        else {
            func = f;
        }
    }

    void setDefaultFunc() {
        func = defaultFunc;
    }

    static double defaultFunc(double x) {
        return x;
    }

    void setLimits(double low, double high) {
        if (low >= high) {
            std::cout << "Ошибка: a < b\n";
        }
        else {
            a = low;
            b = high;
        }
    }

    void getLimits(double& low, double& high) const {
        low = a;
        high = b;
    }

    double getLow() const {
        return a;
    }

    double getHigh() const {
        return b;
    }

    void setSegments(int seg) {
        if (seg <= 0) {
            std::cout << "Ошибка: n > 0\n";
        }
        else {
            n = seg;
        }
    }

    int getSegments() const {
        return n;
    }

    void setMethod(int m) {
        if (m >= 1 && m <= 3) {
            method = m;
        }
        else {
            std::cout << "Ошибка: метод 1,2 или 3\n";
        }
    }

    int getMethod() const {
        return method;
    }

    double calc() const {
        if (!func) {
            std::cout << "Ошибка: функция не задана\n";
            return 0.0;
        }

        double h = (b - a) / n;
        double sum = 0.0;

        if (method == 1) {
            for (int i = 0; i < n; i++) {
                double x = a + i * h;
                sum += func(x);
            }
        }
        else if (method == 2) {
            for (int i = 1; i <= n; i++) {
                double x = a + i * h;
                sum += func(x);
            }
        }
        else if (method == 3) {
            for (int i = 0; i < n; i++) {
                double x = a + (i + 0.5) * h;
                sum += func(x);
            }
        }

        return sum * h;
    }

    void print() const {
        std::cout << "Результат: " << calc() << "\n";
    }
};

double square(double x) {
    return x * x;
}

int main() {
    setlocale(LC_ALL, "Ru_ru.UTF8");
    IntegralCalculator ic;

    ic.setFunc(square);
    ic.setLimits(0, 2);
    ic.setSegments(1000);
    ic.setMethod(3);

    ic.print();
    system("pause");
    return 0;
}