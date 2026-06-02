#include <iostream>
#include <vector>
#include <string>
#include <ctime>

using namespace std;

class Computer {
private:
    string secret;

public:
    void createNumber(int n) {
        secret = "";
        srand(time(0));

        while (secret.length() < n) {

            char num = '0' + rand() % 10;
            bool repeat = false;

            for (int i = 0; i < secret.length(); i++) {
                if (secret[i] == num) {
                    repeat = true;
                    break;
                }
            }

            if (secret.empty() && num == '0') {
                continue;
            }

            if (!repeat) {
                secret += num;
            }
        }
    }

    void check(string number, int& bulls, int& cows) {

        bulls = 0;
        cows = 0;

        for (int i = 0; i < number.length(); i++) {

            if (number[i] == secret[i]) {
                bulls++;
            }
            else {
                for (int j = 0; j < secret.length(); j++) {
                    if (number[i] == secret[j]) {
                        cows++;
                    }
                }
            }
        }
    }

    string getSecret() {
        return secret;
    }
};

class Player {
public:
    string enterNumber(int n) {

        string number;

        while (true) {

            cout << "Enter a number: ";
            cin >> number;

            bool correct = true;

            if (number.length() != n) {
                correct = false;
            }

            if (number[0] == '0') {
                correct = false;
            }

            for (int i = 0; i < number.length(); i++) {

                if (!isdigit(number[i])) {
                    correct = false;
                }

                for (int j = i + 1; j < number.length(); j++) {
                    if (number[i] == number[j]) {
                        correct = false;
                    }
                }
            }

            if (correct) {
                return number;
            }

            cout << "Wrong input. Try again.\n";
        }
    }
};

class Game {
private:
    int size;
    Computer pc;
    Player human;
    vector<string> variants;

public:

    void start() {

        int mode;

        cout << "We are playing Bulls and Cows game\n";
        cout << "1 - Player guesses\n";
        cout << "2 - Computer guesses\n";
        cout << "Choose the gamemode: ";
        cin >> mode;

        cout << "Enter the length of a number: ";
        cin >> size;

        while (size < 1 || size > 10) {
            cout << "Enter a number from 1 to 10: ";
            cin >> size;
        }

        if (mode == 1) {
            playerGuesses();
        }
        else if (mode == 2) {
            computerGuesses();
        }
        else {
            cout << "Wrong gamemode.\n";
        }
    }

private:

    // Режим 1 — игрок угадывает
    void playerGuesses() {

        pc.createNumber(size);

        int bulls = 0;
        int cows = 0;
        int tries = 0;

        while (bulls != size) {

            string guess = human.enterNumber(size);

            tries++;

            pc.check(guess, bulls, cows);

            cout << "Number of bulls: " << bulls
                << " Number of cows: " << cows << endl;
        }

        cout << "\nPlayer won!\n";
        cout << "Amount of attempts: " << tries << endl;
    }

    // Проверка числа
    bool validNumber(string s) {

        if (s.length() != size) {
            return false;
        }

        if (s[0] == '0') {
            return false;
        }

        for (int i = 0; i < s.length(); i++) {

            for (int j = i + 1; j < s.length(); j++) {
                if (s[i] == s[j]) {
                    return false;
                }
            }
        }

        return true;
    }

    // Сравнение двух чисел
    void compare(string secret,
        string guess,
        int& bulls,
        int& cows) {

        bulls = 0;
        cows = 0;

        for (int i = 0; i < guess.length(); i++) {

            if (guess[i] == secret[i]) {
                bulls++;
            }
            else {
                for (int j = 0; j < secret.length(); j++) {

                    if (guess[i] == secret[j]) {
                        cows++;
                    }
                }
            }
        }
    }

    // Генерация чисел
    void generateVariants(string current) {

        if (current.length() == size) {
            variants.push_back(current);
            return;
        }

        for (char c = '0'; c <= '9'; c++) {

            bool used = false;

            for (char x : current) {
                if (x == c) {
                    used = true;
                }
            }

            if (used) {
                continue;
            }

            if (current.empty() && c == '0') {
                continue;
            }

            generateVariants(current + c);
        }
    }

    // Режим 2 — компьютер угадывает
    void computerGuesses() {

        cout << "\nGuess a number.\n";
        cout << "The computer will now try to guess it.\n";

        variants.clear();

        generateVariants("");

        int tries = 0;

        while (!variants.empty()) {

            string guess = variants[0];
            tries++;

            cout << "\nCOmputer's guess: "
                << guess << endl;

            int bulls, cows;

            cout << "Enter amount of bulls: ";
            cin >> bulls;

            cout << "Enter amount of cows: ";
            cin >> cows;

            if (bulls == size) {
                cout << "\nComputer guessed the number in "
                    << tries
                    << " attempts.\n";
                return;
            }

            vector<string> newVariants;

            for (string variant : variants) {

                int b, c;

                compare(variant, guess, b, c);

                if (b == bulls && c == cows) {
                    newVariants.push_back(variant);
                }
            }

            variants = newVariants;
        }

        cout << "Looks like computer can't guess the number.\n";
    }
};

int main() {

    setlocale(LC_ALL, "Russian");

    Game game;
    game.start();

    return 0;
}