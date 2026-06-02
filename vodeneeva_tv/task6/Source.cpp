#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <clocale>
using namespace std;

class Igra {
private:
    int dlina;
    vector<int> zagadannoeChislo;
    int kolvo;

    void sozdatChislo() {
        zagadannoeChislo.clear();
        bool cifraByla[10] = {false};
        
        for (int i = 0; i < dlina; i++) {
            int cifra;
            do {
                cifra = rand() % 10;
                if (i == 0 && dlina > 1 && cifra == 0) {
                    continue;
                }
            } while (cifraByla[cifra]);
            
            cifraByla[cifra] = true;
            zagadannoeChislo.push_back(cifra);
        }
    }

    bool proveritVvod(const string& strokaVvoda, vector<int>& chisloIgroka) {
        if (strokaVvoda.length() != dlina) {
            cout << "Ошибка: число должно состоять из " << dlina << " цифр!\n";
            return false;
        }
        
        bool cifraIspolzovana[10] = {false};
        chisloIgroka.clear();
        
        for (char simvol : strokaVvoda) {
            if (simvol < '0' || simvol > '9') {
                return false;
            }
            
            int cifra = simvol - '0';
            if (cifraIspolzovana[cifra]) {
                cout << "Ошибка: цифры не должны повторяться!\n";
                return false;
            }
            
            cifraIspolzovana[cifra] = true;
            chisloIgroka.push_back(cifra);
        }
        
        return true;
    }

    void podschet(const vector<int>& popytkaIgroka, int& biki, int& korovy) {
        biki = 0;
        korovy = 0;
        
        for (int i = 0; i < dlina; i++) {
            if (popytkaIgroka[i] == zagadannoeChislo[i]) {
                biki++;
            } else {
                for (int j = 0; j < dlina; j++) {
                    if (popytkaIgroka[i] == zagadannoeChislo[j]) {
                        korovy++;
                        break;
                    }
                }
            }
        }
    }

public:
    Igra() : kolvo(0) {
        srand(time(0));
        
        do {
            cout << "Введите длину числа (от 2 до 10): ";
            cin >> dlina;
            
            if (dlina < 2 || dlina > 10) {
                cout << "Длина числа должна быть от 2 до 10\n";
            }
        } while (dlina < 2 || dlina > 10);
        
        sozdatChislo();
        cout << "Число загадано! Начинаем игру!\n\n";
    }

    void zapustitIgry() {
        string vvodIgroka;
        vector<int> popytkaIgroka;
        
        while (true) {
            cout << "Попытка " << (kolvo + 1) << "\n";
            cout << "Введите " << dlina << "-значное число с неповторяющимися цифрами: ";
            cin >> vvodIgroka;
            
            if (!proveritVvod(vvodIgroka, popytkaIgroka)) {
                continue;
            }
            
            int biki, korovy;
            podschet(popytkaIgroka, biki, korovy);
            kolvo++;
            
            cout << "Результат: Быки: " << biki << ", Коровы: " << korovy << "\n\n";
            
            if (biki == dlina) {
                cout << "Поздравляем! Вы угадали число за " << kolvo << " попыток\n";
                cout << "Загаданное число: ";
                for (int cifra : zagadannoeChislo) {
                    cout << cifra;
                }
                cout << endl;
                break;
            }
        }
    }
};

int main() {
    setlocale(LC_ALL, "Russian");
    Igra igra;
    igra.zapustitIgry();
    
    return 0;
}