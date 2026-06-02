#include <iostream>
#include <string>
#include <windows.h>


//доделать main (TO DO)

using namespace std;

class OneLineEditor {
    int fieldLength;
    int posX;
    int posY;
    string text;

public:

    // конструктор
    OneLineEditor(int len = 10, int x = 0, int y = 0) {
        fieldLength = len;
        posX = x;
        posY = y;
        text = "";
    }

    // деструктор
    ~OneLineEditor() {}

    void setLength(int len) {
        fieldLength = len;
    }

    int getLength() {
        return fieldLength;
    }

    void setPosition(int x, int y) {
        posX = x;
        posY = y;
    }

    void getPosition(int& x, int& y) {
        x = posX;
        y = posY;
    }

    // размещение редактора в консоли
    void draw() {
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        COORD pos;

        pos.X = posX;
        pos.Y = posY;

        SetConsoleCursorPosition(hConsole, pos);

        for (int i = 0; i < fieldLength; i++)
            cout << "_";
    }

    // ввод строки
    void input() {
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        COORD pos;
        pos.X = posX;
        pos.Y = posY;

        SetConsoleCursorPosition(hConsole, pos);
        getline(cin, text);
        if (text.length() > fieldLength)
            text = text.substr(0, fieldLength);
    }

    string getText() {
        return text;
    }
};

int main() {

    OneLineEditor editor;

    editor.setLength(20);
    editor.setPosition(10, 5);

    editor.draw();
    editor.input();

    cout << "\n\n¬ведЄнна€ строка: " << editor.getText() << endl;

    return 0;
}