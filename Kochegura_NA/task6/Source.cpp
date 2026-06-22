#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <limits>
#include <algorithm>
#ifdef _WIN32
#include <windows.h>
#undef max
#undef min
#endif
using namespace std;
void setConsoleColor(int textColor, int bgColor) {
#ifdef _WIN32
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    int color = (bgColor << 4) | textColor;
    SetConsoleTextAttribute(hConsole, color);
#else
    cout << "\033[" << (30 + textColor) << ";" << (40 + bgColor) << "m";
#endif
}
void resetConsoleColor() {
#ifdef _WIN32
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, 7);
#else
    cout << "\033[0m";
#endif
}
void drawShipArt() {
    cout << "\n";
    cout << "                    /\\\n";
    cout << "                   /  \\\n";
    cout << "                  /    \\\n";
    cout << "                 /      \\\n";
    cout << "                /        \\\n";
    cout << "               /__________\\\n";
    cout << "               |  OCEAN   |\n";
    cout << "               |  BATTLE  |\n";
    cout << "               |__________|\n";
    cout << "               |  [][][]  |\n";
    cout << "               |__________|\n";
    cout << "                    ||\n";
    cout << "                    ||\n";
    cout << "                   (||)\n";
    cout << "                   (||)\n";
    cout << "                    ()\n";
    cout << "\n";
}
void showMainMenu() {
    cout << "========================================\n";
    cout << "              BATTLESHIP\n";
    cout << "========================================\n";
    drawShipArt();
    cout << "1. Start (normal mode)\n";
    cout << "2. CHEAT MODE (with cheat codes!)\n";
    cout << "3. Exit\n";
    cout << "----------------------------------------\n";
    cout << "Your choice: ";
}
void showCheatCodesList() {
    cout << "\n========================================\n";
    cout << "           CHEAT CODES (CHEAT MODE)\n";
    cout << "========================================\n";
    cout << "Type these instead of coordinates:\n\n";
    setConsoleColor(10, 0);
    cout << "  bomb X Y";
    resetConsoleColor();
    cout << " - Explodes 3x3 area around chosen cell\n";
    cout << "         Example: bomb A5\n\n";
    setConsoleColor(10, 0);
    cout << "  radar  ";
    resetConsoleColor();
    cout << " - Reveals ALL enemy ships on the map\n\n";
    setConsoleColor(10, 0);
    cout << "  rocket ";
    resetConsoleColor();
    cout << " - Homing missile, guaranteed hit\n\n";
    setConsoleColor(10, 0);
    cout << "  drone  ";
    resetConsoleColor();
    cout << " - Reveals ONE random enemy ship cell\n\n";
    cout << "----------------------------------------\n";
    cout << "========================================\n";
}
enum CellState { EMPTY, SHIP, HIT, MISS };
class GameField {
private:
    CellState grid[10][10];
public:
    GameField() {
        for (int i = 0; i < 10; i++)
            for (int j = 0; j < 10; j++)
                grid[i][j] = EMPTY;
    }
    bool canPlaceShip(int row, int col, int len, int direction) {
        if (direction == 0) {
            if (col + len > 10) return false;
            for (int i = -1; i <= len; i++) {
                for (int dr = -1; dr <= 1; dr++) {
                    int r = row + dr;
                    int c = col + i;
                    if (r >= 0 && r < 10 && c >= 0 && c < 10)
                        if (grid[r][c] == SHIP) return false;
                }
            }
        }
        else {
            if (row + len > 10) return false;
            for (int i = -1; i <= len; i++) {
                for (int dc = -1; dc <= 1; dc++) {
                    int r = row + i;
                    int c = col + dc;
                    if (r >= 0 && r < 10 && c >= 0 && c < 10)
                        if (grid[r][c] == SHIP) return false;
                }
            }
        }
        return true;
    }
    void placeShip(int row, int col, int len, int direction) {
        if (direction == 0)
            for (int i = 0; i < len; i++) grid[row][col + i] = SHIP;
        else
            for (int i = 0; i < len; i++) grid[row + i][col] = SHIP;
    }
    bool shoot(int row, int col) {
        if (row < 0 || row >= 10 || col < 0 || col >= 10) return false;
        if (grid[row][col] == SHIP) {
            grid[row][col] = HIT;
            return true;
        }
        else if (grid[row][col] == EMPTY) {
            grid[row][col] = MISS;
            return false;
        }
        return false;
    }
    int bomb(int row, int col) {
        int hits = 0;
        for (int dr = -1; dr <= 1; dr++) {
            for (int dc = -1; dc <= 1; dc++) {
                int r = row + dr;
                int c = col + dc;
                if (r >= 0 && r < 10 && c >= 0 && c < 10) {
                    if (grid[r][c] == SHIP) {
                        grid[r][c] = HIT;
                        hits++;
                    }
                    else if (grid[r][c] == EMPTY) {
                        grid[r][c] = MISS;
                    }
                }
            }
        }
        return hits;
    }
    vector<pair<int, int>> getAliveShips() {
        vector<pair<int, int>> ships;
        for (int i = 0; i < 10; i++)
            for (int j = 0; j < 10; j++)
                if (grid[i][j] == SHIP)
                    ships.push_back({ i, j });
        return ships;
    }
    bool allShipsSunk() {
        for (int i = 0; i < 10; i++)
            for (int j = 0; j < 10; j++)
                if (grid[i][j] == SHIP) return false;
        return true;
    }
    void display(bool showShips) {
        cout << "  1 2 3 4 5 6 7 8 9 10\n";
        for (int i = 0; i < 10; i++) {
            cout << char('A' + i) << " ";
            for (int j = 0; j < 10; j++) {
                if (grid[i][j] == SHIP && showShips) {
                    setConsoleColor(10, 0); cout << "# ";
                }
                else if (grid[i][j] == HIT) {
                    setConsoleColor(12, 0); cout << "X ";
                }
                else if (grid[i][j] == MISS) {
                    setConsoleColor(14, 0); cout << "O ";
                }
                else {
                    setConsoleColor(7, 0); cout << ". ";
                }
                resetConsoleColor();
            }
            cout << endl;
        }
    }
    void displayCheat() {
        cout << "  1 2 3 4 5 6 7 8 9 10\n";
        for (int i = 0; i < 10; i++) {
            cout << char('A' + i) << " ";
            for (int j = 0; j < 10; j++) {
                if (grid[i][j] == SHIP) {
                    setConsoleColor(10, 0); cout << "# ";
                }
                else if (grid[i][j] == HIT) {
                    setConsoleColor(12, 0); cout << "X ";
                }
                else if (grid[i][j] == MISS) {
                    setConsoleColor(14, 0); cout << "O ";
                }
                else {
                    setConsoleColor(7, 0); cout << ". ";
                }
                resetConsoleColor();
            }
            cout << endl;
        }
    }
    CellState getState(int row, int col) { return grid[row][col]; }
    void clear() {
        for (int i = 0; i < 10; i++)
            for (int j = 0; j < 10; j++)
                grid[i][j] = EMPTY;
    }
    void autoPlaceShips() {
        clear();
        int ships[] = { 4, 3, 3, 2, 2, 2, 1, 1, 1, 1 };
        for (int len : ships) {
            bool placed = false;
            while (!placed) {
                int row = rand() % 10;
                int col = rand() % 10;
                int dir = rand() % 2;
                if (canPlaceShip(row, col, len, dir)) {
                    placeShip(row, col, len, dir);
                    placed = true;
                }
            }
        }
    }
};
class Player {
protected:
    GameField myField;
    GameField enemyField;
    string name;
public:
    Player(string n) : name(n) {}
    virtual ~Player() {}
    virtual void placeShips() = 0;
    virtual pair<int, int> makeMove() = 0;
    GameField& getMyField() { return myField; }
    GameField& getEnemyField() { return enemyField; }
    bool shootAt(Player& opponent, int row, int col) {
        bool hit = opponent.getMyField().shoot(row, col);
        enemyField.shoot(row, col);
        return hit;
    }
    int bombAt(Player& opponent, int row, int col) {
        int hits = opponent.getMyField().bomb(row, col);
        for (int dr = -1; dr <= 1; dr++)
            for (int dc = -1; dc <= 1; dc++) {
                int r = row + dr;
                int c = col + dc;
                if (r >= 0 && r < 10 && c >= 0 && c < 10)
                    enemyField.shoot(r, c);
            }
        return hits;
    }
    bool hasWonAgainst(Player& opponent) { return opponent.getMyField().allShipsSunk(); }
    string getName() { return name; }
    void autoPlaceMyShips() { myField.autoPlaceShips(); }
};
class ComputerPlayer : public Player {
private:
    bool used[10][10];
public:
    ComputerPlayer(string n) : Player(n) {
        for (int i = 0; i < 10; i++)
            for (int j = 0; j < 10; j++) used[i][j] = false;
    }
    void placeShips() override {
        myField.clear();
        for (int i = 0; i < 10; i++)
            for (int j = 0; j < 10; j++) used[i][j] = false;
        int ships[] = { 4, 3, 3, 2, 2, 2, 1, 1, 1, 1 };
        for (int len : ships) {
            bool placed = false;
            while (!placed) {
                int row = rand() % 10;
                int col = rand() % 10;
                int dir = rand() % 2;
                if (myField.canPlaceShip(row, col, len, dir)) {
                    myField.placeShip(row, col, len, dir);
                    placed = true;
                }
            }
        }
    }
    pair<int, int> makeMove() override {
        vector<pair<int, int>> available;
        for (int i = 0; i < 10; i++)
            for (int j = 0; j < 10; j++)
                if (!used[i][j]) available.push_back({ i, j });
        int idx = rand() % available.size();
        int row = available[idx].first;
        int col = available[idx].second;
        used[row][col] = true;
        return { row, col };
    }
};
class HumanPlayer : public Player {
public:
    HumanPlayer(string n) : Player(n) {}
    void placeShips() override {}
    bool tryPlaceShip(int len, int row, int col, int direction) {
        if (myField.canPlaceShip(row, col, len, direction)) {
            myField.placeShip(row, col, len, direction);
            return true;
        }
        return false;
    }
    bool isValidMove(int row, int col) {
        if (row < 0 || row >= 10 || col < 0 || col >= 10) return false;
        CellState state = enemyField.getState(row, col);
        return (state != HIT && state != MISS);
    }
    pair<int, int> makeMove() override { return { -1, -1 }; }
    GameField& getMyFieldForDisplay() { return myField; }
    GameField& getEnemyFieldForDisplay() { return enemyField; }
};
class BattleshipGame {
private:
    HumanPlayer human;
    ComputerPlayer computer;
    bool humanTurn;
    bool cheatMode;
    vector<pair<int, int>> computerMoves;
public:
    BattleshipGame(bool cheat) : human("Player"), computer("Computer"), cheatMode(cheat) {
        srand(time(0));
        humanTurn = true;
    }
    void placeComputerShips() { computer.placeShips(); }
    bool tryPlaceHumanShip(int len, int row, int col, int dir) { return human.tryPlaceShip(len, row, col, dir); }
    bool isHumanMoveValid(int row, int col) { return human.isValidMove(row, col); }
    bool makeHumanMove(int row, int col) { return human.shootAt(computer, row, col); }
    int makeHumanBomb(int row, int col) { return human.bombAt(computer, row, col); }
    pair<int, int> getRandomEnemyShip() {
        vector<pair<int, int>> ships = computer.getMyField().getAliveShips();
        if (ships.empty()) return { -1, -1 };
        int idx = rand() % ships.size();
        return ships[idx];
    }
    bool makeHomingRocket() {
        pair<int, int> target = getRandomEnemyShip();
        if (target.first == -1) return false;
        return makeHumanMove(target.first, target.second);
    }
    void revealFullRadar() {
        cout << "\n";
        setConsoleColor(10, 0);
        cout << "=== RADAR ACTIVE ===\n";
        resetConsoleColor();
        computer.getMyField().displayCheat();
    }
    void revealDrone() {
        vector<pair<int, int>> ships = computer.getMyField().getAliveShips();
        if (ships.empty()) {
            cout << "No ships left to reveal!\n";
            return;
        }
        int idx = rand() % ships.size();
        pair<int, int> target = ships[idx];
        cout << "\n";
        setConsoleColor(10, 0);
        cout << "=== DRONE REVEALS ===\n";
        resetConsoleColor();
        cout << "Enemy ship found at: ";
        setConsoleColor(12, 0);
        cout << char('A' + target.first) << target.second + 1 << endl;
        resetConsoleColor();
    }
    bool makeComputerMove() {
        pair<int, int> move = computer.makeMove();
        bool hit = computer.shootAt(human, move.first, move.second);
        computerMoves.push_back(move);
        return hit;
    }
    pair<int, int> getLastComputerMove() {
        if (computerMoves.empty()) return { -1, -1 };
        return computerMoves.back();
    }
    bool isHumanTurn() { return humanTurn; }
    void switchTurn() { humanTurn = !humanTurn; }
    bool isHumanWin() { return human.hasWonAgainst(computer); }
    bool isComputerWin() { return computer.hasWonAgainst(human); }
    GameField& getHumanMyField() { return human.getMyFieldForDisplay(); }
    GameField& getHumanEnemyField() { return human.getEnemyFieldForDisplay(); }
    GameField& getComputerField() { return computer.getMyField(); }
    bool isCheatMode() { return cheatMode; }
    void autoPlaceHumanShips() { human.autoPlaceMyShips(); }
};
void displayBothFields(GameField& myField, GameField& computerField, bool cheatMode) {
    cout << "\n";
    cout << "=== YOUR FIELD (left) ==============   === ENEMY FIELD (right) ===\n";
    cout << "  1 2 3 4 5 6 7 8 9 10                       1 2 3 4 5 6 7 8 9 10\n";
    for (int i = 0; i < 10; i++) {
        cout << char('A' + i) << " ";
        for (int j = 0; j < 10; j++) {
            CellState state = myField.getState(i, j);
            if (state == SHIP) { setConsoleColor(10, 0); cout << "# "; }
            else if (state == HIT) { setConsoleColor(12, 0); cout << "X "; }
            else if (state == MISS) { setConsoleColor(14, 0); cout << "O "; }
            else { setConsoleColor(7, 0); cout << ". "; }
            resetConsoleColor();
        }
        cout << "                     ";
        cout << char('A' + i) << " ";
        for (int j = 0; j < 10; j++) {
            CellState state = computerField.getState(i, j);
            if (state == HIT) { setConsoleColor(12, 0); cout << "X "; }
            else if (state == MISS) { setConsoleColor(14, 0); cout << "O "; }
            else if (state == SHIP && false) { setConsoleColor(10, 0); cout << "# "; }
            else { setConsoleColor(7, 0); cout << ". "; }
            resetConsoleColor();
        }
        cout << endl;
    }
}
void displayPlacementMessage(int len) {
    if (len == 1) cout << "\nShip length 1. Enter coordinates (A5): ";
    else cout << "\nShip length " << len << ". Enter (A5 H): ";
}
void displayHitMessage(bool hit, bool isPlayer, int bombHits = 0) {
    if (isPlayer) {
        if (bombHits > 0) {
            setConsoleColor(12, 0);
            cout << "BOMB! Hit " << bombHits << " ship cells!\n";
            resetConsoleColor();
        }
        else if (hit) {
            setConsoleColor(12, 0); cout << "HIT!\n"; resetConsoleColor();
        }
        else {
            setConsoleColor(14, 0); cout << "MISS!\n"; resetConsoleColor();
        }
    }
    else {
        if (hit) {
            setConsoleColor(12, 0); cout << "COMPUTER HIT!\n"; resetConsoleColor();
        }
        else {
            setConsoleColor(14, 0); cout << "COMPUTER MISS!\n"; resetConsoleColor();
        }
    }
}
void displayComputerMove(int row, int col) { cout << "Computer shoots at " << char('A' + row) << col + 1 << endl; }
void displayWinMessage(bool playerWin) {
    cout << "\n========================================\n";
    if (playerWin) { drawShipArt(); setConsoleColor(10, 0); cout << "            YOU WIN!\n"; }
    else { setConsoleColor(12, 0); cout << "         COMPUTER WINS!\n"; }
    resetConsoleColor();
    cout << "========================================\n";
}
void displayCheatInfo(bool cheatMode) {
    if (cheatMode) {
        cout << "\n!!! CHEAT MODE ACTIVATED !!!\n";
        cout << "Type 'radar', 'drone', 'rocket', or 'bomb A5'!\n\n";
    }
}
void displayEnemyShips(GameField& computerField) {
    cout << "\n=== ENEMY SHIPS PLACEMENT ===\n";
    computerField.displayCheat();
    cout << "================================\n";
}
void displayPlacementComplete() { cout << "\nPlacement complete!\n"; }
void displayGameStart() { cout << "\n=== GAME START ===\nFirst player: YOU\n"; }
void displayTurnMessage(bool isPlayerTurn) {
    cout << "\n----------------------------------------\n";
    if (isPlayerTurn) cout << "YOUR TURN\n";
    else cout << "COMPUTER TURN\n";
}
void displayInvalidMoveMessage() { cout << "You already shot there!\n"; }
void displayInvalidCoordinatesMessage() { cout << "Error! Use A-J and 1-10\n"; }
void displayCantPlaceMessage() { cout << "Can't place here!\n"; }
void displayCheatCommandMessage(const string& cmd) { cout << "Cheat code activated: " << cmd << "!\n"; }
int main() {
    int choice;
    setConsoleColor(7, 0);
    do {
        showMainMenu();
        cin >> choice;
        switch (choice) {
        case 1:
        case 2: {
            cin.ignore();
            bool cheat = (choice == 2);
            BattleshipGame game(cheat);
            displayCheatInfo(cheat);
            cout << "\n=== PLACE YOUR SHIPS ===\n";
            cout << "Format: A5 H (coordinates and orientation H-horizontal V-vertical)\n";
            cout << "For 1-cell ships just enter coordinates (A5)\n";
            cout << "Type 'auto' for automatic placement\n\n";
            string autoChoice;
            cout << "Auto placement? (yes/no): ";
            cin >> autoChoice;
            if (autoChoice == "yes" || autoChoice == "y" || autoChoice == "auto") {
                game.autoPlaceHumanShips();
                cout << "\nShips placed automatically!\n";
                game.getHumanMyField().display(true);
            }
            else {
                int ships[] = { 4, 3, 3, 2, 2, 2, 1, 1, 1, 1 };
                for (int len : ships) {
                    game.getHumanMyField().display(true);
                    bool placed = false;
                    while (!placed) {
                        displayPlacementMessage(len);
                        if (len == 1) {
                            char rowChar;
                            int col;
                            cin >> rowChar >> col;
                            int row = toupper(rowChar) - 'A';
                            col--;
                            if (row < 0 || row >= 10 || col < 0 || col >= 10) {
                                displayInvalidCoordinatesMessage();
                                continue;
                            }
                            if (game.tryPlaceHumanShip(len, row, col, 0)) placed = true;
                            else displayCantPlaceMessage();
                        }
                        else {
                            char rowChar, orient;
                            int col;
                            cin >> rowChar >> col >> orient;
                            int row = toupper(rowChar) - 'A';
                            col--;
                            orient = toupper(orient);
                            if (row < 0 || row >= 10 || col < 0 || col >= 10) {
                                displayInvalidCoordinatesMessage();
                                continue;
                            }
                            int dir = (orient == 'H') ? 0 : 1;
                            if (game.tryPlaceHumanShip(len, row, col, dir)) placed = true;
                            else displayCantPlaceMessage();
                        }
                    }
                }
                displayPlacementComplete();
                game.getHumanMyField().display(true);
            }
            game.placeComputerShips();
            displayGameStart();
            if (cheat) displayEnemyShips(game.getComputerField());
            bool gameRunning = true;
            while (gameRunning) {
                displayTurnMessage(game.isHumanTurn());
                if (game.isHumanTurn()) {
                    displayBothFields(game.getHumanMyField(), game.getComputerField(), cheat);
                    bool validMove = false;
                    int row = -1, col = -1;
                    string input;
                    while (!validMove && gameRunning) {
                        cout << "\nEnter coordinates or cheat code: ";
                        cin >> input;
                        if (cheat && input == "radar") {
                            displayCheatCommandMessage("RADAR");
                            game.revealFullRadar();
                            displayBothFields(game.getHumanMyField(), game.getComputerField(), cheat);
                            continue;
                        }
                        else if (cheat && input == "drone") {
                            displayCheatCommandMessage("DRONE");
                            game.revealDrone();
                            continue;
                        }
                        else if (cheat && input == "rocket") {
                            displayCheatCommandMessage("HOMING ROCKET");
                            bool hit = game.makeHomingRocket();
                            displayHitMessage(hit, true);
                            displayBothFields(game.getHumanMyField(), game.getComputerField(), cheat);
                            if (game.isHumanWin()) {
                                displayWinMessage(true);
                                gameRunning = false;
                                break;
                            }
                            if (!hit) game.switchTurn();
                            validMove = true;
                            break;
                        }
                        else if (cheat && input == "bomb") {
                            char rowChar;
                            cin >> rowChar >> col;
                            row = toupper(rowChar) - 'A';
                            col--;
                            if (row >= 0 && row < 10 && col >= 0 && col < 10) {
                                displayCheatCommandMessage("BOMB");
                                int hits = game.makeHumanBomb(row, col);
                                displayHitMessage(true, true, hits);
                                displayBothFields(game.getHumanMyField(), game.getComputerField(), cheat);
                                if (game.isHumanWin()) {
                                    displayWinMessage(true);
                                    gameRunning = false;
                                    break;
                                }
                                game.switchTurn();
                                validMove = true;
                                break;
                            }
                            else {
                                displayInvalidCoordinatesMessage();
                                continue;
                            }
                        }
                        else {
                            if (input.length() < 2) {
                                displayInvalidCoordinatesMessage();
                                continue;
                            }
                            char firstChar = input[0];
                            if ((firstChar >= 'A' && firstChar <= 'J') || (firstChar >= 'a' && firstChar <= 'j')) {
                                row = toupper(firstChar) - 'A';
                                string numPart = input.substr(1);
                                bool isNumber = true;
                                for (char c : numPart)
                                    if (!isdigit(c)) { isNumber = false; break; }
                                if (!isNumber) {
                                    displayInvalidCoordinatesMessage();
                                    continue;
                                }
                                col = stoi(numPart) - 1;
                            }
                            else {
                                displayInvalidCoordinatesMessage();
                                continue;
                            }
                            if (row < 0 || row >= 10 || col < 0 || col >= 10) {
                                displayInvalidCoordinatesMessage();
                                continue;
                            }
                            if (!game.isHumanMoveValid(row, col)) {
                                displayInvalidMoveMessage();
                                continue;
                            }
                            validMove = true;
                        }
                    }
                    if (!gameRunning) break;
                    if (row != -1 && col != -1 && input != "rocket" && input != "bomb") {
                        bool hit = game.makeHumanMove(row, col);
                        displayHitMessage(hit, true);
                        displayBothFields(game.getHumanMyField(), game.getComputerField(), cheat);
                        if (game.isHumanWin()) {
                            displayWinMessage(true);
                            break;
                        }
                        if (!hit) game.switchTurn();
                    }
                }
                else {
                    bool hit = game.makeComputerMove();
                    pair<int, int> move = game.getLastComputerMove();
                    displayComputerMove(move.first, move.second);
                    displayHitMessage(hit, false);
                    if (game.isComputerWin()) {
                        displayWinMessage(false);
                        break;
                    }
                    if (!hit) game.switchTurn();
                }
            }
            cout << "\nPress Enter to continue...";
            cin.ignore((numeric_limits<streamsize>::max)(), '\n');
            cin.get();
            break;
        }
        case 3:
            cout << "\nThanks for playing!\n";
            break;
        default:
            cout << "\nWrong choice!\n";
            cout << "\nPress Enter to continue...";
            cin.ignore((numeric_limits<streamsize>::max)(), '\n');
            cin.get();
            break;
        }
    } while (choice != 3);
    resetConsoleColor();
    return 0;
}