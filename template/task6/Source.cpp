#include <iostream>
#include <vector>
#include <deque>
#include <cstdlib>
#include <ctime>
#include <conio.h> 
#include <windows.h> 

using namespace std;

enum Direction {
    UP,
    DOWN,
    LEFT,
    RIGHT
};

class Cell {
private:
    int x, y;
public:
    Cell(int x = 0, int y = 0) : x(x), y(y) {}

    int getX() const { return x; }
    int getY() const { return y; }

    bool operator==(const Cell& other) const {
        return x == other.x && y == other.y;
    }
};

class Snake {
private:
    deque<Cell> body;      
    Direction direction;    
    int targetLength;      

public:
    Snake(int startX, int startY, int targetLen) : targetLength(targetLen) {
        for (int i = 0; i < 5; i++) {
            body.push_back(Cell(startX + i, startY));
        }
        direction = LEFT;
    }

    Cell getHead() const {
        return body.front();
    }

    Cell getTail() const {
        return body.back();
    }

    Direction getDirection() const {
        return direction;
    }

    void setDirection(Direction newDir) {
        if ((direction == UP && newDir == DOWN) ||
            (direction == DOWN && newDir == UP) ||
            (direction == LEFT && newDir == RIGHT) ||
            (direction == RIGHT && newDir == LEFT)) {
            return;
        }
        direction = newDir;
    }

    void move(bool grow) {
        Cell newHead = getHead();

        switch (direction) {
        case UP:    newHead = Cell(getHead().getX(), getHead().getY() - 1); break;
        case DOWN:  newHead = Cell(getHead().getX(), getHead().getY() + 1); break;
        case LEFT:  newHead = Cell(getHead().getX() - 1, getHead().getY()); break;
        case RIGHT: newHead = Cell(getHead().getX() + 1, getHead().getY()); break;
        }

        body.push_front(newHead);

        if (!grow) {
            body.pop_back();
        }
    }

    bool checkSelfCollision() const {
        Cell head = getHead();
        for (size_t i = 1; i < body.size(); i++) {
            if (head == body[i]) {
                return true;
            }
        }
        return false;
    }

    int getLength() const {
        return body.size();
    }

    int getTargetLength() const {
        return targetLength;
    }

    bool isWin() const {
        return body.size() >= targetLength;
    }

    bool occupies(const Cell& cell) const {
        for (const auto& segment : body) {
            if (segment == cell) {
                return true;
            }
        }
        return false;
    }

    const deque<Cell>& getBody() const {
        return body;
    }
};

class Field {
private:
    int width;     
    int height;    
    Cell food;     

public:
    Field(int w, int h) : width(w), height(h) {}

    int getWidth() const { return width; }
    int getHeight() const { return height; }

    bool isWall(const Cell& cell) const {
        int x = cell.getX();
        int y = cell.getY();
        return (x == 0 || x == width + 1 || y == 0 || y == height + 1);
    }

    Cell generateRandomFreeCell(const Snake& snake) {
        vector<Cell> freeCells;

        for (int y = 1; y <= height; y++) {
            for (int x = 1; x <= width; x++) {
                Cell cell(x, y);
                if (!snake.occupies(cell)) {
                    freeCells.push_back(cell);
                }
            }
        }

        if (freeCells.empty()) {
            return Cell(-1, -1);
        }

        int randomIndex = rand() % freeCells.size();
        return freeCells[randomIndex];
    }

    void setFood(const Cell& newFood) {
        food = newFood;
    }

    Cell getFood() const {
        return food;
    }

    void spawnFood(const Snake& snake) {
        food = generateRandomFreeCell(snake);
    }

    bool isFoodEaten(const Cell& head) const {
        return head == food;
    }
};

class Game {
private:
    Field field;
    Snake snake;
    bool gameOver;
    bool gameWin;
    int speed;    

public:
    Game(int fieldWidth, int fieldHeight, int targetLength, int gameSpeed = 100)
        : field(fieldWidth, fieldHeight),
        snake(rand() % fieldWidth + 1, rand() % fieldHeight + 1, targetLength),
        gameOver(false), gameWin(false), speed(gameSpeed) {

        while (snake.getHead().getX() <= 1 || snake.getHead().getX() >= fieldWidth ||
            snake.getHead().getY() <= 1 || snake.getHead().getY() >= fieldHeight) {
            snake = Snake(rand() % fieldWidth + 1, rand() % fieldHeight + 1, targetLength);
        }

        field.spawnFood(snake);
    }

    void handleInput() {
        if (_kbhit()) {
            int key = _getch();
            if (key == 224) { 
                key = _getch();
                switch (key) {
                case 72: snake.setDirection(UP); break;
                case 80: snake.setDirection(DOWN); break;
                case 75: snake.setDirection(LEFT); break;
                case 77: snake.setDirection(RIGHT); break;
                }
            }
        }
    }

    void update() {
        if (gameOver || gameWin) return;

        Cell head = snake.getHead();
        bool grow = field.isFoodEaten(head);

        snake.move(grow);

        head = snake.getHead();
        if (field.isWall(head)) {
            gameOver = true;
            return;
        }

        if (snake.checkSelfCollision()) {
            gameOver = true;
            return;
        }

        if (grow) {
            field.spawnFood(snake);
            if (snake.isWin()) {
                gameWin = true;
            }
        }
    }

    void render() const {
        system("cls"); 

        for (int i = 0; i <= field.getWidth() + 1; i++) {
            cout << "#";
        }
        cout << endl;

        for (int y = 1; y <= field.getHeight(); y++) {
            cout << "#";

            for (int x = 1; x <= field.getWidth(); x++) {
                Cell cell(x, y);

                if (snake.occupies(cell)) {
                    if (cell == snake.getHead()) {
                        cout << "@"; 
                    }
                    else {
                        cout << "O"; 
                    }
                }
                else if (cell == field.getFood()) {
                    cout << "*"; 
                }
                else {
                    cout << " ";
                }
            }

            cout << "#" << endl;
        }

        for (int i = 0; i <= field.getWidth() + 1; i++) {
            cout << "#";
        }
        cout << endl;

        cout << "Длина змейки: " << snake.getLength()
            << " / " << snake.getTargetLength() << endl;

        if (gameWin) {
            cout << "ПОБЕДА! Вы достигли целевой длины!" << endl;
        }
        else if (gameOver) {
            cout << "ПРОИГРЫШ! Игра окончена!" << endl;
        }
        else {
            cout << "Используйте стрелки для управления" << endl;
        }
    }

    bool isGameOver() const {
        return gameOver;
    }

    bool isGameWin() const {
        return gameWin;
    }

    int getSpeed() const {
        return speed;
    }

    void run() {
        while (!gameOver && !gameWin) {
            handleInput();      
            update();          
            render();          

            if (!gameOver && !gameWin) {
                Sleep(speed);  
            }
        }

        render();
    }
};

int main() {
    srand(time(0)); 

    int fieldWidth = 20;      
    int fieldHeight = 15;     
    int targetLength = 15;    
    int gameSpeed = 100;     

    cout << "=== ИГРА ЗМЕЙКА ===" << endl;
    cout << "Правила:" << endl;
    cout << "- Управляйте змейкой с помощью стрелок" << endl;
    cout << "- Ешьте пищу (*), чтобы расти" << endl;
    cout << "- Не врезайтесь в стены (#) и в свое тело" << endl;
    cout << "- Достигните длины " << targetLength << " для победы" << endl;
    cout << "Нажмите любую клавишу для начала..." << endl;
    _getch();

    Game game(fieldWidth, fieldHeight, targetLength, gameSpeed);
    game.run();

    cout << "Нажмите любую клавишу для выхода..." << endl;
    _getch();

    return 0;
}