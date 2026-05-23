#include <iostream>
#include <iomanip>
using namespace std;

class DynamicArray {
private:
    double* data;
    size_t size;

public:
    // конструктор по умолчанию
    DynamicArray() : data(nullptr), size(0) {}

    // конструктор с заданным размером
    DynamicArray(size_t n) : size(n) {
        if (n > 0) {
            data = new double[n]();
        } 
        else {
            data = nullptr;
        }
    }

    // 1) задать размер массива
    void setSize(size_t newSize) {
        if (newSize == size) return;
        
        double* newData = nullptr;
        if (newSize > 0) {
            newData = new double[newSize]();
            size_t copySize = min(size, newSize);
            for (size_t i = 0; i < copySize; ++i) {
                newData[i] = data[i];
            }
        }
        delete[] data;
        data = newData;
        size = newSize;
    }

    // 2) узнать размер массива
    size_t getSize() const {
        return size;
    }

    // 3) задать элемент массива по индексу
    void setElement(size_t index, double value) {
        if (index >= size) {
            throw out_of_range("Индекс выходит за пределы массива");
        }
        data[index] = value;
    }

    // 4) узнать элемент массива по индексу
    double getElement(size_t index) const {
        if (index >= size) {
            throw out_of_range("Индекс выходит за пределы массива");
        }
        return data[index];
    }

    // 5) найти минимальный элемент массива
    double findMin() const {
        if (size == 0) {
            throw runtime_error("Массив пуст, невозможно найти минимум");
        }
        
        double minVal = data[0];
        for (size_t i = 1; i < size; ++i) {
            if (data[i] < minVal) {
                minVal = data[i];
            }
        }
        return minVal;
    }

    // 6) проверить, является ли массив упорядоченным (по возрастанию)
    bool isOrdered() const {
        if (size <= 1) return true;
        
        for (size_t i = 1; i < size; ++i) {
            if (data[i] < data[i - 1]) {
                return false;
            }
        }
        return true;
    }

    // 7) выделить из массива подмассив с элементами с нечетными индексами
    DynamicArray extractOddIndices() const {
        size_t newSize = (size + 1) / 2;  // количество элементов с нечетными индексами
        DynamicArray result(newSize);
        
        size_t resultIndex = 0;
        for (size_t i = 1; i < size; i += 2) {  // начинаем с индекса 1 (нечетный)
            result.data[resultIndex++] = data[i];
        }
        
        return result;
    }

    // дополнительный метод для вывода массива
    void print() const {
        cout << "[";
        for (size_t i = 0; i < size; ++i) {
            cout << data[i];
            if (i < size - 1) cout << ", ";
        }
        cout << "]";
    }
};

int main() {
    setlocale(LC_ALL, "rus");

    cout << "Тест:\n\n";

    // 1) создание массива и задание размера
    cout << "1. Создание массива размером 5:\n";
    DynamicArray arr(5);
    cout << "   Размер массива: " << arr.getSize() << "\n";

    // 2) задание элементов
    cout << "\n2. Заполнение массива:\n";
    for (size_t i = 0; i < arr.getSize(); ++i) {
    arr.setElement(i, i * 2.5);
    cout << "   arr[" << i << "] = " << arr.getElement(i) << "\n";
    }

    // 3) изменение размера массива
    cout << "\n3. Изменение размера массива на 7:\n";
    arr.setSize(7);
    arr.setElement(5, 15.5);
    arr.setElement(6, 3.14);
    cout << "   Новый размер: " << arr.getSize() << "\n";
    cout << "   Массив: ";
    arr.print();
    cout << "\n";

    // 4) поиск минимального элемента
    cout << "\n4. Минимальный элемент: " << arr.findMin() << "\n";

    // 5) проверка упорядоченности
    cout << "\n5. Массив упорядочен? " << (arr.isOrdered() ? "Да" : "Нет") << "\n";

    // 6) создание неупорядоченного массива
    cout << "\n6. Создание неупорядоченного массива:\n";
    DynamicArray arr2(4);
    arr2.setElement(0, 10.0);
    arr2.setElement(1, 5.5);
    arr2.setElement(2, 7.2);
    arr2.setElement(3, 3.8);
    cout << "   Массив: ";
    arr2.print();
    cout << "\n";
    cout << "   Массив упорядочен? " << (arr2.isOrdered() ? "Да" : "Нет") << "\n";

    // 7) выделение подмассива с нечетными индексами
    cout << "\n7. Выделение подмассива с элементами нечетных индексов:\n";
    cout << "   Исходный массив: ";
    arr.print();
    cout << "\n";
        
    DynamicArray oddIndices = arr.extractOddIndices();
    cout << "   Подмассив (индексы 1,3,5,...): ";
    oddIndices.print();
    cout << "\n";

    // 8) работа с пустым массивом и изменение его размера
    cout << "\n8. Работа с пустым массивом:\n";
    DynamicArray emptyArr;
    cout << "   Размер пустого массива: " << emptyArr.getSize() << "\n";

    emptyArr.setSize(3);
    emptyArr.setElement(0, 1.1);
    emptyArr.setElement(1, 2.2);
    emptyArr.setElement(2, 3.3);
    cout << "   После изменения размера: ";
    emptyArr.print();
    cout << "\n";

    // 9) тестирование конструктора копирования
    cout << "\n9. Тестирование конструктора копирования:\n";
    DynamicArray arrCopy(arr);
    cout << "   Оригинал: ";
    arr.print();
    cout << "\n   Копия:    ";
    arrCopy.print();
    cout << "\n";

    // 10) тестирование оператора присваивания
    cout << "\n10. Тестирование оператора присваивания:\n";
    DynamicArray arrAssign;
    arrAssign = arr2;
    cout << "   Исходный массив: ";
    arr2.print();
    cout << "\n   Присвоенный:     ";
    arrAssign.print();
    cout << "\n";
    
    system("pause");

    return 0;
}
