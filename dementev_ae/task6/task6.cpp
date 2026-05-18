#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <random>
#include <ctime>

using namespace std;

class SecretNumber
{
private:
    string number;

    string generateUniqueNumber(int length)
    {
        vector<int> digits;
        for (int i = 0; i <= 9; i++)
        {
            digits.push_back(i);
        }

        random_device rd;
        mt19937 gen(rd());

        uniform_int_distribution<> distFirst(1, 9);
        int firstDigit = distFirst(gen);

        string result = to_string(firstDigit);

        auto it = find(digits.begin(), digits.end(), firstDigit);
        digits.erase(it);

        for (int i = 1; i < length; i++)
        {
            uniform_int_distribution<> dist(0, digits.size() - 1);
            int index = dist(gen);
            result += to_string(digits[index]);
            digits.erase(digits.begin() + index);
        }

        return result;
    }

public:
    SecretNumber(int length)
    {
        if (length < 1 || length > 10)
        {
            throw invalid_argument("Длина числа должна быть от 1 до 10.");
        }
        number = generateUniqueNumber(length);
    }

    string getNumber()
    {
        return number;
    }

    pair<int, int> getBullsAndCows(string guess)
    {
        if (guess.length() != number.length())
        {
            throw invalid_argument("Длина попытки не совпадает с длиной загаданного числа.");
        }

        int bulls = 0;
        int cows = 0;

        for (size_t i = 0; i < number.length(); i++)
        {
            if (guess[i] == number[i])
            {
                bulls++;
            }
            else if (number.find(guess[i]) != string::npos)
            {
                cows++;
            }
        }
        return make_pair(bulls, cows);
    }
};

class Player
{
public:
    virtual string makeGuess(int numberLength) = 0;
    virtual ~Player() {}
};

class HumanPlayer : public Player
{
public:
    string makeGuess(int numberLength) override
    {
        string guess;
        while (true)
        {
            cout << "Введите " << numberLength << "-значное число с неповторяющимися цифрами: ";
            cin >> guess;

            if (guess.length() != numberLength)
            {
                cout << "Ошибка: число должно быть " << numberLength << " цифр." << endl;
                continue;
            }

            bool allDigits = true;
            for (char c : guess)
            {
                if (!isdigit(c))
                {
                    allDigits = false;
                    break;
                }
            }
            if (!allDigits)
            {
                cout << "Ошибка: можно вводить только цифры." << endl;
                continue;
            }

            bool unique = true;
            for (size_t i = 0; i < guess.length(); i++)
            {
                for (size_t j = i + 1; j < guess.length(); j++)
                {
                    if (guess[i] == guess[j])
                    {
                        unique = false;
                        break;
                    }
                }
                if (!unique)
                    break;
            }
            if (!unique)
            {
                cout << "Ошибка: цифры в числе не должны повторяться." << endl;
                continue;
            }

            if (guess[0] == '0' && numberLength > 1)
            {
                cout << "Ошибка: число не может начинаться с нуля." << endl;
                continue;
            }

            return guess;
        }
    }
};

class ComputerPlayer : public Player
{
private:
    mt19937 gen;

public:
    ComputerPlayer() : gen(random_device{}()) {}

    string makeGuess(int numberLength) override
    {
        vector<int> digits;
        for (int i = 0; i <= 9; i++)
        {
            digits.push_back(i);
        }

        uniform_int_distribution<> distFirst(1, 9);
        int firstDigit = distFirst(gen);

        string guess = to_string(firstDigit);

        auto it = find(digits.begin(), digits.end(), firstDigit);
        digits.erase(it);

        for (int i = 1; i < numberLength; i++)
        {
            uniform_int_distribution<> dist(0, digits.size() - 1);
            int index = dist(gen);
            guess += to_string(digits[index]);
            digits.erase(digits.begin() + index);
        }

        cout << "Компьютер предполагает: " << guess << endl;
        return guess;
    }
};

class BullsAndCowsGame
{
private:
    int numberLength;
    SecretNumber *secretNumber;
    Player *human;
    Player *computer;
    bool isHumanTurn;

public:
    BullsAndCowsGame()
    {
        human = new HumanPlayer();
        computer = new ComputerPlayer();
    }

    ~BullsAndCowsGame()
    {
        delete secretNumber;
        delete human;
        delete computer;
    }

    void start()
    {
        cout << "=== Игра Быки и коровы ===" << endl;
        cout << "Правила: компьютер загадывает число с неповторяющимися цифрами." << endl;
        cout << "Вы вводите свой вариант, а компьютер говорит сколько быков (цифра на своем месте) и коров (цифра есть, но не на своем месте)." << endl
             << endl;

        while (true)
        {
            cout << "Выберите длину загадываемого числа (от 1 до 10): ";
            cin >> numberLength;
            if (numberLength >= 1 && numberLength <= 10)
            {
                break;
            }
            cout << "Некорректный ввод. Длина должна быть от 1 до 10." << endl;
        }

        secretNumber = new SecretNumber(numberLength);
        cout << "Компьютер загадал " << numberLength << "-значное число (цифры не повторяются)." << endl;
        cout << "Начинаем игру!" << endl
             << endl;

        isHumanTurn = true;

        while (true)
        {
            if (isHumanTurn)
            {
                cout << "\n--- Ваш ход ---" << endl;
                string guess = human->makeGuess(numberLength);
                pair<int, int> result = secretNumber->getBullsAndCows(guess);
                cout << "Результат: Быки = " << result.first << ", Коровы = " << result.second << endl;

                if (result.first == numberLength)
                {
                    cout << "Поздравляю! Вы отгадали число!" << endl;
                    cout << "Игра окончена. Вы победили!" << endl;
                    break;
                }
            }
            else
            {
                cout << "\n--- Ход компьютера (демонстрационный) ---" << endl;
                string guess = computer->makeGuess(numberLength);
                pair<int, int> result = secretNumber->getBullsAndCows(guess);
                cout << "Результат: Быки = " << result.first << ", Коровы = " << result.second << endl;

                if (result.first == numberLength)
                {
                    cout << "Компьютер угадал число! Вы проиграли." << endl;
                    break;
                }
            }

            isHumanTurn = !isHumanTurn;
        }
    }
};

int main()
{
    setlocale(LC_ALL, "Russian");
    BullsAndCowsGame game;
    game.start();
    return 0;
}