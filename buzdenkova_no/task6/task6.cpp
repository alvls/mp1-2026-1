#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <algorithm>
#include <set>

class BullsAndCows {
private:
    int length;
    std::vector<int> secretNumber;
    std::mt19937 gen;

public:
    BullsAndCows(int len) : length(len) {
        std::random_device rd;
        gen.seed(rd());
        generateSecretNumber();
    }

    void generateSecretNumber() {
        std::vector<int> digits = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
        std::shuffle(digits.begin(), digits.end(), gen);

        if (digits[0] == 0) {
            for (int i = 1; i < 10; ++i) {
                if (digits[i] != 0) {
                    std::swap(digits[0], digits[i]);
                    break;
                }
            }
        }

        secretNumber.assign(digits.begin(), digits.begin() + length);
    }

    bool validateInput(const std::string& input) const {
        if (input.length() != static_cast<size_t>(length)) {
            return false;
        }

        std::set<int> uniqueDigits;
        for (char c : input) {
            if (!std::isdigit(c)) {
                return false;
            }
            int digit = c - '0';
            if (uniqueDigits.find(digit) != uniqueDigits.end()) {
                return false;
            }
            uniqueDigits.insert(digit);
        }
        return true;
    }

    std::pair<int, int> countBullsAndCows(const std::vector<int>& guess) const {
        int bulls = 0, cows = 0;

        for (int i = 0; i < length; ++i) {
            if (guess[i] == secretNumber[i]) {
                ++bulls;
            }
            else if (std::find(secretNumber.begin(), secretNumber.end(), guess[i]) != secretNumber.end()) {
                ++cows;
            }
        }

        return { bulls, cows };
    }

    void play() {
        std::cout << "Enter your guesses (or 'q' to quit):\n";

        std::string input;
        while (std::cin >> input) {
            if (input == "q" || input == "Q") {
                break;
            }

            if (!validateInput(input)) {
                std::cout << "Error! Different digits/incorrect length/using non-numbers. Try again: ";
                continue;
            }

            std::vector<int> guess;
            for (char c : input) {
                guess.push_back(c - '0');
            }

            std::pair<int, int> result = countBullsAndCows(guess);
            int bulls = result.first;
            int cows = result.second;

            std::cout << "Bulls: " << bulls << ", Cows: " << cows << "\n";

            if (bulls == length) {
                std::cout << "You've guessed the number!\n";
                break;
            }
        }
    }

    void showSecretNumber() const {
        std::cout << "Secret number: ";
        for (int digit : secretNumber) {
            std::cout << digit;
        }
        std::cout << "\n";
    }
};

int main() {
    const int MIN_LENGTH = 2;
    const int MAX_LENGTH = 5;
    int n;

    std::cout << "Enter the length of the hidden number ("
        << MIN_LENGTH << " to " << MAX_LENGTH << "): ";

    while (!(std::cin >> n) || n < MIN_LENGTH || n > MAX_LENGTH) {
        std::cout << "Invalid length. Please enter a number from "
            << MIN_LENGTH << " to " << MAX_LENGTH << ": ";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    BullsAndCows game(n);

    game.play();

    return 0;
}