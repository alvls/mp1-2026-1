#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include <algorithm>
using namespace std;

// класс "быки и коровы"
class BullsAndCowsGame {
private:
    vector<int> secretNumber;   // загаданное число (цифры)
    int digitsCount;            // количество цифр в числе

    // генерация случайного числа с уникальными цифрами, первая цифра не 0
    void generateSecretNumber() {
        vector<int> digits;
        for (int i = 0; i <= 9; ++i) digits.push_back(i);

        // перемешиваем цифры
        srand(static_cast<unsigned>(time(nullptr)));
        random_shuffle(digits.begin(), digits.end());

        // берём первые digitsCount цифр
        secretNumber.clear();
        for (int i = 0; i < digitsCount; ++i) {
            secretNumber.push_back(digits[i]);
        }

        // первая цифра не должна быть 0
        if (secretNumber[0] == 0) {
            // меняем местами с любой ненулевой цифрой среди выбранных
            for (int i = 1; i < digitsCount; ++i) {
                if (secretNumber[i] != 0) {
                    swap(secretNumber[0], secretNumber[i]);
                    break;
                }
            }
        }
    }

    // преобразование строки в вектор цифр с проверкой корректности
    vector<int> parseGuess(const string& guessStr) const {
        if (guessStr.length() != static_cast<size_t>(digitsCount)) {
            throw invalid_argument("Число должно содержать ровно " + to_string(digitsCount) + " цифр.");
        }

        vector<int> guess;
        for (char ch : guessStr) {
            if (!isdigit(ch)) {
                throw invalid_argument("Вводите только цифры.");
            }
            int digit = ch - '0';
            guess.push_back(digit);
        }

        // проверка на уникальность цифр (опционально, но по правилам игры желательно)
        vector<bool> seen(10, false);
        for (int d : guess) {
            if (seen[d]) {
                throw invalid_argument("Цифры в числе не должны повторяться.");
            }
            seen[d] = true;
        }

        return guess;
    }

public:
    // конструктор
    BullsAndCowsGame(int n) : digitsCount(n) {
        if (n < 2 || n > 5) {
            throw out_of_range("Количество цифр должно быть от 2 до 5.");
        }
        generateSecretNumber();
    }

    // получить количество цифр
    int getDigitsCount() const {
        return digitsCount;
    }

    // подсчёт быков и коров для введённой строки
    pair<int, int> countBullsAndCows(const string& guessStr) {
        vector<int> guess = parseGuess(guessStr);

        int bulls = 0, cows = 0;
        for (int i = 0; i < digitsCount; ++i) {
            if (guess[i] == secretNumber[i]) {
                ++bulls;
            }
            else {
                // если цифра есть в загаданном числе, но на другой позиции
                for (int j = 0; j < digitsCount; ++j) {
                    if (guess[i] == secretNumber[j]) {
                        ++cows;
                        break;
                    }
                }
            }
        }
        return { bulls, cows };
    }

    // проверка, угадано ли число полностью
    bool isGuessed(const string& guessStr) {
        vector<int> guess = parseGuess(guessStr);
        return guess == secretNumber;
    }

    void printSecret() const {
        cout << "Загаданное число: ";
        for (int d : secretNumber) cout << d;
        cout << endl;
    }
};

// меню
void showMenu() {
    cout << "\n========== БЫКИ И КОРОВЫ ==========\n";
    cout << "Правила:\n";
    cout << " - Быки   : цифра угадана и стоит на своём месте.\n";
    cout << " - Коровы : цифра угадана, но не на своём месте.\n";
    cout << "Цифры в числе не повторяются, первая цифра не ноль.\n";
}

int main() {
    setlocale(LC_ALL, "rus");
    srand(static_cast<unsigned>(time(nullptr)));

    int choice = 1;
    do {
        showMenu();

        int n;
        cout << "\nВыберите количество цифр (от 2 до 5): ";
        cin >> n;

        if (cin.fail() || n < 2 || n > 5) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Ошибка: введите число от 2 до 5.\n";
            continue;
        }

        BullsAndCowsGame game(n);

        cout << "Число загадано. Попробуйте отгадать!\n";
        int attempts = 0;

        while (true) {
            string guess;
            cout << "Ваш ход (введите " << n << "-значное число): ";
            cin >> guess;

            try {
                if (game.isGuessed(guess)) {
                    ++attempts;
                    cout << "Поздравляю! Вы угадали число с " << attempts << " попытки(ок)!\n";
                    break;
                }
                else {
                    auto result = game.countBullsAndCows(guess);
                    cout << "Быков: " << result.first << ", Коров: " << result.second << endl;
                    ++attempts;
                }
            }
            catch (const exception& e) {
                cout << "Ошибка: " << e.what() << "\n";
                cout << "Попробуйте ещё раз.\n";
            }
        }

        cout << "\nХотите сыграть ещё?\n";
        cout << "1 - Да\n";
        cout << "0 - Нет\n";
        cout << "Ваш выбор: ";
        cin >> choice;
        cin.ignore();

    } while (choice == 1);

    cout << "Спасибо за игру!\n";
    return 0;
}