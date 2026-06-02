#include <iostream>
#include <deque>
#include <cstdlib>
#include <ctime>
#include <conio.h>
#include <windows.h>

using namespace std;

class Point {
public:
    int x, y;

    Point(int _x = 0, int _y = 0) {
        x = _x;
        y = _y;
    }

    bool operator==(const Point& other) const {
        return x == other.x && y == other.y;
    }
};

enum Direction {
    UP,
    DOWN,
    LEFT,
    RIGHT
};

class Snake {
private:
    deque<Point> body;
    Direction dir;

public:
    Snake(int startX, int startY) {
        dir = LEFT;

        // √олова слева, хвост справа
        for (int i = 0; i < 5; i++) {
            body.push_back(Point(startX + i, startY));
        }
    }

    Point getHead() const {
        return body.front();
    }

    int getLength() const {
        return body.size();
    }

    Direction getDirection() const {
        return dir;
    }

    void setDirection(Direction newDir) {
        if ((dir == UP && newDir == DOWN) ||
            (dir == DOWN && newDir == UP) ||
            (dir == LEFT && newDir == RIGHT) ||
            (dir == RIGHT && newDir == LEFT)) {
            return;
        }

        dir = newDir;
    }

    Point nextHead() const {
        Point head = getHead();

        if (dir == UP) head.y--;
        else if (dir == DOWN) head.y++;
        else if (dir == LEFT) head.x--;
        else if (dir == RIGHT) head.x++;

        return head;
    }

    void move(bool grow) {
        Point newHead = nextHead();
        body.push_front(newHead);

        if (!grow) {
            body.pop_back();
        }
    }

    bool contains(const Point& p) const {
        for (const Point& part : body) {
            if (part == p) return true;
        }
        return false;
    }

    bool headHitsBody() const {
        Point head = getHead();

        for (int i = 1; i < body.size(); i++) {
            if (body[i] == head) return true;
        }

        return false;
    }

    const deque<Point>& getBody() const {
        return body;
    }
};

class Food {
private:
    Point position;

public:
    Food() {}

    Point getPosition() const {
        return position;
    }

    void generate(int width, int height, const Snake& snake) {
        do {
            position.x = 1 + rand() % width;
            position.y = 1 + rand() % height;
        } while (snake.contains(position));
    }
};

class Game {
private:
    int width;
    int height;
    int targetLength;
    int speed;
    bool gameOver;
    bool win;

    Snake* snake;
    Food food;

public:
    Game(int n, int m, int target, int speedMs) {
        width = m;
        height = n;
        targetLength = target;
        speed = speedMs;
        gameOver = false;
        win = false;

        srand(time(0));

        int startX = 6 + rand() % (width - 6);
        int startY = 1 + rand() % height;

        snake = new Snake(startX, startY);
        food.generate(width, height, *snake);
    }

    ~Game() {
        delete snake;
    }

    void draw() {
        system("cls");

        for (int y = 0; y <= height + 1; y++) {
            for (int x = 0; x <= width + 1; x++) {
                Point current(x, y);

                if (x == 0 || y == 0 || x == width + 1 || y == height + 1) {
                    cout << "#";
                }
                else if (current == snake->getHead()) {
                    cout << "@"; // голова
                }
                else if (current == food.getPosition()) {
                    cout << "*"; // пища
                }
                else if (snake->contains(current)) {
                    cout << "o"; // тело
                }
                else {
                    cout << " ";
                }
            }
            cout << endl;
        }

        cout << "Length: " << snake->getLength()
            << " / " << targetLength << endl;
    }

    void input() {
        if (_kbhit()) {
            int key = _getch();

            if (key == 224) {
                key = _getch();

                if (key == 72) snake->setDirection(UP);
                else if (key == 80) snake->setDirection(DOWN);
                else if (key == 75) snake->setDirection(LEFT);
                else if (key == 77) snake->setDirection(RIGHT);
            }
        }
    }

    bool hitWall(const Point& p) const {
        return p.x <= 0 || p.y <= 0 || p.x > width || p.y > height;
    }

    void update() {
        Point next = snake->nextHead();

        if (hitWall(next) || snake->contains(next)) {
            gameOver = true;
            win = false;
            return;
        }

        bool eat = (next == food.getPosition());

        snake->move(eat);

        if (eat) {
            if (snake->getLength() >= targetLength) {
                gameOver = true;
                win = true;
                return;
            }

            food.generate(width, height, *snake);
        }
    }

    void run() {
        while (!gameOver) {
            draw();
            input();
            update();
            Sleep(speed);
        }

        draw();

        if (win) {
            cout << "YOU WIN!" << endl;
        }
        else {
            cout << "GAME OVER!" << endl;
        }
    }
};

int main() {
    int n, m, target;

    cout << "Enter field height N: ";
    cin >> n;

    cout << "Enter field width M: ";
    cin >> m;

    cout << "Enter target snake length: ";
    cin >> target;

    if (n < 5 || m < 10 || target <= 5) {
        cout << "Incorrect parameters!" << endl;
        return 0;
    }

    Game game(n, m, target, 200);
    game.run();

    return 0;
}
