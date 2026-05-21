#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include <cstdlib>
#include <windows.h>

using namespace std;

const int BOARD_SIZE = 10;

enum CellState {
    EMPTY,
    SHIP,
    MISS,
    HIT,
    SUNK
};

struct Point {
    int r, c;
};

class Ship {
public:
    int size;
    vector<Point> coords;
    int hits = 0;

    Ship(int s) : size(s) {}

    bool isSunk() const {
        return hits >= size;
    }
};

class Board {
public:
    vector<vector<CellState>> grid;
    vector<Ship> ships;

    Board() {
        grid.assign(BOARD_SIZE, vector<CellState>(BOARD_SIZE, EMPTY));
    }

    bool canPlaceShip(int r, int c, int size, bool horizontal) {
        for (int i = 0; i < size; ++i) {
            int curr_r = r + (horizontal ? 0 : i);
            int curr_c = c + (horizontal ? i : 0);

            if (curr_r < 0 || curr_r >= BOARD_SIZE || curr_c < 0 || curr_c >= BOARD_SIZE)
                return false;

            for (int dr = -1; dr <= 1; ++dr) {
                for (int dc = -1; dc <= 1; ++dc) {
                    int nr = curr_r + dr;
                    int nc = curr_c + dc;
                    if (nr >= 0 && nr < BOARD_SIZE && nc >= 0 && nc < BOARD_SIZE) {
                        if (grid[nr][nc] == SHIP) return false;
                    }
                }
            }
        }
        return true;
    }

    bool placeShip(int size, int r, int c, bool horizontal) {
        if (!canPlaceShip(r, c, size, horizontal)) return false;

        Ship newShip(size);
        for (int i = 0; i < size; ++i) {
            int curr_r = r + (horizontal ? 0 : i);
            int curr_c = c + (horizontal ? i : 0);
            grid[curr_r][curr_c] = SHIP;
            newShip.coords.push_back({ curr_r, curr_c });
        }
        ships.push_back(newShip);
        return true;
    }

    void autoPlaceAllShips() {
        vector<int> shipSizes = { 4, 3, 3, 2, 2, 2, 1, 1, 1, 1 };
        for (int size : shipSizes) {
            bool placed = false;
            while (!placed) {
                int r = rand() % BOARD_SIZE;
                int c = rand() % BOARD_SIZE;
                bool horizontal = rand() % 2 == 0;
                placed = placeShip(size, r, c, horizontal);
            }
        }
    }

    int takeShot(int r, int c) {
        if (grid[r][c] == MISS || grid[r][c] == HIT || grid[r][c] == SUNK) {
            return -1;
        }

        if (grid[r][c] == EMPTY) {
            grid[r][c] = MISS;
            return 0;
        }

        if (grid[r][c] == SHIP) {
            grid[r][c] = HIT;
            for (auto& ship : ships) {
                for (auto& pt : ship.coords) {
                    if (pt.r == r && pt.c == c) {
                        ship.hits++;
                        if (ship.isSunk()) {
                            for (auto& p : ship.coords) {
                                grid[p.r][p.c] = SUNK;
                            }
                            return 2;
                        }
                        return 1;
                    }
                }
            }
        }
        return 0;
    }

    bool hasAliveShips() {
        for (const auto& ship : ships) {
            if (!ship.isSunk()) return true;
        }
        return false;
    }
};


class Player {
protected:
    string name;
public:
    Board myBoard;
    Board enemyBoard;

    Player(string n) : name(n) {}
    string getName() const { return name; }

    virtual Point makeMove() = 0;
    virtual ~Player() {}
};

class HumanPlayer : public Player {
public:
    HumanPlayer(string n) : Player(n) {}

    Point makeMove() override {
        int r, c;
        while (true) {
            cout << "Ваш ход (строка 0-9 и столбец 0-9 через пробел): ";
            if (cin >> r >> c) {
                if (r >= 0 && r < BOARD_SIZE && c >= 0 && c < BOARD_SIZE) {
                    return { r, c };
                }
            }
            else {
                cin.clear();
                cin.ignore(10000, '\n');
            }
            cout << "Некорректные координаты! Попробуйте снова.\n";
        }
    }
};

class ComputerPlayer : public Player {
public:
    ComputerPlayer(string n) : Player(n) {}

    Point makeMove() override {
        int r = rand() % BOARD_SIZE;
        int c = rand() % BOARD_SIZE;
        return { r, c };
    }
};

class Game {
private:
    HumanPlayer human;
    ComputerPlayer computer;

    char getCellChar(CellState state, bool hideShips) {
        switch (state) {
        case EMPTY: return '.';
        case SHIP:  return hideShips ? '.' : 'S';
        case MISS:  return '*';
        case HIT:   return 'X';
        case SUNK:  return '#';
        default:    return '.';
        }
    }

public:
    Game() : human("Игрок (Вы)"), computer("Компьютер") {
        srand(time(0));
    }

    void init() {
        human.myBoard.autoPlaceAllShips();
        computer.myBoard.autoPlaceAllShips();
    }

    void display() {
        cout << "\n    ЛЕВОЕ ПОЛЕ (ВАШЕ)         ПРАВОЕ ПОЛЕ (ВРАГ)\n";
        cout << "  0 1 2 3 4 5 6 7 8 9       0 1 2 3 4 5 6 7 8 9\n";
        for (int i = 0; i < BOARD_SIZE; ++i) {
            cout << i << " ";
            for (int j = 0; j < BOARD_SIZE; ++j) {
                cout << getCellChar(human.myBoard.grid[i][j], false) << " ";
            }
            cout << "    " << i << " ";
            for (int j = 0; j < BOARD_SIZE; ++j) {
                cout << getCellChar(human.enemyBoard.grid[i][j], false) << " ";
            }
            cout << "\n";
        }
        cout << endl;
    }

    void play() {
        init();
        bool humanTurn = true;

        while (human.myBoard.hasAliveShips() && computer.myBoard.hasAliveShips()) {
            if (humanTurn) {
                display();
                cout << "--- ХОД ИГРОКА ---\n";
                Point p = human.makeMove();

                int result = computer.myBoard.takeShot(p.r, p.c);
                if (result == -1) {
                    cout << "Вы уже стреляли туда! Повторите ход.\n";
                    continue;
                }
                else if (result == 0) {
                    cout << "Промах!\n";
                    human.enemyBoard.grid[p.r][p.c] = MISS;
                    humanTurn = false;
                }
                else {
                    if (result == 1) cout << "Попадание (Ранен)!\n";
                    if (result == 2) cout << "Ура! Корабль противника потоплен!\n";
                    human.enemyBoard.grid[p.r][p.c] = (result == 1) ? HIT : SUNK;
                }
            }
            else {
                Point p = computer.makeMove();
                int result = human.myBoard.takeShot(p.r, p.c);


                if (result == -1) continue;

                cout << "Компьютер стреляет в: (" << p.r << ", " << p.c << ") -> ";
                if (result == 0) {
                    cout << "Промах компьютера!\n";
                    computer.enemyBoard.grid[p.r][p.c] = MISS;
                    humanTurn = true;
                }
                else {
                    if (result == 1) cout << "Компьютер попал в ваш корабль!\n";
                    if (result == 2) cout << "Компьютер потопил ваш корабль! \n";
                    computer.enemyBoard.grid[p.r][p.c] = (result == 1) ? HIT : SUNK;
                }
            }
        }

        display();
        if (human.myBoard.hasAliveShips()) {
            cout << "ПОЗДРАВЛЯЕМ! Вы победили!\n";
        }
        else {
            cout << "ИГРА ОКОНЧЕНА. Компьютер победил.\n";
        }
    }
};

int main() {
    SetConsoleOutputCP(65001);
    system("chcp 65001 > nul");
    Game seaBattle;
    seaBattle.play();
    return 0;
}