#include <iostream>
#include <windows.h>
#include <string>
#include <chrono>
#include <unordered_map>
#include <random>

class Computer {
public:
	size_t word_length = 1;
	std::string word = "";

	Computer(size_t _word_length) : word_length(_word_length) {}
	Computer() {}

	void Make_Word() {
		word = "";

		std::vector<int> digits = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

		std::random_device rd;
		std::mt19937 g(rd());
		
		std::shuffle(digits.begin(), digits.end(), g);

		if (digits[0] == 0 && word_length > 1) {
			std::uniform_int_distribution<> dist(1, word_length - 1);
			int random_pos = dist(g);
			std::swap(digits[0], digits[random_pos]);
		}

		for (int i = 0; i < word_length; i++) {
			word += std::to_string(digits[i]);
		}
	}

	std::pair<int, int> Check_Guess(std::string guess) const {
		std::pair<int, int> result = { 0, 0 };

		for (int i = 0; i < word_length; i++) {
			if (word[i] == guess[i]) result.second++;
			else if (word.find(guess[i]) != std::string::npos) result.first++;
		}

		return result;
	}
};

class Game {
public:
	Computer computer;

	Game(Computer _computer) : computer(_computer) {}

	void Stage1() {
		std::cout << "Введите длину загадываемого числа\n";
		int length;
		std::cin >> length;

		if (length < 1 || length > 10) throw std::exception("bad word length");

		computer.word_length = length;
	}

	void Stage2() {
		computer.Make_Word();
	}

	void Stage3() {
		std::cout << "Угадывайте число\n";
		bool win = false;
		while (win == false) {
			std::string guess;
			std::cin >> guess;

			if (guess.length() != computer.word_length) {
				std::cout << "Неправильная длина слова. Вводите слова длины " << computer.word_length << std::endl;
				continue;
			}

			std::pair<int, int> check_result = computer.Check_Guess(guess);

			std::cout << "Коров: " << std::to_string(check_result.first) << ", Быков: " << std::to_string(check_result.second) << std::endl;

			if (check_result.second == computer.word_length) {
				std::cout << "Победа!\n";
				win = true;
			}
		}
	}

	void Loop() {
		while (1) {
			try {
				Stage1();
			}
			catch (std::exception) { continue; }
			Stage2();
			Stage3();
		}
	}
};

int main() {
	SetConsoleOutputCP(1251);
	SetConsoleCP(1251);

	Game game = Game(Computer());

	game.Loop();
}