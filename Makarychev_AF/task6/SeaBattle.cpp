#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>

using namespace std;

enum CellState {
    Empty,
    ship,
    Miss,
    Hit
};

class Cell {
private:
    CellState state;

public:
    Cell() {
        state = Empty;
    }

    CellState getState() const {
        return state;
    }

    void setState(CellState s) {
        state = s;
    }

    bool hasShip() const {
        return state == ship;
    }
};

class Ship {
private:
    int x, y;
    int length;
    bool vertical;
    int hits;

public:
    Ship(int _x = 0, int _y = 0, int _length = 1, bool _vertical = false) {
        x = _x;
        y = _y;
        length = _length;
        vertical = _vertical;
        hits = 0;
    }

    bool contains(int cx, int cy) const {
        for (int i = 0; i < length; i++) {
            int sx = x + (vertical ? 0 : i);
            int sy = y + (vertical ? i : 0);

            if (sx == cx && sy == cy)
                return true;
        }
        return false;
    }

    void hit() {
        hits++;
    }

    bool isKilled() const {
        return hits >= length;
    }
};

class Board {
private:
    vector<vector<Cell>> field;
    vector<Ship> ships;

public:
    Board() {
        field.resize(10, vector<Cell>(10));
    }

    bool inField(int x, int y) const {
        return x >= 0 && x < 10 && y >= 0 && y < 10;
    }

    bool canPlaceShip(int x, int y, int length, bool vertical) const {
        for (int i = 0; i < length; i++) {
            int nx = x + (vertical ? 0 : i);
            int ny = y + (vertical ? i : 0);

            if (!inField(nx, ny))
                return false;

            for (int dx = -1; dx <= 1; dx++) {
                for (int dy = -1; dy <= 1; dy++) {
                    int cx = nx + dx;
                    int cy = ny + dy;

                    if (inField(cx, cy) && field[cy][cx].hasShip())
                        return false;
                }
            }
        }

        return true;
    }

    bool placeShip(int x, int y, int length, bool vertical) {
        if (!canPlaceShip(x, y, length, vertical))
            return false;

        for (int i = 0; i < length; i++) {
            int nx = x + (vertical ? 0 : i);
            int ny = y + (vertical ? i : 0);

            field[ny][nx].setState(ship);
        }

        ships.push_back(Ship(x, y, length, vertical));
        return true;
    }

    void markShot(int x, int y, bool hit) {
        if (!inField(x, y))
            return;

        if (hit)
            field[y][x].setState(Hit);
        else
            field[y][x].setState(Miss);
    }

    bool shoot(int x, int y, int k = 0) {
        if (!inField(x, y))
            return false;

        CellState state = field[y][x].getState();

        if (state == ship) {
            field[y][x].setState(Hit);

            for (int i = 0; i < ships.size(); i++) {
                if (ships[i].contains(x, y)) {
                    ships[i].hit();

                    if (ships[i].isKilled())
                        cout << "Корабль уничтожен!\n";
                    else
                        cout << "Попадание!\n";

                    break;
                }
            }

            return true;
        }

        if (state == Empty) {
            field[y][x].setState(Miss);
            if (k) {
                cout << "Промах!\n";
            }
        }
        else
        {
            if (k) {
                cout << "Промах!\n";
            }
        }
        return false;
    }

    bool alreadyShot(int x, int y) const {
        if (!inField(x, y))
            return true;

        CellState state = field[y][x].getState();
        return state == Miss || state == Hit;
    }

    bool allShipsKilled() const {
        for (int i = 0; i < ships.size(); i++) {
            if (!ships[i].isKilled())
                return false;
        }
        return true;
    }

    void printOwn() const {
        cout << "  ";
        for (int x = 0; x < 10; x++) cout << x << " ";
        cout << endl;

        for (int y = 0; y < 10; y++) {
            cout << y << " ";

            for (int x = 0; x < 10; x++) {
                CellState s = field[y][x].getState();

                if (s == Empty) cout << ". ";
                else if (s == ship) cout << "S ";
                else if (s == Miss) cout << "* ";
                else if (s == Hit) cout << "X ";
            }

            cout << endl;
        }
    }

    void printEnemy() const {
        cout << "  ";
        for (int x = 0; x < 10; x++) cout << x << " ";
        cout << endl;

        for (int y = 0; y < 10; y++) {
            cout << y << " ";

            for (int x = 0; x < 10; x++) {
                CellState s = field[y][x].getState();

                if (s == Hit) cout << "X ";
                else if (s == Miss) cout << "* ";
                else cout << ". ";
            }

            cout << endl;
        }
    }
};

class Player {
private:
    Board ownBoard;
    Board shotBoard;
    bool computer;

public:
    Player(bool isComputer = false) {
        computer = isComputer;
    }

    Board& getOwnBoard() {
        return ownBoard;
    }

    Board& getShotBoard() {
        return shotBoard;
    }

    bool isComputer() const {
        return computer;
    }
};

class Game {
private:
    Player human;
    Player computer;

public:
    Game() : human(false), computer(true) {
        srand(time(0));
    }

    void autoPlace(Player& player) {
        int lengths[10] = { 4, 3, 3, 2, 2, 2, 1, 1, 1, 1 };

        for (int i = 0; i < 10; i++) {
            bool placed = false;

            while (!placed) {
                int x = rand() % 10;
                int y = rand() % 10;
                bool vertical = rand() % 2;

                placed = player.getOwnBoard().placeShip(x, y, lengths[i], vertical);
            }
        }
    }

    void manualPlace(Player& player) {
        int lengths[10] = { 4, 3, 3, 2, 2, 2, 1, 1, 1, 1 };

        cout << "Расстановка кораблей.\n";
        cout << "Координаты вводятся от 0 до 9.\n";
        cout << "Направление: 0 - горизонтально, 1 - вертикально.\n\n";

        for (int i = 0; i < 10; i++) {
            bool placed = false;

            while (!placed) {
                player.getOwnBoard().printOwn();

                int x, y, v;
                cout << "\nПоставьте корабль длины " << lengths[i] << endl;
                cout << "x y направление: ";
                cin >> x >> y >> v;

                placed = player.getOwnBoard().placeShip(x, y, lengths[i], v == 1);

                if (!placed)
                    cout << "Нельзя поставить корабль сюда!\n\n";
            }
        }
    }

    bool humanMove() {
        int x, y;

        cout << "\nВаше поле:\n";
        human.getOwnBoard().printOwn();

        cout << "\nПоле выстрелов:\n";
        human.getShotBoard().printEnemy();

        cout << "\nВаш выстрел x y: ";
        cin >> x >> y;

        while (!computer.getOwnBoard().inField(x, y) ||
            human.getShotBoard().alreadyShot(x, y)) {
            cout << "Некорректный выстрел. Введите снова x y: ";
            cin >> x >> y;
        }

        bool hit = computer.getOwnBoard().shoot(x, y, 1);

        human.getShotBoard().markShot(x, y, hit);

        return hit;
    }

    bool computerMove() {
        int x, y;

        do {
            x = rand() % 10;
            y = rand() % 10;
        } while (computer.getShotBoard().alreadyShot(x, y));

        cout << "\nКомпьютер стреляет: " << x << " " << y << endl;

        bool hit = human.getOwnBoard().shoot(x, y, 1);

        computer.getShotBoard().markShot(x, y, hit);

        return hit;
    }

    void run() {
        manualPlace(human);
        autoPlace(computer);

        cout << "\nИгра началась!\n";

        bool humanTurn = true;

        while (true) {
            if (humanTurn) {
                bool hit = humanMove();

                if (computer.getOwnBoard().allShipsKilled()) {
                    cout << "\nВы победили!\n";
                    break;
                }

                if (!hit)
                    humanTurn = false;
            }
            else {
                bool hit = computerMove();

                if (human.getOwnBoard().allShipsKilled()) {
                    cout << "\nКомпьютер победил!\n";
                    break;
                }

                if (!hit)
                    humanTurn = true;
            }
        }
    }
};

int main() {
    setlocale(LC_ALL, "Russian");
    Game game;
    game.run();

    return 0;
}