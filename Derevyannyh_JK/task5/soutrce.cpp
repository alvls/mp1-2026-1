#include <iostream>
#include <map>
#include <vector>
#include <locale.h>
using namespace std;

class Product {
public:
    string code, name;
    int price, discount;

    Product() {}
    Product(string c, string n, int p, int d) : code(c), name(n), price(p), discount(d) {}

    int getPriceWithDiscount() {
        return price * (100 - discount) / 100;
    }
};

class Warehouse {
public:
    map<string, Product> products;

    void add(Product p) { products[p.code] = p; }

    Product* find(string code) {
        if (products.count(code)) return &products[code];
        return nullptr;
    }

    void init() {
        add(Product("01", "Молоко", 80, 10));
        add(Product("02", "Хлеб", 45, 5));
        add(Product("03", "Сыр", 350, 20));
        add(Product("04", "Колбаса", 500, 15));
        add(Product("05", "Яблоки", 120, 0));
    }
};

struct Item {
    Product product;
    int count;

    Item(Product p, int c) : product(p), count(c) {}
};

class CashRegister {
private:
    Warehouse* warehouse;
    map<string, Item*> cart;

public:
    CashRegister(Warehouse* w) : warehouse(w) {}

    void scan(string code) {
        Product* p = warehouse->find(code);
        if (!p) {
            cout << "Товар не найден!\n";
            return;
        }

        if (cart.count(code)) {
            cart[code]->count++;
        }
        else {
            cart[code] = new Item(*p, 1);
        }
        cout << "Добавлен: " << p->name << "\n";
    }

    void getInfo(string code) {
        Product* p = warehouse->find(code);
        if (p) {
            cout << p->name << " - " << p->price << " руб., скидка " << p->discount << "%\n";
        }
        else {
            cout << "Не найден\n";
        }
    }

    int getTotal() {
        int total = 0;
        for (auto& pair : cart) {
            total += pair.second->product.getPriceWithDiscount() * pair.second->count;
        }
        return total;
    }

    void printCheck() {
        cout << "\n========== ЧЕК ==========\n";
        int totalBefore = 0, totalDiscount = 0;

        for (auto& pair : cart) {
            Item* item = pair.second;
            int priceWithDisc = item->product.getPriceWithDiscount();
            int totalPrice = item->product.price * item->count;
            int finalPrice = priceWithDisc * item->count;

            cout << item->product.name << " - " << item->product.price << " руб - "
                << item->count << " шт - " << finalPrice << " руб\n";

            totalBefore += totalPrice;
            totalDiscount += (totalPrice - finalPrice);
        }

        cout << "-------------------------\n";
        cout << "Общая стоимость: " << totalBefore << " руб\n";
        cout << "Скидка: " << totalDiscount << " руб\n";
        cout << "К оплате: " << getTotal() << " руб\n";
        cout << "=========================\n\n";
    }

    void remove(string code) {
        if (cart.count(code)) {
            delete cart[code];
            cart.erase(code);
            cout << "Товар удален\n";
        }
        else {
            cout << "Товара нет в корзине\n";
        }
    }

    void showCart() {
        cout << "\nКорзина:\n";
        for (auto& pair : cart) {
            Item* i = pair.second;
            cout << i->product.name << " x" << i->count << " = "
                << i->product.getPriceWithDiscount() * i->count << " руб\n";
        }
        cout << "Итого: " << getTotal() << " руб\n\n";
    }

    ~CashRegister() {
        for (auto& pair : cart) delete pair.second;
    }
};
int main() {
    setlocale(LC_ALL, "rus");
    Warehouse warehouse;
    warehouse.init();
    CashRegister cash(&warehouse);

    string cmd, code;

    cout << "Команды: scan, info, cart, check, total, remove, exit\n\n";

    while (true) {
        cout << "> ";
        cin >> cmd; if (cmd == "exit") break;
        else if (cmd == "scan") { cin >> code; cash.scan(code); }
        else if (cmd == "info") { cin >> code; cash.getInfo(code); }
        else if (cmd == "cart") cash.showCart();
        else if (cmd == "check") cash.printCheck();
        else if (cmd == "total") cout << "К оплате: " << cash.getTotal() << " руб\n";
        else if (cmd == "remove") { cin >> code; cash.remove(code); }
        else cout << "Неизвестная команда\n";
    }

    return 0;
}