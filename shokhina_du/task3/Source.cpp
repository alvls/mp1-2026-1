#include <iostream>
#include <string>
#include <vector>
#include <fstream>
using namespace std;

class TranslatorDictionary {
private:
    struct DictionaryEntry {
        string englishWord;
        string russianTranslation;

        DictionaryEntry() : englishWord(""), russianTranslation("") {}
        DictionaryEntry(const string& eng, const string& rus)
            : englishWord(eng), russianTranslation(rus) {
        }
    };

    vector<DictionaryEntry> entries;

    // поиск индекса слова
    int findIndex(const string& englishWord) const {
        for (size_t i = 0; i < entries.size(); i++) {
            if (entries[i].englishWord == englishWord) {
                return static_cast<int>(i);
            }
        }
        return -1;
    }

public:
    // конструктор по умолчанию
    TranslatorDictionary() {}
    // конструктор копирования
    TranslatorDictionary(const TranslatorDictionary& other) {
        entries = other.entries;
    }

    // деструктор
    ~TranslatorDictionary() {
        entries.clear();
    }

    // оператор присваивания
    TranslatorDictionary& operator=(const TranslatorDictionary& other) {
        if (this != &other) {
            entries = other.entries;
        }
        return *this;
    }

    // 1) добавить в словарь слово и его перевод
    void addWord(const string& englishWord, const string& russianTranslation) {
        if (englishWord.empty()) {
            throw invalid_argument("Английское слово не может быть пустым");
        }

        if (russianTranslation.empty()) {
            throw invalid_argument("Русский перевод не может быть пустым");
        }

        if (findIndex(englishWord) != -1) {
            throw runtime_error("Слово \"" + englishWord + "\" уже существует в словаре");
        }

        entries.push_back(DictionaryEntry(englishWord, russianTranslation));
    }

    // 2) изменить перевод указанного слова
    void updateTranslation(const string& englishWord, const string& newTranslation) {
        if (newTranslation.empty()) {
            throw invalid_argument("Новый перевод не может быть пустым");
        }

        int index = findIndex(englishWord);
        if (index == -1) {
            throw runtime_error("Слово \"" + englishWord + "\" не найдено в словаре");
        }

        string oldTranslation = entries[index].russianTranslation;
        entries[index].russianTranslation = newTranslation;
    }

    // 3) получить перевод выбранного слова
    string getTranslation(const string& englishWord) const {
        int index = findIndex(englishWord);
        if (index == -1) {
            throw runtime_error("Слово \"" + englishWord + "\" не найдено в словаре");
        }

        return entries[index].russianTranslation;
    }

    // 4) проверить наличие слова в словаре
    bool hasWord(const string& englishWord) const {
        return findIndex(englishWord) != -1;
    }

    // 5) получить число слов в словаре
    size_t getWordCount() const {
        return entries.size();
    }

    // 6) сохранить словарь в файл
    void saveToFile(const string& filename) const {
        ofstream outFile(filename);
        if (!outFile.is_open()) {
            throw runtime_error("Не удалось открыть файл для записи: " + filename);
        }

        for (const auto& entry : entries) {
            outFile << entry.englishWord << "|" << entry.russianTranslation << "\n";
        }

        outFile.close();
    }

    // 7) считать словарь из файла
    void loadFromFile(const string& filename) {
        ifstream inFile(filename);
        if (!inFile.is_open()) {
            throw runtime_error("Не удалось открыть файл для чтения: " + filename);
        }

        vector<DictionaryEntry> newEntries;
        string line;
        int lineNumber = 0;

        while (getline(inFile, line)) {
            lineNumber++;
            if (line.empty()) continue;

            size_t delimiterPos = line.find('|');
            if (delimiterPos == string::npos) {
                throw runtime_error("Ошибка формата в строке " + to_string(lineNumber) +
                    ": отсутствует разделитель '|'");
            }

            string englishWord = line.substr(0, delimiterPos);
            string russianTranslation = line.substr(delimiterPos + 1);

            if (englishWord.empty()) {
                throw runtime_error("Ошибка формата в строке " + to_string(lineNumber) +
                    ": пустое английское слово");
            }

            if (russianTranslation.empty()) {
                throw runtime_error("Ошибка формата в строке " + to_string(lineNumber) +
                    ": пустой русский перевод");
            }

            // проверка на дубликаты
            bool duplicate = false;
            for (const auto& entry : newEntries) {
                if (entry.englishWord == englishWord) {
                    duplicate = true;
                    break;
                }
            }

            if (duplicate) {
                throw runtime_error("Ошибка загрузки: дубликат слова \"" + englishWord +
                    "\" в строке " + to_string(lineNumber));
            }

            newEntries.push_back(DictionaryEntry(englishWord, russianTranslation));
        }

        inFile.close();
        entries = newEntries;
    }
};

// меню
void printMenu() {
    cout << "\n======= МЕНЮ СЛОВАРЯ =======\n";
    cout << "1. Добавить слово и перевод\n";
    cout << "2. Изменить перевод слова\n";
    cout << "3. Получить перевод слова\n";
    cout << "4. Проверить наличие слова\n";
    cout << "5. Получить количество слов\n";
    cout << "6. Сохранить словарь в файл\n";
    cout << "7. Загрузить словарь из файла\n";
    cout << "0. Выход\n";
    cout << "Выберите действие: ";
}

int main() {
    setlocale(LC_ALL, "rus");

    TranslatorDictionary dictionary;
    string filename = "dictionary.txt";
    int choice;

    cout << "Добро пожаловать в программу 'Англо-русский словарь'\n";

    do {
        printMenu();
        cin >> choice;

        try {
            switch (choice) {
            case 1: {
                string eng, rus;
                cout << "Введите английское слово: ";
                getline(cin, eng);
                cout << "Введите русский перевод: ";
                getline(cin, rus);
                dictionary.addWord(eng, rus);
                break;
            }
            case 2: {
                string eng, newRus;
                cout << "Введите английское слово: ";
                getline(cin, eng);
                cout << "Введите новый перевод: ";
                getline(cin, newRus);
                dictionary.updateTranslation(eng, newRus);
                break;
            }
            case 3: {
                string eng;
                cout << "Введите английское слово: ";
                getline(cin, eng);
                string translation = dictionary.getTranslation(eng);
                cout << "Перевод: " << translation << "\n";
                break;
            }
            case 4: {
                string eng;
                cout << "Введите английское слово: ";
                getline(cin, eng);
                if (dictionary.hasWord(eng)) {
                    cout << "Слово \"" << eng << "\" есть в словаре\n";
                }
                else {
                    cout << "Слово \"" << eng << "\" не найдено в словаре\n";
                }
                break;
            }
            case 5: {
                cout << "Количество слов в словаре: " << dictionary.getWordCount() << "\n";
                break;
            }
            case 6: {
                cout << "Введите имя файла (по умолчанию dictionary.txt): ";
                string fname;
                getline(cin, fname);
                if (fname.empty()) fname = filename;
                dictionary.saveToFile(fname);
                break;
            }
            case 7: {
                cout << "Введите имя файла (по умолчанию dictionary.txt): ";
                string fname;
                getline(cin, fname);
                if (fname.empty()) fname = filename;
                dictionary.loadFromFile(fname);
                break;
            }
            case 0: {
                break;
            }
            default: {
                cout << "Неверный выбор. Пожалуйста, выберите пункт от 0 до 7.\n";
                break;
            }
            }
        }
        catch (const exception& e) {
            cout << "Ошибка: " << e.what() << "\n";
        }

    } while (choice != 0);

    return 0;
}
