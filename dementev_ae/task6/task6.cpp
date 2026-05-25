#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <ctime>

class NumberValidator {
public:
    static bool isValid(const std::string& number, int length) {
        if (number.length() != length) return false;
        
        for (char c : number) {
            if (c < '0' || c > '9') return false;
        }
        
        for (size_t i = 0; i < number.length(); i++) {
            for (size_t j = i + 1; j < number.length(); j++) {
                if (number[i] == number[j]) return false;
            }
        }
        
        return true;
    }
};

class BullsAndCows {
private:
    std::string secretNumber;
    int length;
    int attempts;
    
    std::pair<int, int> calculateBullsAndCows(const std::string& guess) const {
        int bulls = 0;
        int cows = 0;
        
        std::vector<bool> secretUsed(length, false);
        std::vector<bool> guessUsed(length, false);
        
        for (int i = 0; i < length; i++) {
            if (guess[i] == secretNumber[i]) {
                bulls++;
                secretUsed[i] = true;
                guessUsed[i] = true;
            }
        }
        
        for (int i = 0; i < length; i++) {
            if (!guessUsed[i]) {
                for (int j = 0; j < length; j++) {
                    if (!secretUsed[j] && guess[i] == secretNumber[j]) {
                        cows++;
                        secretUsed[j] = true;
                        break;
                    }
                }
            }
        }
        
        return {bulls, cows};
    }
    
    std::string generateSecretNumber() {
        std::string digits = "0123456789";
        
        for (int i = 9; i > 0; i--) {
            int j = rand() % (i + 1);
            std::swap(digits[i], digits[j]);
        }
        
        return digits.substr(0, length);
    }
    
public:
    BullsAndCows(int n) : length(n), attempts(0) {
        if (n < 1 || n > 10) {
            throw std::invalid_argument("Длина числа должна быть от 1 до 10");
        }
        secretNumber = generateSecretNumber();
    }
    
    bool makeGuess(const std::string& guess, int& bulls, int& cows) {
        if (!NumberValidator::isValid(guess, length)) {
            throw std::invalid_argument("Некорректное число!");
        }
        
        attempts++;
        auto result = calculateBullsAndCows(guess);
        bulls = result.first;
        cows = result.second;
        
        return (bulls == length);
    }
    
    int getAttempts() const { return attempts; }
    int getLength() const { return length; }
};

class Game {
private:
    BullsAndCows game;
    bool gameOver;
    
    void displayRules() const {
        std::cout << "Компьютер загадал число\n";
    }
    
public:
    Game(int length) : game(length), gameOver(false) {
        displayRules();
    }
    
    void play() {
        while (!gameOver) {
            std::string guess;
            std::cout << "Попытка #" << (game.getAttempts() + 1) 
                      << ". Введите " << game.getLength() 
                      << "-значное число: ";
            std::cin >> guess;
            
            try {
                int bulls, cows;
                bool isGuessed = game.makeGuess(guess, bulls, cows);
                
                std::cout << "Результат: " << cows << " коров";
                if (cows == 1) std::cout << "а";
                else if (cows >= 2 && cows <= 4) std::cout << "ы";
                std::cout << ", " << bulls << " бык";
                if (bulls != 1) std::cout << "а";
                std::cout << "\n\n";
                
                if (isGuessed) {
                    std::cout << "Вы отгадали число " << guess << "!\n";
                    std::cout << "Количество попыток: " << game.getAttempts() << "\n";
                    gameOver = true;
                }
            } catch (const std::exception& e) {
                std::cout << "Ошибка: " << e.what() << "\n";
                std::cout << "Попробуйте снова.\n\n";
            }
        }
    }
};

class GameMenu {
private:
    int getValidNumber() {
        int n;
        while (true) {
            std::cout << "Введите длину числа (1-10): ";
            std::cin >> n;
            
            if (std::cin.fail() || n < 1 || n > 10) {
                std::cin.clear();
                std::cin.ignore(10000, '\n');
                std::cout << "Ошибка! Введите число от 1 до 10.\n";
            } else {
                return n;
            }
        }
    }
    
public:
    void start() {
        int length = getValidNumber();
        
        Game game(length);
        game.play();
        
        std::cout << "\nХотите сыграть еще? (y/n): ";
        char answer;
        std::cin >> answer;
        
        if (answer == 'y' || answer == 'Y') {
            std::cout << "\n\n";
            start();
        } else {
            std::cout << "Спасибо за игру!\n";
        }
    }
};

int main() {
    srand(time(0));
    GameMenu menu;
    menu.start();
    
    return 0;
}