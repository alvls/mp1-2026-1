#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cstdlib>
#include <cctype>
#include <ctime>

using namespace std;

const int MISS = 0;
const int HIT = 1;
const int KILL = 2;
const int INVALID = -1;

class Ship {
private:
	int startX, startY;
	int length;
	bool horizontal;
	vector<bool> hits;
public:
	Ship(int x, int y, int len, bool horiz) {
		startX = x;
		startY = y;
		length = len;
		horizontal = horiz;
		hits = vector<bool>(len, false);
	}
	
	
	bool contains(int x, int y) {
		if (horizontal) {
			if (y == startY && x >= startX && x < startX + length) {
				return true;
			}
		}
		else {
			if (x == startX && y >= startY && y < startY + length) {
				return true;
			}
		}
		return false;
	}

	bool hit(int x, int y) {
		if (!contains(x, y)) {
			return false;
		}
		int index;
		if (horizontal) {
			index = x - startX;
		}
		else {
			index = y - startY;
		}
		if (hits[index]) {
			return false;
		}
		hits[index] = true;
		return true;
	}


	bool is_alive() {
		for (bool cellHit : hits) {
			if (!cellHit) {
				return true;
			}
		}
		return false;
	}

	int getLength() {
		return length;
	}

	vector<pair<int, int>> getCoordinates() {
		vector<pair<int, int>> coords;
		if (horizontal) {
			for (int i = 0; i < length; i++) {
				coords.push_back({ startX + i, startY });
			}
		}
		else {
			for (int i = 0; i < length; i++) {
				coords.push_back({ startX, startY + i });
			}
		}
		return coords;
	}
};



class Board {
private:
	static const int SIZE = 10;
	int grid[SIZE][SIZE];
	vector<Ship> ships;

	bool isInside(int x, int y) {
		return (x >= 0 && x < 10 && y >= 0 && y < 10);
	}

	bool hasAdjacentShip(int x, int y) {
		for (int dx = -1; dx <= 1; dx++) {
			for (int dy = -1; dy <= 1; dy++) {
				if (dx == 0 && dy == 0) {
					continue;
				}

				int nx = x + dx;
				int ny = y + dy;

				if (isInside(nx, ny)) {
					if (grid[nx][ny] == 1) {
						return true;
					}
				}
			}
		}
		return false;
	}

	void markSunkShip(int x, int y) {
		Ship* sunkShip = nullptr;
		for (Ship& ship : ships) {
			if (ship.contains(x, y)) {
				sunkShip = &ship;
				break;
			}
		}
		if (sunkShip == nullptr) {
			return;
		}

		vector<pair<int, int>> coords = sunkShip->getCoordinates();

		for (auto coord : coords) {
			int cx = coord.first;
			int cy = coord.second;

			for (int dx = -1; dx <= 1; dx++) {
				for (int dy = -1; dy <= 1; dy++) {
					if (dx == 0 && dy == 0) continue;

					int nx = cx + dx;
					int ny = cy + dy;

					if (isInside(nx, ny)) {
						if (grid[nx][ny] == 0) {
							grid[nx][ny] = 3;
						}
					}
				}
			}
		}
	}

public:
	Board() {
		for (int i = 0; i < SIZE; i++) {
			for (int j = 0; j < SIZE; j++) {
				grid[i][j] = 0;
			}
		}
		ships.clear();
	}

	bool canPlaceShip(int x, int y, int length, bool horizontal) {
		if (horizontal) {
			if (x + length - 1 >= 10) {
				return false;
			}
		}
		else {
			if (y + length - 1 >= 10) {
				return false;
			}
		}

		for (int i = 0; i < length; i++) {
			int curX, curY;

			if (horizontal) {
				curX = x + i;
				curY = y;
			}
			else {
				curX = x;
				curY = y + i;
			}

			if (!isInside(curX, curY)) {
				return false;
			}
			if (grid[curX][curY] != 0) {
				return false;
			}

			if (hasAdjacentShip(curX, curY)) {
				return false;
			}
		}
		return true;
	}
	bool placeShip(int x, int y, int length, bool horizontal) {
		if (!canPlaceShip(x, y, length, horizontal)) {
			return false;
		}
		Ship newShip(x, y, length, horizontal);
		ships.push_back(newShip);

		for (int i = 0; i < length; i++) {
			int curX, curY;

			if (horizontal) {
				curX = x + i;
				curY = y;
			}
			else {
				curX = x;
				curY = y + i;
			}
			grid[curX][curY] = 1;
		}
		return true;

	}
	int shoot(int x, int y) {
		if (!isInside(x, y)) {
			return INVALID;
		}
		if (grid[x][y] == 3 || grid[x][y] == 2) {
			return MISS;
		}
		if (grid[x][y] == 0) {
			grid[x][y] = 3;
			return MISS;
		}
		
		Ship* targetShip = nullptr;
		for (Ship& ship : ships) {
			if (ship.contains(x, y)) {
				targetShip = &ship;
				break;
			}
		}
		if (targetShip == nullptr) {
			return INVALID;
		}

		targetShip->hit(x, y);

		grid[x][y] = 2;

		if (!targetShip->is_alive()) {
			markSunkShip(x, y);
			return KILL;
		}
		
	}
	bool allShipsSunk() {
		for (Ship& ship : ships) {
			if (ship.is_alive()) {
				return false;
			}
		}
		return true;
	}

	void print(bool hideShips) {
		cout << "  ";
		for (int i = 0; i < 10; i++) {
			cout << " " << char('A' + i);
		}
		cout << endl;

		for (int y = 0; y < 10; y++) {
			cout << (y + 1);
			if (y + 1 < 10) cout << " ";
			for (int x = 0; x < 10; x++) {
				char symbol;

				if (grid[x][y] == 0) {
					symbol = '.';
				}
				else if (grid[x][y] == 1) {
					if (hideShips) {
						symbol = '.';
					}
					else {
						symbol = '#';
					}
				}
				else if (grid[x][y] == 2) {
					symbol = 'X';
				}
				else if (grid[x][y] == 3) {
					symbol = 'O';
				}
				else {
					symbol = '?';
				}
				cout << " " << symbol;
			}
			cout << endl;
		}
	}

};



class Player {
protected:
	Board& myBoard;
	Board& enemyBoard;
public:
	Player(Board& my, Board& enemy) : myBoard(my), enemyBoard(enemy) {}

	virtual ~Player() {}

	virtual void placeShips() = 0;
	virtual bool makeMove() = 0;

	Board& getMyBoard() { return myBoard; }
	Board& getEnemyBoard() { return enemyBoard; }
};


class HumanPlayer : public Player {
public:
	HumanPlayer(Board& my, Board& enemy) : Player(my, enemy) {}

	void placeShips() override {
		cout << "\n=== SHIP PLACEMENT ===\n";
		cout << "Your board is 10x10. Coordinates from A1 to J10.\n";
		cout << "Ships cannot touch each other (even diagonally).\n\n";

		int shipsToPlace[4][2] = {
			{4, 1},
			{3, 2},
			{2, 3},
			{1, 4}
		};

		for (int i = 0; i < 4; i++) {
			int length = shipsToPlace[i][0];
			int count = shipsToPlace[i][1];

			for (int j = 0; j < count; j++) {
				bool placed = false;

				while (!placed) {
					cout << "\nCurrent board:\n";
					myBoard.print(false);

					cout << "\nPlacing " << length << "-deck ship #" << (j + 1) << "\n";

					string coord;
					cout << "Enter starting coordinates (for example A5): ";
					cin >> coord;

					int x = toupper(coord[0]) - 'A';
					int y = 0;

					if (coord.length() == 3) {
						y = 9;
					}
					else {
						y = coord[1] - '1';
					}

					char dir;
					cout << "Enter direction (H - horizontal, V - vertical): ";
					cin >> dir; 
					bool horizontal = (toupper(dir) == 'H');

					if (myBoard.placeShip(x, y, length, horizontal)) {
						placed = true;
						cout << "Ship placed!\n";
					}
					else {
						cout << "Cannot place there! Check:\n";
						cout << "- Ship fits inside the board\n";
						cout << "- Cells are not occupied\n";
						cout << "- No touching other ships\n\n";
					}
				}
			}
		}
		cout << "\n=== PLACEMENT COMPLETE ===\n";
		myBoard.print(false);
		cout << "Press Enter to continue...";
		cin.ignore();
		cin.get();
	}

	bool makeMove() override {
		cout << "\n=== YOUR TURN ===\n";
		cout << "Enemy board (your shots):\n";
		enemyBoard.print(true);

		int x, y;
		bool valid = false;

		while (!valid) {
			string coord;
			cout << "\nEnter shot coordinates (for example A5): ";
			cin >> coord;

			x = toupper(coord[0]) - 'A';
			if (coord.length() == 3) {
				y = 9;
			}
			else {
				y = coord[1] - '1';
			}

			if (x >= 0 && x < 10 && y >= 0 && y < 10) {
				valid = true;
			}
			else {
				cout << "Invalid coordinates! Enter A1 to J10.\n";
			}
		}

		int result = enemyBoard.shoot(x, y);

		if (result == -1) {
			cout << "Already shot there! Turn lost.\n";
			return false;
		}
		else if (result == 0) {
			cout << "Miss!\n";
			return false;
		}
		else if (result == 1) {
			cout << "Hit!\n";
			return true;
		}
		else if (result == 2) {
			cout << "Hit! SHIP SUNK!\n";
			return true;
		}
		return false;
	}
};


class ComputerPlayer : public Player {
private:
	vector<pair<int, int>> availableShots;

	vector<pair<int, int>> hitStack;
	enum Mode { RANDOM, TARGETING };
	Mode currentMode;

	vector<pair<int, int>> potentialTargets;

	void initAvailableShots() {
		for (int x = 0; x < 10; x++) {
			for (int y = 0; y < 10; y++) {
				availableShots.push_back({ x, y });
			}
		}
	}

	bool isValidShot(int x, int y) {
		if (x < 0 || x >= 10 || y < 0 || y >= 10) {
			return false;
		}

		for (auto shot : availableShots) {
			if (shot.first == x && shot.second == y) {
				return true;
			}
		}
		return false;
	}

	void addPotentialTargets(int x, int y) {
		vector<pair<int, int>> directions = {
			{0, -1}, {0, 1}, {-1, 0}, {1, 0}
		}; 
		for (auto dir : directions) {
			int nx = x + dir.first;
			int ny = y + dir.second;

			if (isValidShot(nx, ny)) {
				bool alreadyInList = false;
				for (auto target : potentialTargets) {
					if (target.first == nx && target.second == ny) {
						alreadyInList = true;
						break;
					}
				}
				if (!alreadyInList) {
					potentialTargets.push_back({ nx, ny });
				}
			}
		}
	}
	void clearPotentialTargets() {
		potentialTargets.clear();
		hitStack.clear();
	}

public:
	ComputerPlayer(Board& my, Board& enemy) : Player(my, enemy) {
		srand(time(0));
		initAvailableShots();
		currentMode = RANDOM;
	}

	void placeShips() override {
		cout << "\n=== COMPUTER IS PLACING ITS SHIPS ===\n";

		int shipsToPlace[4][2] = {
			{4, 1},
			{3, 2},
			{2, 3},
			{1, 4}
		};
		for (int i = 0; i < 4; i++) {
			int length = shipsToPlace[i][0];
			int count = shipsToPlace[i][1];
			
			for (int j = 0; j < count; j++) {
				bool placed = false;
				while (!placed) {
					int x = rand() % 10;
					int y = rand() % 10;
					bool horizontal = (rand() % 2 == 0);

					if (myBoard.placeShip(x, y, length, horizontal)) {
						placed = true;
					}
				}
			}
		}
		cout << "Computer's ships placed!\n";
		cout << "Press Enter to continue...";
		cin.ignore();
		cin.get();
	}

	bool makeMove() override {
		cout << "\n=== COMPUTER'S TURN ===\n";
		
		int x, y;

		if (currentMode == TARGETING && !potentialTargets.empty()) {
			int index = rand() % potentialTargets.size();
			x = potentialTargets[index].first;
			y = potentialTargets[index].second;

			potentialTargets.erase(potentialTargets.begin() + index);
			cout << "Computer is targeting...\n";
		}
		else {
			if (availableShots.empty()) {
				return false;
			}

			int index = rand() % availableShots.size();
			x = availableShots[index].first;
			y = availableShots[index].second;

			cout << "Computer is searching...\n";
		}

		for (int i = 0; i < availableShots.size(); i++) {
			if (availableShots[i].first == x && availableShots[i].second == y) {
				availableShots.erase(availableShots.begin() + i);
				break;
			}
		}

		char col = 'A' + x;
		int row = y + 1;
		cout << "Computer shoots at " << col << row << "...\n";

		int result = enemyBoard.shoot(x, y);

		if (result == 0) {
			cout << "Computer missed!\n";
			return false;
		}
		else if (result == 1) {
			cout << "Computer HIT!\n";
			hitStack.push_back({ x, y });
			addPotentialTargets(x, y);
			currentMode = TARGETING;
			return true;
		}
		else if (result == 2) {
			cout << "Computer HIT! SHIP SUNK!\n";
			clearPotentialTargets();
			currentMode = RANDOM;
			return true;
		}
		else {
			return false;
		}
	}

};


class Game {
private:
	Board playerBoard;
	Board computerBoard;
	HumanPlayer human;
	ComputerPlayer computer;
public:
	Game() : human(playerBoard, computerBoard), computer(computerBoard, playerBoard) {}

	void start() {
		cout << "=====================================\n";
		cout << "         SEA BUTTLE GAME\n";
		cout << "=====================================\n";
		cout << "Rules:\n";
		cout << "- You have 4 type of ships: 4, 3, 3, 2, 2, 2, 1, 1, 1, 1\n";
		cout << "- Ships cannot touch each other\n";
		cout << "- First player to sink all enemy ships wins\n";
		cout << "- You go first!\n";
		cout << "=====================================\n\n";

		human.placeShips();
		computer.placeShips();

		bool humanTurn = true;
		bool gameRunning = true;

		while (gameRunning) {
			cout << "\n===================================\n";

			if (humanTurn) {
				cout << "YOUR TURN\n";
				cout << "=================================\n";

				cout << "\nYour board (your ships):\n";
				playerBoard.print(false);
				cout << "\nEnemy board (your shots):\n";
				computerBoard.print(true);
				bool hit = human.makeMove();
				if (computerBoard.allShipsSunk()) {
					cout << "\n=======================================\n";
					cout << "CONGRATULATIONS! YOU WIN!\n";
					cout << "\n=======================================\n";
					gameRunning = false;
					break;
				}
				if (!hit) {
					humanTurn = false;
					cout << "\nPress Enter to continue...";
					cin.ignore();
					cin.get();
				}
			}
			else {
				cout << "COMPUTER'S TURN\n";
				cout << "===================================\n";

				bool hit = computer.makeMove();

				if (playerBoard.allShipsSunk()) {
					cout << "\n=======================================\n";
					cout << "GAME OVER! COMPUTER WINS!\n";
					cout << "\n=======================================\n";
					gameRunning = false;
					break;
				}
				if (!hit) {
					humanTurn = true;
					cout << "\nPress Enter to continue...";
					cin.ignore();
					cin.get();
				}
			}
		}
		cout << "\n=== FINAL BOARDS ===\n";
		cout << "\nYour board:\n";
		playerBoard.print(false);
		cout << "\nComputer's board(revealed):\n";
		computerBoard.print(false);
	}
};


int main() {
	Game game;
	game.start();


	//cin.get();
	return 0;
}