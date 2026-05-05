#include <iostream>
#include <string>
#include <unordered_map>
#include <iomanip>

using namespace std;

class Warehouse {
public:
    struct Product {
        string barcode;
        string name;
        int price;
        int discount;

        Product(string bc, string nm, int pr, int disc = 0)
            : barcode(bc), name(nm), price(pr), discount(disc) {
        }
        Product() : barcode(""), name(""), price(0), discount(0) {}
    };

    unordered_map<string, Product> products;

    Warehouse() {
        addProduct("0001", "Сок", 80, 10);
        addProduct("0002", "Хлеб", 50, 5);
        addProduct("0003", "Сыр", 250, 20);
        addProduct("0004", "Яблоки", 120, 15);
    }

    bool addProduct(const string& barcode, const string& name,
        int price, int discount = 0) {
        if (discount < 0 || discount > 50) {
            cout << "Скидка должна быть в диапазоне 0–50 %" << endl;
            return false;
        }
        products[barcode] = Product(barcode, name, price, discount);
        return true;
    }

    const Product& getProduct(const string& barcode) const {
        return products.at(barcode);
    }

    bool setDiscount(const string& barcode, int discount) {
        if (discount < 1 || discount > 50) {
            cout << "Ошибка: Скидка должна быть в диапазоне 1–50 %\n";
            return false;
        }

        auto it = products.find(barcode);
        if (it == products.end()) {
            cout << "Ошибка: Товар не найден на складе\n";
            return false;
        }

        it->second.discount = discount;
        return true;
    }

    bool hasProduct(const string& barcode) const {
        return products.find(barcode) != products.end();
    }
};

class boxOffice {
private:
    Warehouse& warehouse;

    struct CheckItem {
        Warehouse::Product product;
        int quantity;
        int totalPrice;
        int totalDiscount;
        int finalPrice;

        CheckItem(const Warehouse::Product& prod, int qty)
            : product(prod), quantity(qty) {
            totalPrice = prod.price * qty;
            totalDiscount = (prod.price * prod.discount / 100) * qty;
            finalPrice = totalPrice - totalDiscount;
        }
        CheckItem() : quantity(0), totalPrice(0), totalDiscount(0), finalPrice(0) {}
    };

    unordered_map<string, CheckItem> currentCheck;
    int totalAmount = 0;
    int totalDiscount = 0;
    int finalAmount = 0;

    void updateTotals() {
        totalAmount = 0;
        totalDiscount = 0;
        finalAmount = 0;

        for (const auto& item : currentCheck) {
            totalAmount += item.second.totalPrice;
            totalDiscount += item.second.totalDiscount;
            finalAmount += item.second.finalPrice;
        }
    }

public:
    boxOffice(Warehouse& wh) : warehouse(wh) {}

    bool scanProduct(const string& barcode) {
        if (!warehouse.hasProduct(barcode)) {
            cout << "Ошибка: Товар не найден на складе\n";
            return false;
        }

        const auto& product = warehouse.getProduct(barcode);
        addProductToCheck(product);
        cout << "Товар '" << product.name << "' добавлен в чек.\n";
        return true;
    }

    bool GetProductInfo(const string& barcode, Warehouse::Product& outProduct) const {
        if (warehouse.hasProduct(barcode)) {
            outProduct = warehouse.getProduct(barcode);
            return true;
        }
        return false;
    }

    void addProductToCheck(const Warehouse::Product& product) {
        auto& checkItem = currentCheck[product.barcode];

        if (checkItem.quantity == 0) {
            checkItem = CheckItem(product, 1);
        }
        else {
            checkItem.quantity++;
            checkItem.totalPrice = product.price * checkItem.quantity;
            checkItem.totalDiscount = (product.price * product.discount / 100) * checkItem.quantity;
            checkItem.finalPrice = checkItem.totalPrice - checkItem.totalDiscount;
        }

        updateTotals();
    }

    void printReceipt() const {
        if (currentCheck.empty()) {
            cout << "Чек пустой.\n";
            return;
        }

        cout << "\n--- ЧЕК ---\n";
        cout << left << setw(20) << "Наименование"
            << setw(10) << "Цена"
            << setw(8) << "Кол-во"
            << setw(12) << "Сумма"
            << setw(12) << "Скидка"
            << "Итого\n";

        for (const auto& item : currentCheck) {
            const auto& checkItem = item.second;
            cout << left << setw(20) << checkItem.product.name
                << setw(10) << checkItem.product.price
                << setw(8) << checkItem.quantity
                << setw(12) << checkItem.totalPrice
                << setw(12) << checkItem.totalDiscount
                << checkItem.finalPrice << " руб.\n";
        }

        cout << "Общая стоимость: " << totalAmount << " руб.\n";
        cout << "Суммарная скидка: " << totalDiscount << " руб.\n";
        cout << "Итого к оплате: " << finalAmount << " руб.\n";
    }

    int FinalAmount() const {
        return finalAmount;
    }

    bool removeProductFromCheck(const string& barcode) {
        auto it = currentCheck.find(barcode);
        if (it == currentCheck.end()) {
            cout << "Ошибка: " << barcode
                << " не найден в чеке!\n";
            return false;
        }

        currentCheck.erase(it);
        updateTotals();
        cout << "Товар удалён из чека.\n";
        return true;
    }
};

int main() {
    setlocale(LC_ALL, "Russian");

    Warehouse wh;
    boxOffice boxOffice(wh);

    boxOffice.scanProduct("0001");
    boxOffice.scanProduct("0002");
    boxOffice.scanProduct("0001");
    boxOffice.scanProduct("0003");

    Warehouse::Product juice;
    if (boxOffice.GetProductInfo("0001", juice)) {
        cout << "Информация о товаре: " << juice.name
            << ", цена: " << juice.price << " руб.\n\n";
    }
    else {
        cout << "Ошибка: Товар не найден на складе\n";
    }

    boxOffice.printReceipt();

    boxOffice.removeProductFromCheck("0002");

    cout << "\nПосле удаления хлеба:\n";
    boxOffice.printReceipt();

    cout << "\nИтоговая сумма к оплате: "
        << boxOffice.FinalAmount() << " руб.\n";

    return 0;
}