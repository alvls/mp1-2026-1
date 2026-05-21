#include <iostream>
#include <cstdlib>
#include <ctime>

using namespace std;

const int SIZE = 10;

class Board {
    int grid[SIZE][SIZE];

    bool canPlace(int x, int y, int size, bool hor) {
        if (hor && y + size > SIZE) return false;
        if (!hor && x + size > SIZE) return false;

        int sx = x - 1;
        int sy = y - 1;
        int ex = hor ? x + 1 : x + size;
        int ey = hor ? y + size : y + 1;

        for (int i = sx; i <= ex; i++) {
            for (int j = sy; j <= ey; j++) {
                if (i >= 0 && i < SIZE && j >= 0 && j < SIZE) {
                    if (grid[i][j] == 1) return false;
                }
            }
        }
        return true;
    }

    void place(int x, int y, int size, bool hor) {
        for (int i = 0; i < size; i++) {
            if (hor) grid[x][y + i] = 1;
            else grid[x + i][y] = 1;
        }
    }

public:
    Board() {
        for (int i = 0; i < SIZE; i++)
            for (int j = 0; j < SIZE; j++)
                grid[i][j] = 0;
    }

    void autoPlace() {
        for (int i = 0; i < SIZE; i++)
            for (int j = 0; j < SIZE; j++)
                grid[i][j] = 0;

        int ships[] = { 4, 3, 3, 2, 2, 2, 1, 1, 1, 1 };

        for (int s = 0; s < 10; s++) {
            int size = ships[s];
            bool placed = false;

            while (!placed) {
                int x = rand() % SIZE;
                int y = rand() % SIZE;
                bool hor = rand() % 2;

                if (!canPlace(x, y, size, hor)) continue;

                place(x, y, size, hor);
                placed = true;
            }
        }
    }

    int shoot(int y, int x) {
        x--; y--;
        if (grid[x][y] == 1) { grid[x][y] = 2; return 1; }
        if (grid[x][y] == 0) { grid[x][y] = 3; return 0; }
        return -1;
    }

    bool allDead() {
        for (int i = 0; i < SIZE; i++)
            for (int j = 0; j < SIZE; j++)
                if (grid[i][j] == 1) return false;
        return true;
    }

    char getCell(int x, int y, bool hide) {
        int val = grid[x][y];
        if (hide && val == 1) return '.';
        if (val == 0) return '.';
        if (val == 1) return 'S';
        if (val == 2) return '*';
        if (val == 3) return 'o';
        return '.';
    }
};

class Player {
public:
    Board myBoard;
    Board enemyBoard;

    void placeShips() { myBoard.autoPlace(); }

    void markShot(int x, int y, int result) {
        if (result == 1) enemyBoard.shoot(x, y);
    }
};

class Human : public Player {
public:
    int makeMove(Player& enemy, int x, int y) {
        if (x < 1 || x > SIZE || y < 1 || y > SIZE) return -1;
        return enemy.myBoard.shoot(x, y);
    }
};

class Computer : public Player {
    bool shotHistory[SIZE][SIZE] = { {false} };
public:
    void makeMove(Player& enemy, int& outX, int& outY, int& result) {
        do {
            outX = rand() % SIZE + 1;
            outY = rand() % SIZE + 1;
        } while (shotHistory[outX - 1][outY - 1]);  

        shotHistory[outX - 1][outY - 1] = true;     
        result = enemy.myBoard.shoot(outX, outY);
    }
};
class Game {
    Human human;
    Computer computer;

public:
    Game() {
        srand(time(0));
        human.placeShips();
        computer.placeShips();
    }

    char getHumanCell(int x, int y, bool hide) { return human.myBoard.getCell(x, y, hide); }
    char getComputerCell(int x, int y, bool hide) { return computer.myBoard.getCell(x, y, hide); }

    int humanMove(int x, int y) {
        return human.makeMove(computer, x, y);
    }

    void computerMove(int& x, int& y, int& result) {
        computer.makeMove(human, x, y, result);
    }

    bool humanWins() { return computer.myBoard.allDead(); }
    bool computerWins() { return human.myBoard.allDead(); }
};

void showBoard(Game& game, bool isHuman, bool hide) {
    cout << "   ";
    for (int j = 1; j <= SIZE; j++) {
        if (j < 10) cout << j << "  ";
        else cout << j << " ";
    }
    cout << endl;

    for (int i = 0; i < SIZE; i++) {
        if (i + 1 < 10) cout << (i + 1) << "  ";
        else cout << (i + 1) << " ";

        for (int j = 0; j < SIZE; j++) {
            char c = isHuman ? game.getHumanCell(i, j, hide) : game.getComputerCell(i, j, hide);
            cout << c << "  ";
        }
        cout << endl;
    }
}

int main() {
    setlocale(LC_ALL, "Russian");

    cout << "=== МОРСКОЙ БОЙ ===\n";
    cout << "Координаты от 1 до 10\n\n";

    Game game;
    bool humanTurn = true;

    while (true) {
        if (humanTurn) {
            cout << "\n--- ВАШ ХОД ---\n";
            cout << "Ваши корабли:\n";
            showBoard(game, true, false);
            cout << "\nПоле врага:\n";
            showBoard(game, false, true);

            int x, y;
            cout << "Куда стрелять (x y): ";
            cin >> x >> y;

            int result = game.humanMove(x, y);

            if (result == -1) {
                cout << "Неверные координаты!\n";
                continue;
            }
            else if (result == 1) {
                cout << "Попадание!\n";
            }
            else if (result == 0) {
                cout << "Промах.\n";
                humanTurn = false;
            }
            else {
                cout << "Уже стреляли сюда.\n";
                continue;
            }

            if (game.humanWins()) {
                cout << "\n=== ПОБЕДА! ===\n";
                break;
            }
        }
        else {
            cout << "\n--- ХОД КОМПЬЮТЕРА ---\n";

            int x, y, result;
            game.computerMove(x, y, result);

            cout << "Компьютер стреляет (" << x << "," << y << ")\n";

            if (result == 1) {
                cout << "Компьютер попал!\n";
            }
            else if (result == 0) {
                cout << "Компьютер промахнулся.\n";
                humanTurn = true;
            }
            else {
                continue;
            }

            if (game.computerWins()) {
                cout << "\n=== ПОРАЖЕНИЕ ===\n";
                break;
            }
        }
    }
    cout << "\nИтоговое поле:\n";
    showBoard(game, true, false);
    system("pause");
    return 0;
}