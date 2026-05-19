#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iomanip>

using namespace std;

class Thermometer {
private:
    static const int DAYS_IN_MONTH[12]; // количество дней в месяцах (невисокосный год)
    static const int HOURS_IN_DAY = 24;

    double temperatures[365][24];   // массив температур
    bool hasValue[365][24];         // флаг наличия наблюдения

    int startYear, startMonth, startDay, startHour; // начальные дата и время

    // вспомогательный метод: преобразует (день, месяц) в день года (0..364)
    int dayOfYear(int day, int month) const {
        if (month < 1 || month > 12)
            throw invalid_argument("Месяц должен быть от 1 до 12");
        int daysBefore = 0;
        for (int m = 1; m < month; ++m)
            daysBefore += DAYS_IN_MONTH[m - 1];
        if (day < 1 || day > DAYS_IN_MONTH[month - 1])
            throw invalid_argument("Некорректный день для указанного месяца");
        return daysBefore + (day - 1);
    }

    // проверка корректности часа
    void checkHour(int hour) const {
        if (hour < 0 || hour >= HOURS_IN_DAY)
            throw invalid_argument("Час должен быть от 0 до 23");
    }

    // проверка года (не високосный)
    void checkYear(int year) const {
        if (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0))
            throw invalid_argument("Год не должен быть високосным");
    }

public:
    // конструктор по умолчанию (год 2023, не високосный)
    Thermometer() : startYear(2023), startMonth(1), startDay(1), startHour(0) {
        // инициализация флагов
        for (int d = 0; d < 365; ++d)
            for (int h = 0; h < HOURS_IN_DAY; ++h)
                hasValue[d][h] = false;
    }

    // 1) установить начальные дату и время наблюдений
    void setStartDateTime(int year, int month, int day, int hour) {
        checkYear(year);
        dayOfYear(day, month); // проверка корректности даты
        checkHour(hour);
        startYear = year;
        startMonth = month;
        startDay = day;
        startHour = hour;
    }

    // 2) узнать начальные дату и время наблюдений
    void getStartDateTime(int& year, int& month, int& day, int& hour) const {
        year = startYear;
        month = startMonth;
        day = startDay;
        hour = startHour;
    }

    // 3) задать наблюдение (если уже есть, заменяется)
    void setObservation(int day, int month, int hour, double temperature) {
        int dayIdx = dayOfYear(day, month);
        checkHour(hour);
        temperatures[dayIdx][hour] = temperature;
        hasValue[dayIdx][hour] = true;
    }

    // 4) узнать температуру в выбранном наблюдении
    double getObservation(int day, int month, int hour) const {
        int dayIdx = dayOfYear(day, month);
        checkHour(hour);
        if (!hasValue[dayIdx][hour])
            throw runtime_error("Наблюдение для указанных даты и времени отсутствует");
        return temperatures[dayIdx][hour];
    }

    // 5) задать серию наблюдений для выбранной даты (массив из 24 температур по часам 0..23)
    void setSeriesForDate(int day, int month, const vector<double>& series) {
        if (series.size() != HOURS_IN_DAY)
            throw invalid_argument("Серия должна содержать 24 значения (для каждого часа)");
        int dayIdx = dayOfYear(day, month);
        for (int h = 0; h < HOURS_IN_DAY; ++h) {
            temperatures[dayIdx][h] = series[h];
            hasValue[dayIdx][h] = true;
        }
    }

    // 6a) средняя температура для выбранной даты
    double averageTemperatureForDate(int day, int month) const {
        int dayIdx = dayOfYear(day, month);
        double sum = 0.0;
        int count = 0;
        for (int h = 0; h < HOURS_IN_DAY; ++h) {
            if (hasValue[dayIdx][h]) {
                sum += temperatures[dayIdx][h];
                ++count;
            }
        }
        if (count == 0)
            throw runtime_error("Нет наблюдений для указанной даты");
        return sum / count;
    }

    // 6b) средняя температура для выбранного месяца
    double averageTemperatureForMonth(int month) const {
        double sum = 0.0;
        int count = 0;
        int daysInMonth = DAYS_IN_MONTH[month - 1];
        for (int d = 1; d <= daysInMonth; ++d) {
            int dayIdx = dayOfYear(d, month);
            for (int h = 0; h < HOURS_IN_DAY; ++h) {
                if (hasValue[dayIdx][h]) {
                    sum += temperatures[dayIdx][h];
                    ++count;
                }
            }
        }
        if (count == 0)
            throw runtime_error("Нет наблюдений для указанного месяца");
        return sum / count;
    }

    // 6c) средняя температура за всю историю наблюдений
    double averageTemperatureOverall() const {
        double sum = 0.0;
        int count = 0;
        for (int d = 0; d < 365; ++d) {
            for (int h = 0; h < HOURS_IN_DAY; ++h) {
                if (hasValue[d][h]) {
                    sum += temperatures[d][h];
                    ++count;
                }
            }
        }
        if (count == 0)
            throw runtime_error("Нет наблюдений за весь период");
        return sum / count;
    }

    // 7) средняя дневная или ночная температура для выбранного месяца
    // isDay = true -> дневные часы (6:00 – 18:00), false -> ночные (0-5, 19-23)
    double averageDayNightTemperature(int month, bool isDay) const {
        double sum = 0.0;
        int count = 0;
        int daysInMonth = DAYS_IN_MONTH[month - 1];
        for (int d = 1; d <= daysInMonth; ++d) {
            int dayIdx = dayOfYear(d, month);
            for (int h = 0; h < HOURS_IN_DAY; ++h) {
                bool isDayHour = (h >= 6 && h <= 18);
                if (isDayHour == isDay && hasValue[dayIdx][h]) {
                    sum += temperatures[dayIdx][h];
                    ++count;
                }
            }
        }
        if (count == 0)
            throw runtime_error("Нет наблюдений для указанного периода (день/ночь) в данном месяце");
        return sum / count;
    }

    // 8) сохранить историю наблюдений в файл
    void saveToFile(const string& filename) const {
        ofstream outFile(filename);
        if (!outFile.is_open())
            throw runtime_error("Не удалось открыть файл для записи: " + filename);

        // сохраняем начальные дату и время
        outFile << startYear << " " << startMonth << " " << startDay << " " << startHour << "\n";

        // сохраняем все наблюдения
        for (int d = 0; d < 365; ++d) {
            for (int h = 0; h < HOURS_IN_DAY; ++h) {
                if (hasValue[d][h]) {
                    int dayNum = d + 1;
                    int month = 1;
                    while (dayNum > DAYS_IN_MONTH[month - 1]) {
                        dayNum -= DAYS_IN_MONTH[month - 1];
                        ++month;
                    }
                    int day = dayNum;
                    outFile << day << " " << month << " " << h << " " << fixed << setprecision(2) << temperatures[d][h] << "\n";
                }
            }
        }
        outFile.close();
    }

    // 9) считать историю наблюдений из файла
    void loadFromFile(const string& filename) {
        ifstream inFile(filename);
        if (!inFile.is_open())
            throw runtime_error("Не удалось открыть файл для чтения: " + filename);

        // сброс текущих данных
        for (int d = 0; d < 365; ++d)
            for (int h = 0; h < HOURS_IN_DAY; ++h)
                hasValue[d][h] = false;

        string line;
        int lineNum = 0;
        // первая строка: начальные дата и время
        if (!getline(inFile, line))
            throw runtime_error("Файл пуст");
        lineNum++;
        istringstream iss(line);
        int y, m, d, h;
        if (!(iss >> y >> m >> d >> h))
            throw runtime_error("Ошибка формата в строке " + to_string(lineNum) + ": ожидались начальные дата и время");
        setStartDateTime(y, m, d, h);

        // последующие строки: наблюдения
        while (getline(inFile, line)) {
            lineNum++;
            if (line.empty()) continue;
            istringstream iss2(line);
            int day, month, hour;
            double temp;
            if (!(iss2 >> day >> month >> hour >> temp))
                throw runtime_error("Ошибка формата в строке " + to_string(lineNum) + ": ожидались день, месяц, час, температура");
            setObservation(day, month, hour, temp);
        }
        inFile.close();
    }
};

// определение статического массива дней в месяцах
const int Thermometer::DAYS_IN_MONTH[12] = { 31,28,31,30,31,30,31,31,30,31,30,31 };

// меню
void printMenu() {
    cout << "\n========= МЕНЮ ТЕРМОМЕТРА =========\n";
    cout << "1. Установить начальные дату и время\n";
    cout << "2. Узнать начальные дату и время\n";
    cout << "3. Задать наблюдение (температуру)\n";
    cout << "4. Узнать температуру в наблюдении\n";
    cout << "5. Задать серию наблюдений для даты (24 часа)\n";
    cout << "6. Найти среднюю температуру\n";
    cout << "    a) для выбранной даты\n";
    cout << "    b) для выбранного месяца\n";
    cout << "    c) за всю историю\n";
    cout << "7. Средняя дневная/ночная температура для месяца\n";
    cout << "8. Сохранить историю в файл\n";
    cout << "9. Загрузить историю из файла\n";
    cout << "0. Выход\n";
    cout << "Выберите действие: ";
}

int main() {
    setlocale(LC_ALL, "rus");

    Thermometer thermometer;
    int choice;
    int year, month, day, hour;
    double temp;
    string filename;

    cout << "Программа 'Термометр: история наблюдений за год'\n";

    do {
        printMenu();
        cin >> choice;

        try {
            switch (choice) {
            case 1:
                cout << "Введите год (не високосный): ";
                cin >> year;
                cout << "Введите месяц (1-12): ";
                cin >> month;
                cout << "Введите день: ";
                cin >> day;
                cout << "Введите час (0-23): ";
                cin >> hour;
                thermometer.setStartDateTime(year, month, day, hour);
                cout << "Начальные дата и время установлены.\n";
                break;

            case 2:
                thermometer.getStartDateTime(year, month, day, hour);
                cout << "Начальные дата и время: "
                    << year << "." << month << "." << day << " " << hour << ":00\n";
                break;

            case 3:
                cout << "Введите день, месяц, час и температуру (через пробел): ";
                cin >> day >> month >> hour >> temp;
                thermometer.setObservation(day, month, hour, temp);
                cout << "Наблюдение добавлено/обновлено.\n";
                break;

            case 4:
                cout << "Введите день, месяц и час: ";
                cin >> day >> month >> hour;
                temp = thermometer.getObservation(day, month, hour);
                cout << "Температура: " << temp << " °C\n";
                break;

            case 5: {
                cout << "Введите день и месяц: ";
                cin >> day >> month;
                vector<double> series(24);
                cout << "Введите 24 значения температуры (для часов 0..23):\n";
                for (int i = 0; i < 24; ++i) {
                    cout << "  час " << i << ": ";
                    cin >> series[i];
                }
                thermometer.setSeriesForDate(day, month, series);
                cout << "Серия наблюдений для даты добавлена.\n";
                break;
            }

            case 6: {
                int sub;
                cout << "Выберите подпункт (1 - дата, 2 - месяц, 3 - вся история): ";
                cin >> sub;
                if (sub == 1) {
                    cout << "Введите день и месяц: ";
                    cin >> day >> month;
                    double avg = thermometer.averageTemperatureForDate(day, month);
                    cout << "Средняя температура за дату: " << avg << " °C\n";
                }
                else if (sub == 2) {
                    cout << "Введите месяц: ";
                    cin >> month;
                    double avg = thermometer.averageTemperatureForMonth(month);
                    cout << "Средняя температура за месяц: " << avg << " °C\n";
                }
                else if (sub == 3) {
                    double avg = thermometer.averageTemperatureOverall();
                    cout << "Средняя температура за весь период: " << avg << " °C\n";
                }
                else {
                    cout << "Неверный подпункт.\n";
                }
                break;
            }

            case 7: {
                int month;
                int type;
                cout << "Введите месяц: ";
                cin >> month;
                cout << "Выберите (1 - дневная, 2 - ночная): ";
                cin >> type;
                bool isDay = (type == 1);
                double avg = thermometer.averageDayNightTemperature(month, isDay);
                cout << (isDay ? "Дневная" : "Ночная") << " средняя температура: " << avg << " °C\n";
                break;
            }

            case 8:
                cout << "Введите имя файла для сохранения: ";
                cin >> filename;
                thermometer.saveToFile(filename);
                cout << "История сохранена в файл " << filename << "\n";
                break;

            case 9:
                cout << "Введите имя файла для загрузки: ";
                cin >> filename;
                thermometer.loadFromFile(filename);
                cout << "История загружена из файла " << filename << "\n";
                break;

            case 0:
                cout << "Выход из программы.\n";
                break;

            default:
                cout << "Неверный выбор. Пожалуйста, выберите пункт от 0 до 9.\n";
            }
        }
        catch (const exception& e) {
            cout << "Ошибка: " << e.what() << "\n";
            // очищаем поток ввода при ошибке
            cin.clear();
            cin.ignore(10000, '\n');
        }
    } while (choice != 0);

    return 0;
}