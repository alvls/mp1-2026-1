#include <iostream>
#include <string>
#include <fstream>
#include <locale.h>

using namespace std;

string trim(const string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == string::npos) return "";
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, last - first + 1);
}

struct Date {
    int day, month, year;

    Date() : day(0), month(0), year(0) {}
    Date(int d, int m, int y) : day(d), month(m), year(y) {}

    string toString() const {
        if (day == 0 && month == 0 && year == 0) return "не указана";
        return to_string(day) + "." + to_string(month) + "." + to_string(year);
    }

    bool isValid() const {
        if (day == 0 && month == 0 && year == 0) return true;
        if (year < 1900 || year > 2026) return false;
        if (month < 1 || month > 12) return false;

        int daysInMonth[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
        bool isLeap = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
        if (month == 2 && isLeap) daysInMonth[1] = 29;

        return (day >= 1 && day <= daysInMonth[month - 1]);
    }
};

class Contact {
public:
    string lastName;
    string firstName;
    string patronymic;
    string phone;
    Date birthday;
    bool favorite;

    // Конструкторы
    Contact() : favorite(false) {}

    Contact(const string& ln, const string& fn, const string& p,
        const string& ph = "", const Date& bd = Date(), bool fav = false) {
        if (ln.empty() || fn.empty() || p.empty()) {
            throw "Ошибка: Фамилия, имя и отчество обязательны";
        }
        lastName = trim(ln);
        firstName = trim(fn);
        patronymic = trim(p);
        phone = trim(ph);
        birthday = bd;
        favorite = fav;
    }

    string getFullName() const {
        return lastName + " " + firstName + " " + patronymic;
    }

    string getSortKey() const {
        return lastName + "|" + firstName + "|" + patronymic;
    }

    void print() const {
        cout << (favorite ? "* " : "  ");
        cout << getFullName();
        cout << " | Тел: " << (phone.empty() ? "-" : phone);
        cout << " | ДР: " << birthday.toString() << endl;
    }

    void saveToFile(ofstream& file) const {
        file << lastName << endl;
        file << firstName << endl;
        file << patronymic << endl;
        file << phone << endl;
        file << birthday.day << endl;
        file << birthday.month << endl;
        file << birthday.year << endl;
        file << (favorite ? "1" : "0") << endl;
    }

    bool loadFromFile(ifstream& file) {
        string line;

        if (!getline(file, line)) return false;
        lastName = line;

        if (!getline(file, line)) return false;
        firstName = line;

        if (!getline(file, line)) return false;
        patronymic = line;

        if (!getline(file, line)) return false;
        phone = line;

        if (!getline(file, line)) return false;
        int d = stoi(line);

        if (!getline(file, line)) return false;
        int m = stoi(line);

        if (!getline(file, line)) return false;
        int y = stoi(line);

        if (!getline(file, line)) return false;
        string favStr = line;

        birthday = Date(d, m, y);
        if (!birthday.isValid()) return false;

        if (favStr != "0" && favStr != "1") return false;
        favorite = (favStr == "1");

        return !lastName.empty() && !firstName.empty() && !patronymic.empty();
    }
};

class ContactsBook {
private:
    Contact** contacts; 
    int size;
    int capacity;

    void resize() {
        int newCapacity = capacity * 2;
        Contact** newArr = new Contact * [newCapacity];
        for (int i = 0; i < size; i++) {
            newArr[i] = contacts[i];
        }
        delete[] contacts;
        contacts = newArr;
        capacity = newCapacity;
    }

    void sortContacts() {
        for (int i = 0; i < size - 1; i++) {
            for (int j = 0; j < size - i - 1; j++) {
                if (contacts[j]->getSortKey() > contacts[j + 1]->getSortKey()) {
                    Contact* temp = contacts[j];
                    contacts[j] = contacts[j + 1];
                    contacts[j + 1] = temp;
                }
            }
        }
    }

public:
    ContactsBook() : size(0), capacity(10) {
        contacts = new Contact * [capacity];
        for (int i = 0; i < capacity; i++) {
            contacts[i] = nullptr;
        }
    }

    ContactsBook(const ContactsBook& other) : size(0), capacity(10) {
        contacts = new Contact * [capacity];
        for (int i = 0; i < capacity; i++) {
            contacts[i] = nullptr;
        }

        for (int i = 0; i < other.size; i++) {
            createContact(*other.contacts[i]);
        }
    }

    ContactsBook& operator=(const ContactsBook& other) {
        if (this == &other) return *this;

        for (int i = 0; i < size; i++) {
            delete contacts[i];
        }
        delete[] contacts;

        size = 0;
        capacity = other.capacity;
        contacts = new Contact * [capacity];
        for (int i = 0; i < capacity; i++) {
            contacts[i] = nullptr;
        }

        for (int i = 0; i < other.size; i++) {
            createContact(*other.contacts[i]);
        }

        return *this;
    }

    ~ContactsBook() {
        for (int i = 0; i < size; i++) {
            delete contacts[i];
        }
        delete[] contacts;
    }

    bool createContact(const Contact& contact) {
        for (int i = 0; i < size; i++) {
            if (contacts[i]->getSortKey() == contact.getSortKey()) {
                return false;
            }
        }

        if (size >= capacity) {
            resize();
        }

        contacts[size] = new Contact(contact);
        size++;
        sortContacts();
        return true;
    }

    bool modifyContact(const string& lastName, const string& firstName, const string& patronymic,
        const Contact& newData) {
        if (!deleteContact(lastName, firstName, patronymic)) {
            return false;
        }
        return createContact(newData);
    }

    Contact* findContactByFullName(const string& lastName, const string& firstName, const string& patronymic) {
        for (int i = 0; i < size; i++) {
            if (contacts[i]->lastName == lastName &&
                contacts[i]->firstName == firstName &&
                contacts[i]->patronymic == patronymic) {
                return contacts[i];
            }
        }
        return nullptr;
    }

    Contact* findContactByPhone(const string& phone) {
        if (phone.empty()) return nullptr;
        string p = trim(phone);
        for (int i = 0; i < size; i++) {
            if (contacts[i]->phone == p) {
                return contacts[i];
            }
        }
        return nullptr;
    }

    ContactsBook getContactsByLetter(char letter) const {
        ContactsBook result;
        if (letter >= 'a' && letter <= 'z') letter = letter - 'a' + 'A';
        for (int i = 0; i < size; i++) {
            if (!contacts[i]->lastName.empty()) {
                char firstChar = contacts[i]->lastName[0];
                if (firstChar >= 'a' && firstChar <= 'z') firstChar = firstChar - 'a' + 'A';
                if (firstChar == letter) {
                    result.createContact(*contacts[i]);
                }
            }
        }
        return result;
    }

    int getContactCount() const {
        return size;
    }

    bool addToFavorites(const string& lastName, const string& firstName, const string& patronymic) {
        Contact* c = findContactByFullName(lastName, firstName, patronymic);
        if (c == nullptr) return false;

        Contact newContact(c->lastName, c->firstName, c->patronymic,
            c->phone, c->birthday, true);
        return modifyContact(lastName, firstName, patronymic, newContact);
    }

    bool removeFromFavorites(const string& lastName, const string& firstName, const string& patronymic) {
        Contact* c = findContactByFullName(lastName, firstName, patronymic);
        if (c == nullptr) return false;

        Contact newContact(c->lastName, c->firstName, c->patronymic,
            c->phone, c->birthday, false);
        return modifyContact(lastName, firstName, patronymic, newContact);
    }

    ContactsBook getAllFavorites() const {
        ContactsBook result;
        for (int i = 0; i < size; i++) {
            if (contacts[i]->favorite) {
                result.createContact(*contacts[i]);
            }
        }
        return result;
    }

    bool deleteContact(const string& lastName, const string& firstName, const string& patronymic) {
        for (int i = 0; i < size; i++) {
            if (contacts[i]->lastName == lastName &&
                contacts[i]->firstName == firstName &&
                contacts[i]->patronymic == patronymic) {
                delete contacts[i];
                for (int j = i; j < size - 1; j++) {
                    contacts[j] = contacts[j + 1];
                }
                size--;
                return true;
            }
        }
        return false;
    }

    bool saveToFile(const string& filename) const {
        ofstream file(filename);
        if (!file.is_open()) return false;

        file << size << endl;
        for (int i = 0; i < size; i++) {
            contacts[i]->saveToFile(file);
        }

        file.close();
        return true;
    }

    bool loadFromFile(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) return false;

        int newSize;
        file >> newSize;
        file.ignore();

        ContactsBook temp;

        for (int i = 0; i < newSize; i++) {
            Contact* c = new Contact();
            if (!c->loadFromFile(file)) {
                delete c;
                return false;
            }
            temp.createContact(*c);
            delete c;
        }

        file.close();

        for (int i = 0; i < size; i++) {
            delete contacts[i];
        }

        delete[] contacts;
        size = temp.size;
        capacity = temp.capacity;
        contacts = new Contact * [capacity];
        for (int i = 0; i < capacity; i++) {
            contacts[i] = nullptr;
        }
        for (int i = 0; i < size; i++) {
            contacts[i] = new Contact(*temp.contacts[i]);
        }

        return true;
    }
    void printAll() const {
        if (size == 0) {
            cout << "Список контактов пуст." << endl;
            return;
        }
        cout << "\n=== СПИСОК КОНТАКТОВ ===" << endl;
        for (int i = 0; i < size; i++) {
            contacts[i]->print();
        }
        cout << "Всего: " << size << " контактов\n" << endl;
    }
};

int main() {
    setlocale(LC_ALL, "Russian");

    ContactsBook book;

    Contact c1("Иванов", "Иван", "Иванович", "+7-123-456-78-90", Date(15, 5, 1985), false);
    Contact c2("Петров", "Петр", "Петрович", "+7-234-567-89-01", Date(20, 10, 1990), true);
    Contact c3("Сидоров", "Сидор", "Сидорович", "+7-345-678-90-12", Date(3, 3, 2000), false);
    Contact c4("Иванова", "Мария", "Ивановна", "+7-456-789-01-23", Date(10, 12, 1995), false);

    book.createContact(c1);
    book.createContact(c2);
    book.createContact(c3);
    book.createContact(c4);

    cout << "=== ВСЕ КОНТАКТЫ ===" << endl;
    book.printAll();

    cout << "\n=== ПОИСК ПО ФИО ===" << endl;
    Contact* found = book.findContactByFullName("Петров", "Петр", "Петрович");
    if (found != nullptr) {
        cout << "Найден: ";
        found->print();
    }
    else {
        cout << "Контакт не найден" << endl;
    }

    found = book.findContactByFullName("Смирнов", "Алексей", "Алексеевич");
    if (found == nullptr) {
        cout << "Контакта 'Смирнов Алексей Алексеевич' не существует" << endl;
    }

    cout << "\n=== ПОИСК ПО ТЕЛЕФОНУ ===" << endl;
    found = book.findContactByPhone("+7-345-678-90-12");
    if (found != nullptr) {
        cout << "Найден: ";
        found->print();
    }
    else {
        cout << "Контакт не найден" << endl;
    }

    cout << "\n=== КОНТАКТЫ НА БУКВУ 'И' ===" << endl;
    ContactsBook letterI = book.getContactsByLetter('И');
    letterI.printAll();

    cout << "Всего контактов: " << book.getContactCount() << endl;

    cout << "\n=== ДОБАВЛЕНИЕ В ИЗБРАННЫЕ ===" << endl;
    book.addToFavorites("Сидоров", "Сидор", "Сидорович");
    book.printAll();

    cout << "\n=== УДАЛЕНИЕ ИЗ ИЗБРАННЫХ ===" << endl;
    book.removeFromFavorites("Петров", "Петр", "Петрович");
    book.printAll();

    cout << "\n=== ТОЛЬКО ИЗБРАННЫЕ ===" << endl;
    ContactsBook favorites = book.getAllFavorites();
    favorites.printAll();

    cout << "\n=== УДАЛЕНИЕ КОНТАКТА ===" << endl;
    book.deleteContact("Иванова", "Мария", "Ивановна");
    book.printAll();

    if (book.saveToFile("contacts.txt")) {
        cout << "Контакты сохранены в файл 'contacts.txt'" << endl;
    }
    else {
        cout << "Ошибка при сохранении" << endl;
    }

    ContactsBook newBook;
    if (newBook.loadFromFile("contacts.txt")) {
        cout << "\n=== ЗАГРУЖЕННЫЕ ИЗ ФАЙЛА КОНТАКТЫ ===" << endl;
        newBook.printAll();
    }
    else {
        cout << "Ошибка при загрузке файла" << endl;
    }
    system("pause");
    return 0;
}