#include <iostream>
#include <string>
#include <ctime>

using namespace std;

class Computer {
private:
    string secret;

public:
    void createNumber(int n) { //генератор рандомных чисел
        srand(time(0));

        while (secret.length() < n) {
            char num = '0' + rand() % 10;

            bool repeat = false;

            for (int i = 0; i < secret.length(); i++) { //проверка нае повтор
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

    void check(string playerNumber, int& bulls, int& cows) {
        bulls = 0;
        cows = 0;

        for (int i = 0; i < playerNumber.length(); i++) {

            if (playerNumber[i] == secret[i]) {
                bulls++;
            }
            else {
                for (int j = 0; j < secret.length(); j++) {
                    if (playerNumber[i] == secret[j]) {
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

            for (int i = 0; i < number.length(); i++) { // проверка повторов и цифр

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

public:
    void start() {

        cout << "We are playing bulls and cows game\n";
        cout << "Enter the length of a number: ";
        cin >> size;

        while (size < 1 || size > 10) {
            cout << "Enter a number from 1 to 10: ";
            cin >> size;
        }

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

        cout << "\nYou won!\n";
        cout << "Amount of attempts: " << tries << endl;
    }
};

int main() {
    Game game;
    game.start();
    return 0;
}