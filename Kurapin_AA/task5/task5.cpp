#include <iostream>
#include <windows.h>
#include <string>
#include <chrono>
#include <unordered_map>

class GRR {
public:
	struct Carriage {
	public:
		size_t upper;
		size_t lower;
		std::vector<bool> upper_seats;
		std::vector<bool> lower_seats;
		std::string type;
		int price;
		std::unordered_map<std::string, int> seat_price = {
			{"upper", 30},
			{"lower", 50},
		};

		Carriage(size_t upper_t, size_t lower_t, std::string _type, int _price) : upper{ upper_t }, lower{ lower_t }, upper_seats(upper_t, false), lower_seats(lower_t, false), type(_type), price(_price) {}

		void Occupy(size_t seat) {
			if (seat <= upper) {
				upper_seats[seat - 1] = true;
			}
			else {
				lower_seats[seat - upper - 1] = true;
			}
		}

		void Unoccupy(size_t seat) {
			if (seat <= upper) {
				upper_seats[seat - 1] = false;
			}
			else {
				lower_seats[seat - upper - 1] = false;
			}
		}

		bool is_Occupied(size_t seat) {
			if (seat <= upper) {
				return upper_seats[seat - 1];
			}
			else {
				return lower_seats[seat - upper - 1];
			}
		}

		std::string Seat_Type(size_t seat) {
			if (seat <= upper) {
				return "upper";
			}
			else return "lower";
		}
	};

	inline static const std::unordered_map<std::string, std::vector<std::pair<std::string, std::string>>> carriage_types = {
		{ "LAS_C", { {"Нижние места", "1-100"}}},
		{ "SL", { { "Нижние места", "1-18"} }},
		{ "COM", { { "Верхние места", "1-18"}, {"Нижние места", "19-36"}}},
		{ "RS", { { "Верхние места", "1-27"}, {"Нижние места", "28-54"}}},
	};

	inline static const Carriage LAS_C = { 0, 100, "LAS_C", 100 };
	inline static const Carriage SL = { 0, 18, "SL", 300};
	inline static const Carriage COM = { 18, 18, "COM", 200};
	inline static const Carriage RS = { 27, 27, "RS", 150};

	struct Timetable {
		std::string departure_station;
		std::string destination;

		Timetable(std::string _departure_station, std::string _destination) : departure_station(_departure_station), destination(_destination) {}

		std::string to_string() const {
			return departure_station + "-" + destination;
		}
	};

	struct Train {
		std::vector<Carriage> carriages;
		std::string type;
		int price;

		Train(std::initializer_list<std::pair<size_t, Carriage>> values, std::string _type, int _price) : type(_type), price(_price) {
			size_t total = 0;
			for (const auto& pair : values) {
				total += pair.first;
			}
			carriages.reserve(total);

			for (const auto& pair : values) {
				carriages.insert(carriages.end(), pair.first, pair.second);
			}
		}
	};

	inline static const std::unordered_map<std::string, std::vector<std::pair<std::string, std::string>>> train_types = {
		{ "LAS", { {"Ласточка", "1-8"}}},
		{ "FIR", { { "СВ", "1-2"}, {"Купейные", "3-8"}, {"Плацкарт", "9-12"}}},
		{ "SCO", { { "Купейные", "1-4"}, {"Плацкарт", "5-12"}}},
	};

	inline static const Train LAS = { { { 8, LAS_C}}, "LAS", 200};
	inline static const Train FIR = { { { 2, SL }, { 6, COM }, { 4, RS } }, "FIR", 400};
	inline static const Train SCO = { { { 4, COM }, { 8, RS } }, "SCO", 500};

	inline static std::vector<std::pair<Train, Timetable>> trains = {
		{ LAS, Timetable("Moscow", "Nizh")}, { LAS, Timetable("Moscow", "Nizh")}, { LAS, Timetable("Moscow", "Nizh")}, 
		{ LAS, Timetable("Nizh", "Moscow")}, { LAS, Timetable("Nizh", "Moscow")}, { LAS, Timetable("Nizh", "Moscow")}, 
		{ FIR, Timetable("Moscow", "Nizh")}, { FIR, Timetable("Nizh", "Moscow")}, 
		{ SCO, Timetable("Moscow", "Nizh")}, { SCO, Timetable("Nizh", "Moscow")},
	};

	struct Ticket {
		std::chrono::year_month_day date;
		size_t train;
		size_t carriage;
		size_t seat;
		std::string name;
		std::string departure_station;
		std::string destination;

		Ticket(int year, unsigned month, unsigned day, size_t _train, size_t _carriage, size_t _seat, std::string _name, std::string _departure_station, std::string _destination) : date(std::chrono::year_month_day{
				std::chrono::year{year},
				std::chrono::month{month},
				std::chrono::day{day}
			}),
			train(_train - 1), carriage(_carriage - 1),  seat(_seat - 1), name(_name), departure_station(_departure_station), destination(_destination)
		{ }

		std::string to_string() const {
			return "\nДата: " + std::format("{}", date) +
				"\nНомер поезда: " + std::to_string((int)train + 1) +
				"\nНомер вагона: " + std::to_string((int)carriage + 1) +
				"\nМесто: " + std::to_string((int)seat + 1) +
				"\nИмя пассажира: " + name +
				"\nСтанция отправки: " + departure_station +
				"\nСтанция прибытия: " + destination;
		}
	};

	inline static std::vector<Ticket> tickets;
};

class Ticket_Interface {
public:
	std::vector<GRR::Ticket> ticket_drafts;

	void Make_Drafts() {
		char mode = '1';

		while (mode != '0') {
			std::cout << "Вводим данные для нового билета\n";

			std::cout << "Введите дату в формате 31:12:2000\n";
			int year;
			unsigned month;
			unsigned day;
			char colon;
			std::cin >> day >> colon >> month >> colon >> year;

			std::cout << "Введите номер поезда (";
			for (size_t index = 0; index < GRR::trains.size(); index++) {
				std::cout << index + 1 << ":" << GRR::trains[index].first.type << " " << GRR::trains[index].second.to_string() << ", ";
			}
			std::cout << ")\n";
			size_t train;
			std::cin >> train;
			std::string train_type = GRR::trains[train - 1].first.type;

			for (const auto& [key, value] : GRR::train_types) {
				if (train_type == key) {
					std::cout << "Введите номер вагона (";
					for (size_t index = 0; index < value.size(); index++) {
						std::cout << "Вагоны " << value[index].first << ": " << value[index].second << ", ";
					}
					std::cout << ")\n";
				}
			}
			size_t carriage;
			std::cin >> carriage;
			std::string carriage_type = GRR::trains[train - 1].first.carriages[carriage - 1].type;

			for (const auto& [key, value] : GRR::carriage_types) {
				if (carriage_type == key) {
					std::cout << "Введите номер места (";
					for (size_t index = 0; index < value.size(); index++) {
						std::cout << value[index].first << ": " << value[index].second << ", ";
					}
					std::cout << ")\n";
				}
			}
			size_t seat;
			std::cin >> seat;

			std::cout << "Введите имя пассажира\n";
			std::string name;
			std::cin >> name;

			std::string departure_station = GRR::trains[train - 1].second.departure_station;
			std::string destination = GRR::trains[train - 1].second.destination;

			ticket_drafts.push_back(GRR::Ticket(year, month, day, train, carriage, seat, name, departure_station, destination));

			std::cout << "Чтобы закончить напишите 0, чтобы продолжить напишите 1\n";
			std::cin >> mode;
			std::cin.ignore();
		}
	}

	bool Seats_Available() const {
		if (ticket_drafts.empty()) return false;

		for (size_t index = 0; index < ticket_drafts.size(); index++) {
			if (GRR::trains[ticket_drafts[index].train].first.carriages[ticket_drafts[index].carriage].is_Occupied(ticket_drafts[index].seat)) return false;
		}

		return true;
	}

	void Book_Seats() const {
		if (Seats_Available()) {
			for (size_t index = 0; index < ticket_drafts.size(); index++) {
				GRR::trains[ticket_drafts[index].train].first.carriages[ticket_drafts[index].carriage].Occupy(ticket_drafts[index].seat);
			}
		}
	}

	void Cancel_Seats() const {
		for (size_t index = 0; index < ticket_drafts.size(); index++) {
			GRR::trains[ticket_drafts[index].train].first.carriages[ticket_drafts[index].carriage].Unoccupy(ticket_drafts[index].seat);
		}
	}

	int Tickets_Price() const {
		int price = 0;

		if (Seats_Available()) {
			for (size_t index = 0; index < ticket_drafts.size(); index++) {
				price += GRR::trains[ticket_drafts[index].train].first.price;
				price += GRR::trains[ticket_drafts[index].train].first.carriages[ticket_drafts[index].carriage].price;

				std::string seat_type = GRR::trains[ticket_drafts[index].train].first.carriages[ticket_drafts[index].carriage].Seat_Type(ticket_drafts[index].seat);
				price += GRR::trains[ticket_drafts[index].train].first.carriages[ticket_drafts[index].carriage].seat_price[seat_type];
			}
		}

		return price;
	}

	void Cancel_Drafts() {
		Cancel_Seats();
		ticket_drafts.clear();
		ticket_drafts.shrink_to_fit();
	}

	void Confirm_Tickets() {
		if (Seats_Available()) {
			Book_Seats();

			GRR::tickets.insert(GRR::tickets.end(), ticket_drafts.begin(), ticket_drafts.end());
			ticket_drafts.clear();
			ticket_drafts.shrink_to_fit();
		}
	}

	std::string Show_Drafts() const {
		std::string result = "";

		for (size_t index = 0; index < ticket_drafts.size(); index++) {
			result += ticket_drafts[index].to_string() + "\n";
		}

		return result;
	}

	std::string Show_Tickets() const {
		std::string result = "";

		for (size_t index = 0; index < GRR::tickets.size(); index++) {
			result += GRR::tickets[index].to_string() + "\n";
		}

		return result;
	}
};

int main() {
	SetConsoleOutputCP(1251);
	SetConsoleCP(1251);

	Ticket_Interface user_interface = Ticket_Interface();

	user_interface.Make_Drafts();
	std::cout << user_interface.Show_Drafts() << std::endl;

	std::cout << user_interface.Seats_Available();
	std::cout << user_interface.Tickets_Price();

	user_interface.Confirm_Tickets();
	std::cout << user_interface.Show_Tickets();
	getchar();
}
