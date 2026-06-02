#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <map>
#include <sstream>
using namespace std;

// вспомогательные функции для работы с датами
string getCurrentDate() {
    // фиксированная текущая дата 
    return "2026-05-01";
}

string addDays(const string& date, int days) {
    int year = stoi(date.substr(0, 4));
    int month = stoi(date.substr(5, 2));
    int day = stoi(date.substr(8, 2));

    day += days;
    while (day > 30) {
        day -= 30;
        month++;
        if (month > 12) {
            month = 1;
            year++;
        }
    }

    stringstream ss;
    ss << setw(4) << setfill('0') << year << "-" << setw(2) << setfill('0') << month << "-" << setw(2) << setfill('0') << day;
    return ss.str();
}

int daysBetween(const string& from, const string& to) {
    int y1 = stoi(from.substr(0, 4)), m1 = stoi(from.substr(5, 2)), d1 = stoi(from.substr(8, 2));
    int y2 = stoi(to.substr(0, 4)), m2 = stoi(to.substr(5, 2)), d2 = stoi(to.substr(8, 2));
    int days1 = y1 * 360 + m1 * 30 + d1;
    int days2 = y2 * 360 + m2 * 30 + d2;
    return days2 - days1;
}

enum class TrainType { Lastochka, Firm, Fast };
enum class CarType { Sitting, Platzkart, Coupe, SV };
enum class SeatType { Sitting, Up, Down };

// структура для мест 
struct Seat {
    int number;
    SeatType type;
    bool isOccupied;

    Seat(int num, SeatType t) : number(num), type(t), isOccupied(false) {}
};

// страктура для вагонов 
class Car {
public:
    int number;
    CarType type;
    vector<Seat> seats;

    Car(int num, CarType t, int seatCount, SeatType defaultType) : number(num), type(t) {
        for (int i = 1; i <= seatCount; ++i) {
            seats.emplace_back(i, defaultType);
        }
    }

    // верх/низ
    Car(int num, CarType t, int upCount, int downCount) : number(num), type(t) {
        for (int i = 1; i <= upCount; ++i) {
            seats.emplace_back(i, SeatType::Up);
        }
        for (int i = upCount + 1; i <= upCount + downCount; ++i) {
            seats.emplace_back(i, SeatType::Down);
        }
    }

    // проверка возможности бронирования 
    bool canReserve(const map<SeatType, int>& required) const {
        map<SeatType, int> available;
        for (const auto& seat : seats) {
            if (!seat.isOccupied) {
                available[seat.type]++;
            }
        }
        for (const auto& req : required) {
            if (available.at(req.first) < req.second) return false;
        }
        return true;
    }

    // бронирование
    vector<int> reserve(const map<SeatType, int>& required) {
        map<SeatType, int> reqCopy = required;
        vector<int> reservedNumbers;
        for (auto& seat : seats) {
            if (seat.isOccupied) continue;
            auto it = reqCopy.find(seat.type);
            if (it != reqCopy.end() && it->second > 0) {
                seat.isOccupied = true;
                reservedNumbers.push_back(seat.number);
                it->second--;
                if (it->second == 0) {
                    reqCopy.erase(it);
                }
                if (reqCopy.empty()) break;
            }
        }
        return reservedNumbers;
    }

    // отмена бронирования 
    void cancelReservation(const vector<int>& seatNumbers) {
        for (int num : seatNumbers) {
            for (auto& seat : seats) {
                if (seat.number == num) {
                    seat.isOccupied = false;
                    break;
                }
            }
        }
    }
};

// структура для поездов 
class Train {
public:
    int number;
    TrainType type;
    string direction; // "НН -> МСК" или "МСК -> НН"
    string date;
    vector<Car> cars;

    // Конструктор по умолчанию (обязателен для map)
    Train() : number(0), type(TrainType::Lastochka), direction(""), date("") {}

    Train(int num, TrainType t, const string& dir, const string& dt): number(num), type(t), direction(dir), date(dt) {
        createCars();
    }

private:
    void createCars() {
        if (type == TrainType::Lastochka) {
            // 8 вагонов по 100 сидячих мест
            for (int i = 1; i <= 8; ++i) {
                cars.emplace_back(i, CarType::Sitting, 100, SeatType::Sitting);
            }
        }
        else if (type == TrainType::Firm) {
            // 2 СВ (18 нижних), 6 купе (18 верх + 18 низ), 4 плацкарта (27 верх + 27 низ)
            for (int i = 1; i <= 2; ++i) {
                cars.emplace_back(i, CarType::SV, 0, 18);
            }
            for (int i = 3; i <= 8; ++i) {
                cars.emplace_back(i, CarType::Coupe, 18, 18);
            }
            for (int i = 9; i <= 12; ++i) {
                cars.emplace_back(i, CarType::Platzkart, 27, 27);
            }
        }
        else if (type == TrainType::Fast) {
            // 4 купе, 8 плацкарта
            for (int i = 1; i <= 4; ++i) {
                cars.emplace_back(i, CarType::Coupe, 18, 18);
            }
            for (int i = 5; i <= 12; ++i) {
                cars.emplace_back(i, CarType::Platzkart, 27, 27);
            }
        }
    }
};

// Горьковская ж/д
class GorkyRailway {
private:
    map<string, map<int, Train>> schedule;          // дата -> номер поезда -> Train
    map<int, tuple<int, int, vector<int>>> orders;  // id заказа -> (номер поезда, номер вагона, список мест)
    int nextOrderId = 1;

    // стоимость билетов 
    double getTicketPrice(CarType carType, SeatType seatType) const {
        switch (carType) {
        case CarType::Sitting: return 500.0;
        case CarType::Platzkart: return (seatType == SeatType::Up) ? 700.0 : 800.0;
        case CarType::Coupe: return (seatType == SeatType::Up) ? 1000.0 : 1200.0;
        case CarType::SV: return 2000.0;
        default: return 0;
        }
    }

public:
    GorkyRailway() {
        string startDate = getCurrentDate();
        // создание расписания на месяц 
        for (int day = 0; day <= 30; ++day) {
            string date = addDays(startDate, day);
            // НН -> МСК
            schedule[date][1] = Train(1, TrainType::Lastochka, "Нижний Новгород -> Москва", date);
            schedule[date][2] = Train(2, TrainType::Lastochka, "Нижний Новгород -> Москва", date);
            schedule[date][3] = Train(3, TrainType::Lastochka, "Нижний Новгород -> Москва", date);
            schedule[date][4] = Train(4, TrainType::Firm, "Нижний Новгород -> Москва", date);
            schedule[date][5] = Train(5, TrainType::Fast, "Нижний Новгород -> Москва", date);
            // МСК -> НН
            schedule[date][6] = Train(6, TrainType::Lastochka, "Москва -> Нижний Новгород", date);
            schedule[date][7] = Train(7, TrainType::Lastochka, "Москва -> Нижний Новгород", date);
            schedule[date][8] = Train(8, TrainType::Lastochka, "Москва -> Нижний Новгород", date);
            schedule[date][9] = Train(9, TrainType::Firm, "Москва -> Нижний Новгород", date);
            schedule[date][10] = Train(10, TrainType::Fast, "Москва -> Нижний Новгород", date);
        }
    }

    // проверка доступности мест для заданного запроса:
    // возвращает true, и в outCarNumber, outSeatNumbers записывает информацию о вагоне и местах
    bool checkAvailability(const string& date, int trainNumber, CarType requiredCarType,
        const map<SeatType, int>& requiredSeats,
        int& outCarNumber, vector<int>& outSeatNumbers, double& outTotalCost) {
        auto dateIt = schedule.find(date);
        if (dateIt == schedule.end()) return false;
        auto trainIt = dateIt->second.find(trainNumber);
        if (trainIt == dateIt->second.end()) return false;

        Train& train = trainIt->second;
        for (auto& car : train.cars) {
            if (car.type == requiredCarType && car.canReserve(requiredSeats)) {
                outCarNumber = car.number;
                // временно резервируем, чтобы узнать номера мест (для расчёта стоимости)
                auto tempSeats = car.reserve(requiredSeats);
                outSeatNumbers = tempSeats;
                // откат временной резервации
                car.cancelReservation(tempSeats);

                // расчёт стоимости 
                outTotalCost = 0;
                for (int seatNum : outSeatNumbers) {
                    SeatType st = SeatType::Sitting;
                    for (const auto& s : car.seats) {
                        if (s.number == seatNum) {
                            st = s.type;
                            break;
                        }
                    }
                    outTotalCost += getTicketPrice(car.type, st);
                }
                return true;
            }
        }
        return false;
    }

    // резервирование мест, возвращение id заказа 
    int reserveSeats(const string& date, int trainNumber, CarType carType,
        const map<SeatType, int>& requiredSeats,
        const vector<string>& passengerNames,
        vector<tuple<string, int, int, int, string, string, string>>& ticketsInfo) {
        int carNumber = -1;
        vector<int> seatNumbers;
        double cost;
        if (!checkAvailability(date, trainNumber, carType, requiredSeats, carNumber, seatNumbers, cost)) {
            throw runtime_error("Нет свободных мест для указанного запроса");
        }
        if (seatNumbers.size() != passengerNames.size()) {
            throw runtime_error("Количество имён не соответствует количеству мест");
        }

        // находим поезд и вагон, выполняем реальное резервирование
        auto& train = schedule[date][trainNumber];
        Car* targetCar = nullptr;
        for (auto& car : train.cars) {
            if (car.number == carNumber) {
                targetCar = &car;
                break;
            }
        }
        if (!targetCar) throw runtime_error("Вагон не найден");
        vector<int> reservedSeats = targetCar->reserve(requiredSeats);

        // сохраняем заказ 
        int orderId = nextOrderId++;
        orders[orderId] = make_tuple(trainNumber, carNumber, reservedSeats);

        // формируем билет 
        ticketsInfo.clear();
        for (size_t i = 0; i < reservedSeats.size(); ++i) {
            ticketsInfo.emplace_back(date, trainNumber, carNumber, reservedSeats[i],
                passengerNames[i], train.direction.substr(0, train.direction.find(" ->")),
                train.direction.substr(train.direction.find("-> ") + 3));
        }
        return orderId;
    }

    // отмена заказа по id 
    void cancelOrder(int orderId) {
        auto it = orders.find(orderId);
        if (it == orders.end()) {
            throw runtime_error("Заказ не найден");
        }
        
        int trainNumber = get<0>(it->second);
        int carNumber = get<1>(it->second);
        vector<int> seatNumbers = get<2>(it->second);

        bool found = false;
        for (auto& dateEntry : schedule) {
            auto trainIt = dateEntry.second.find(trainNumber);
            if (trainIt != dateEntry.second.end()) {
                for (auto& car : trainIt->second.cars) {
                    if (car.number == carNumber) {
                        car.cancelReservation(seatNumbers);
                        found = true;
                        break;
                    }
                }
                if (found) break;
            }
        }
        if (!found) throw runtime_error("Не удалось найти поезд/вагон для отмены");
        orders.erase(it);
    }

    // gолучить список всех доступных направлений
    vector<string> getDirections() const {
        return { "Нижний Новгород -> Москва", "Москва -> Нижний Новгород" };
    }

    // получить список поездов на дату и направление
    vector<pair<int, string>> getTrains(const string& date, const string& direction) const {
        vector<pair<int, string>> result;
        auto it = schedule.find(date);
        if (it != schedule.end()) {
            for (auto trainIt = it->second.begin(); trainIt != it->second.end(); ++trainIt) {
                int num = trainIt->first;
                const Train& train = trainIt->second;
                if (train.direction == direction) {
                    string typeStr;
                    switch (train.type) {
                    case TrainType::Lastochka: typeStr = "Ласточка"; break;
                    case TrainType::Firm: typeStr = "Фирменный"; break;
                    case TrainType::Fast: typeStr = "Скорый"; break;
                    }
                    result.emplace_back(num, typeStr);
                }
            }
        }
        return result;
    }

    // получить типы вагонов для поезда (в зависимости от типа поезда)
    vector<CarType> getCarTypesForTrain(int trainNumber, const string& date) const {
        auto it = schedule.find(date);
        if (it == schedule.end()) return {};
        auto trainIt = it->second.find(trainNumber);
        if (trainIt == it->second.end()) return {};

        vector<CarType> types;
        if (trainIt->second.type == TrainType::Lastochka) {
            types.push_back(CarType::Sitting);
        }
        else if (trainIt->second.type == TrainType::Firm) {
            types.push_back(CarType::SV);
            types.push_back(CarType::Coupe);
            types.push_back(CarType::Platzkart);
        }
        else if (trainIt->second.type == TrainType::Fast) {
            types.push_back(CarType::Coupe);
            types.push_back(CarType::Platzkart);
        }
        return types;
    }

    // получить возможные типы мест для вагона
    vector<SeatType> getSeatTypesForCar(CarType carType) const {
        if (carType == CarType::Sitting) return { SeatType::Sitting };
        if (carType == CarType::SV) return { SeatType::Down };
        if (carType == CarType::Coupe) return { SeatType::Up, SeatType::Down };
        if (carType == CarType::Platzkart) return { SeatType::Up, SeatType::Down };
        return {};
    }

    string getCarTypeString(CarType ct) const {
        switch (ct) {
        case CarType::Sitting: return "Сидячий";
        case CarType::Platzkart: return "Плацкарт";
        case CarType::Coupe: return "Купе";
        case CarType::SV: return "СВ";
        default: return "";
        }
    }

    string getSeatTypeString(SeatType st) const {
        switch (st) {
        case SeatType::Sitting: return "Сидячее";
        case SeatType::Up: return "Верхнее";
        case SeatType::Down: return "Нижнее";
        default: return "";
        }
    }
};

// ж/д касса 
class RailwayTicketOffice {
private:
    GorkyRailway& railway;

    void clearBuffer() {
        cin.clear();
        cin.ignore(10000, '\n');
    }

    // дата
    string inputDate() {
        string date;
        cout << "Введите дату отправления (ГГГГ-ММ-ДД, в пределах 30 дней от текущей): ";
        cin >> date;
        string today = getCurrentDate();
        if (daysBetween(today, date) < 0 || daysBetween(today, date) > 30) {
            throw runtime_error("Дата должна быть в пределах 30 дней от текущей");
        }
        return date;
    }
    
    // направление 
    string inputDirection(const vector<string>& directions) {
        cout << "Доступные направления:\n";
        for (size_t i = 0; i < directions.size(); ++i) {
            cout << i + 1 << ". " << directions[i] << "\n";
        }
        int choice;
        cout << "Выберите направление (номер): ";
        cin >> choice;
        if (choice < 1 || choice >(int)directions.size()) {
            throw runtime_error("Неверный выбор направления");
        }
        return directions[choice - 1];
    }

    // поезд 
    int selectTrain(const string& date, const string& direction) {
        auto trains = railway.getTrains(date, direction);
        if (trains.empty()) {
            throw runtime_error("Нет поездов на указанную дату и направление");
        }
        cout << "Доступные поезда:\n";
        for (size_t i = 0; i < trains.size(); ++i) {
            cout << i + 1 << ". Номер " << trains[i].first << " (" << trains[i].second << ")\n";
        }
        int choice;
        cout << "Выберите поезд (номер): ";
        cin >> choice;
        if (choice < 1 || choice >(int)trains.size()) {
            throw runtime_error("Неверный выбор поезда");
        }
        return trains[choice - 1].first;
    }

    // тип вагона 
    CarType selectCarType(int trainNumber, const string& date) {
        auto types = railway.getCarTypesForTrain(trainNumber, date);
        if (types.empty()) {
            throw runtime_error("У данного поезда нет вагонов");
        }
        cout << "Типы вагонов:\n";
        for (size_t i = 0; i < types.size(); ++i) {
            cout << i + 1 << ". " << railway.getCarTypeString(types[i]) << "\n";
        }
        int choice;
        cout << "Выберите тип вагона: ";
        cin >> choice;
        if (choice < 1 || choice >(int)types.size()) {
            throw runtime_error("Неверный выбор типа вагона");
        }
        return types[choice - 1];
    }

    // тип места 
    map<SeatType, int> inputSeatQuantities(CarType carType) {
        auto seatTypes = railway.getSeatTypesForCar(carType);
        map<SeatType, int> required;
        cout << "Введите количество билетов для каждого типа мест:\n";
        for (SeatType st : seatTypes) {
            int count;
            cout << railway.getSeatTypeString(st) << ": ";
            cin >> count;
            if (count < 0) throw runtime_error("Количество не может быть отрицательным");
            if (count > 0) required[st] = count;
        }
        if (required.empty()) {
            throw runtime_error("Не выбрано ни одного билета");
        }
        return required;
    }

    // ввод ФИО
    vector<string> inputPassengerNames(int totalSeats) {
        vector<string> names;
        clearBuffer(); // очистка перед getline
        for (int i = 0; i < totalSeats; ++i) {
            cout << "Введите ФИО пассажира для места " << i + 1 << ": ";
            string name;
            getline(cin, name);
            if (name.empty()) throw runtime_error("ФИО не может быть пустым");
            names.push_back(name);
        }
        return names;
    }

public:
    RailwayTicketOffice(GorkyRailway& rw) : railway(rw) {}

    void acceptOrder() {
        cout << "\n--- Приём данных покупателя ---\n";
        string date = inputDate();
        string direction = inputDirection(railway.getDirections());
        int trainNumber = selectTrain(date, direction);
        CarType carType = selectCarType(trainNumber, date);
        auto requiredSeats = inputSeatQuantities(carType);

        int totalSeats = 0;
        for (auto& p : requiredSeats) totalSeats += p.second;

        vector<string> passengerNames = inputPassengerNames(totalSeats);

        int carNumber;
        vector<int> seatNumbers;
        double totalCost;
        if (!railway.checkAvailability(date, trainNumber, carType, requiredSeats, carNumber, seatNumbers, totalCost)) {
            cout << "К сожалению, нет свободных мест, удовлетворяющих запросу.\n";
            return;
        }

        cout << "\nДоступно! Общая стоимость: " << totalCost << " руб.\n";
        cout << "Подтвердить бронирование? (1 - да, 0 - нет): ";
        int confirm;
        cin >> confirm;
        if (confirm != 1) {
            cout << "Бронирование отменено.\n";
            return;
        }

        vector<tuple<string, int, int, int, string, string, string>> ticketsInfo;
        int orderId = railway.reserveSeats(date, trainNumber, carType, requiredSeats, passengerNames, ticketsInfo);
        cout << "Билеты успешно забронированы! Номер заказа: " << orderId << "\n";

        cout << "\n--- Сформированные билеты ---\n";
        for (const auto& t : ticketsInfo) {
            cout << "Дата: " << get<0>(t)
                << ", Поезд №" << get<1>(t)
                << ", Вагон №" << get<2>(t)
                << ", Место №" << get<3>(t)
                << ", Пассажир: " << get<4>(t)
                << ", Отправление: " << get<5>(t)
                << ", Прибытие: " << get<6>(t) << "\n";
        }
    }

    void checkAvailability() {
        cout << "\n--- Проверка наличия мест ---\n";
        string date = inputDate();
        string direction = inputDirection(railway.getDirections());
        int trainNumber = selectTrain(date, direction);
        CarType carType = selectCarType(trainNumber, date);
        auto requiredSeats = inputSeatQuantities(carType);

        int carNumber;
        vector<int> seatNumbers;
        double totalCost;
        if (railway.checkAvailability(date, trainNumber, carType, requiredSeats, carNumber, seatNumbers, totalCost)) {
            cout << "Места есть. Примерная стоимость: " << totalCost << " руб.\n";
        }
        else {
            cout << "Свободных мест для указанного запроса нет.\n";
        }
    }

    void cancelOrder() {
        cout << "\n--- Отмена заказа ---\n";
        int orderId;
        cout << "Введите номер заказа: ";
        cin >> orderId;
        railway.cancelOrder(orderId);
        cout << "Заказ успешно отменён.\n";
    }

    void calculateCost() {
        cout << "\n--- Расчёт стоимости ---\n";
        string date = inputDate();
        string direction = inputDirection(railway.getDirections());
        int trainNumber = selectTrain(date, direction);
        CarType carType = selectCarType(trainNumber, date);
        auto requiredSeats = inputSeatQuantities(carType);

        int carNumber;
        vector<int> seatNumbers;
        double totalCost;
        if (railway.checkAvailability(date, trainNumber, carType, requiredSeats, carNumber, seatNumbers, totalCost)) {
            cout << "Общая стоимость билетов: " << totalCost << " руб.\n";
        }
        else {
            cout << "Невозможно рассчитать стоимость: нет свободных мест.\n";
        }
    }
};

void printMenu() {
    cout << "\n======= ЖЕЛЕЗНОДОРОЖНАЯ КАССА =======\n";
    cout << "1. Принять данные покупателя и зарезервировать билеты\n";
    cout << "2. Проверить наличие свободных мест\n";
    cout << "3. Рассчитать общую стоимость билетов\n";
    cout << "4. Отменить заказ билетов\n";
    cout << "5. Выход\n";
    cout << "Выберите действие: ";
}

int main() {
    setlocale(LC_ALL, "ru_RU.UTF-8");

    GorkyRailway railway;
    RailwayTicketOffice ticketOffice(railway);

    cout << "Добро пожаловать в систему продажи билетов на поезда Горьковской железной дороги\n";

    int choice;
    do {
        printMenu();
        cin >> choice;

        try {
            switch (choice) {
            case 1:
                ticketOffice.acceptOrder();
                break;
            case 2:
                ticketOffice.checkAvailability();
                break;
            case 3:
                ticketOffice.calculateCost();
                break;
            case 4:
                ticketOffice.cancelOrder();
                break;
            case 5:
                cout << "До свидания!\n";
                break;
            default:
                cout << "Неверный выбор. Пожалуйста, выберите пункт от 1 до 5.\n";
            }
        }
        catch (const exception& e) {
            cout << "Ошибка: " << e.what() << "\n";
        }

    } while (choice != 5);

    return 0;
}