#include <iostream>
#include <string>
#include <cstdlib>
#include <locale.h>

using namespace std;

class SecretNumber {
private:
    string number;
    int length;

public:
    SecretNumber(int n) {
        length = n;

        string digits = "0123456789";

        for (int i = 0; i < 50; i++) {
            int a = rand() % 10;
            int b = rand() % 10;
            char temp = digits[a];
            digits[a] = digits[b];
            digits[b] = temp;
        }

        number = "";
        for (int i = 0; i < n; i++) {
            number += digits[i];
        }

        if (number[0] == '0') {
            for (int i = 1; i < n; i++) {
                if (number[i] != '0') {
                    char temp = number[0];
                    number[0] = number[i];
                    number[i] = temp;
                    break;
                }
            }
        }
    }

    void check(string guess, int& bulls, int& cows) {
        bulls = 0;
        cows = 0;

        for (int i = 0; i < length; i++) {
            if (guess[i] == number[i]) {
                bulls++;
            }
            else {
                for (int j = 0; j < length; j++) {
                    if (guess[i] == number[j]) {
                        cows++;
                        break;
                    }
                }
            }
        }
    }

    string getNumber() {
        return number;
    }
};

class Player {
private:
    string name;
    int length;

public:
    Player(string n, int len) {
        name = n;
        length = len;
    }

    string makeGuess() {
        string guess;
        bool ok = false;

        while (!ok) {
            cout << name << ", введите " << length << "-значное число: ";
            cin >> guess;

            ok = true;

            if (guess.length() != length) {
                cout << "Ошибка! Нужно " << length << " цифр." << endl;
                ok = false;
                continue;
            }

            if (guess[0] == '0') {
                cout << "Ошибка! Число не может начинаться с 0." << endl;
                ok = false;
                continue;
            }

            for (int i = 0; i < length; i++) {
                if (guess[i] < '0' || guess[i] > '9') {
                    cout << "Ошибка! Можно вводить только цифры." << endl;
                    ok = false;
                    break;
                }
                for (int j = i + 1; j < length; j++) {
                    if (guess[i] == guess[j]) {
                        cout << "Ошибка! Цифры не должны повторяться." << endl;
                        ok = false;
                        break;
                    }
                }
                if (!ok) break;
            }
        }

        return guess;
    }
};

class Game {
private:
    SecretNumber secret;
    Player human;
    int attempts;

public:
    Game(int n) : secret(n), human("Игрок", n) {
        attempts = 0;
    }

    void start() {
        int bulls = 0;
        int cows = 0;

        while (bulls != secret.getNumber().length()) {
            attempts++;
            cout << "\nПопытка " << attempts << ":" << endl;

            string guess = human.makeGuess();

            secret.check(guess, bulls, cows);

            cout << "Коров: " << cows << ", Быков: " << bulls << endl;

            if (bulls == secret.getNumber().length()) {
                cout << "\nПОБЕДА!" << endl;
                cout << "Ты отгадал число " << secret.getNumber() << "!" << endl;
                cout << "Количество попыток: " << attempts << endl;
            }
        }
    }
};

int main() {
    setlocale(LC_ALL, "Russian");
    int n;

    cout << "=== ИГРА 'БЫКИ И КОРОВЫ' ===" << endl;
    cout << "Компьютер загадает число, а ты должен отгадать его." << endl;
    cout << endl;

    cout << "Введите длину числа (1-4): ";
    cin >> n;

    while (n < 1 || n > 4) {
        cout << "Ошибка! Введите число от 1 до 4: ";
        cin >> n;
    }

    cout << endl;
    cout << "Компьютер загадал " << n << "-значное число!" << endl;
    cout << "Попробуй отгадать его." << endl;

    Game game(n);
    game.start();

    system("pause");
    return 0;
}