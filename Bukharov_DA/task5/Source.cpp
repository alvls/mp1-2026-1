#include <iostream>
#include <string>
#include <vector>

using namespace std;

struct Place {
	int row;
	int number;
	bool isVip;
	bool isFree;
};

struct Session {
	int day;
	int hour;
	int minute;
	string movieName;
	int hallNumber;
	double basePrice;
	vector<Place> places;
};


class Cinema {
public:
	vector<Session> sessions;

	Cinema() {
		int rows[] = { 8, 10, 12 };
		int seatsPerRow[] = { 10, 12, 15 };
		int vipRows[] = { 2, 3, 3 };

		double basePrice[] = { 300, 350, 400 };

		string movies[] = { "Avatar", "Oppenheimer", "Titanic", "Dune", "Interstellar" };

		for (int day = 1; day <= 30; day++) {
			int hours[] = { 10, 13, 16, 19, 22 };
			int minutes[] = { 0, 0, 0, 0, 0 };

			for (int t = 0; t < 5; t++) {
				for (int hall = 0; hall < 3; hall++) {
					Session s;
					s.day = day;
					s.hour = hours[t];
					s.minute = minutes[t];
					s.movieName = movies[(day + hall + t) % 5];
					s.hallNumber = hall + 1;
					s.basePrice = basePrice[hall];

					int totalSeats = rows[hall] * seatsPerRow[hall];
					for (int row = 1; row <= rows[hall]; row++) {
						for (int seatNum = 1; seatNum <= seatsPerRow[hall]; seatNum++) {
							Place p;
							p.row = row;
							p.number = seatNum;
							p.isVip = (row > rows[hall] - vipRows[hall]);
							p.isFree = true;
							s.places.push_back(p);
						}
					}
					sessions.push_back(s);
				}
			}
		}
	}

	int findSession(int day, int hour, int minute, string movieName, int hallNumber) {
		for (int i = 0; i < sessions.size(); i++) {
			if (sessions[i].day == day &&
				sessions[i].minute == minute &&
				sessions[i].movieName == movieName &&
				sessions[i].hallNumber == hallNumber) {
				return i;
			}
		}
		return -1;
	}
	double getPrice(int sessionIndex) {
		Session& s = sessions[sessionIndex];
		double price = s.basePrice;

		if (s.hour < 12) {
			price = price * 0.75;
		}
		else if (s.hour > 18) {
			price = price * 1.5;
		}
		return price;
	}
	bool canSell(int sessionIndex, int currentDay, int currentHour, int currentMinute) {
		Session& s = sessions[sessionIndex];

		if (s.day > currentDay) return true;
		if (s.day == currentDay) {
			int sessionStart = s.hour * 60 + s.minute;
			int currentTime = currentHour * 60 + currentMinute;

			return currentTime <= sessionStart + 10;
		}
		return false;
	}
 };

class TicketOffice {
private:
	Cinema& cinema;

	int currentSessionIndex;
	string currentZone;
	int currentSeatsCount;
	vector<int> selectedPlaceIndexes;
	double currentTotalPrice;
	bool hasOrder;

public:

	TicketOffice(Cinema& c) : cinema(c), hasOrder(false) {}

	void acceptOrder() {
		cout << "\n=== ORDER FORMATION ==\n";

		cout << "Enter day (1-3 from today): ";
		cin >> currentSessionIndex;
		int day = currentSessionIndex;

		if (day < 1 || day > 3) {
			cout << "ERROR: Tickets can only be sold for sessions within 3 days from today!\n";
			hasOrder = false;
			return;
		}

		cout << "Enter start hour (0-23): ";
		int hour;
		cin >> hour;

		cout << "Enter start minutes: ";
		int minute;
		cin >> minute;

		cout << "Enter movie name: ";
		cin.ignore();
		string movieName;
		getline(cin, movieName);

		cout << "Enter hall number (1-3): ";
		int hallNumber;
		cin >> hallNumber;

		cout << "Select zone (vip or regular): ";
		cin >> currentZone;

		cout << "How many tickets needed: ";
		cin >> currentSeatsCount;

		currentSessionIndex = cinema.findSession(day, hour, minute, movieName, hallNumber);
		if (currentSessionIndex == -1) {
			cout << "ERROR: Session not found!\n";
			hasOrder = false;
			return;
		}
		hasOrder = true;
		selectedPlaceIndexes.clear();
		cout << "ORder accepted!\n";
	}

	bool checkAvailability() {
		if (!hasOrder) {
			cout << "No active order\n";
			return false;
		}

		Session& session = cinema.sessions[currentSessionIndex];

		if (!cinema.canSell(currentSessionIndex, 1, 12, 0)) {
			cout << "Ticket sales for this session have stopped\n";
			return false;
		}
		selectedPlaceIndexes.clear();

		for (int i = 0; i < session.places.size(); i++) {
			bool zoneMatches = false;
			if (currentZone == "vip" && session.places[i].isVip) zoneMatches = true;
			if (currentZone == "regular" && !session.places[i].isVip) zoneMatches = true;

			if (zoneMatches && session.places[i].isFree) {
				selectedPlaceIndexes.push_back(i);
				if (selectedPlaceIndexes.size() == currentSeatsCount) {
					break;
				}
			}
		}

		if (selectedPlaceIndexes.size() == currentSeatsCount) {
			cout << "Seats are available\n";
			return true;
		}
		else {
			cout << "Sorry, no free seats in " << currentZone << " zone\n";
			return false;
		}
	}

	void reserveSeats() {
		if (!hasOrder) {
			cout << "No active order\n";
			return;
		}
		double pricePerTicket = cinema.getPrice(currentSessionIndex);

		if (currentZone == "vip") {
			pricePerTicket = pricePerTicket * 1.5;
		}
		currentTotalPrice = pricePerTicket * currentSeatsCount;
		cout << "Total cost: " << currentTotalPrice << " rubles\n";
	}

	void calculatePrice() {
		if (!hasOrder) {
			cout << "No active order\n";
			return;
		}
		double pricePerTicket = cinema.getPrice(currentSessionIndex);
		if (currentZone == "vip") {
			pricePerTicket = pricePerTicket * 1.5;
		}
		currentTotalPrice = pricePerTicket * currentSeatsCount;
		cout << "Total cost: " << currentTotalPrice << " rubles\n";
	}

	void cancelOrder() {
		if (!hasOrder) {
			cout << "No active order\n";
			return;
		}
		if (!selectedPlaceIndexes.empty()) {
			Session& session = cinema.sessions[currentSessionIndex];
			for (int i = 0; i < selectedPlaceIndexes.size(); i++) {
				int placeIndex = selectedPlaceIndexes[i];
				session.places[placeIndex].isFree = true;
			}
			cout << "Reservation cancelled, seats are freed\n";
		}
		hasOrder = false;
		selectedPlaceIndexes.clear();
		cout << "Order cancelled\n";
	}

	void printTickets() {
		if (!hasOrder || selectedPlaceIndexes.empty()) {
			cout << "No tickets to print\n";
			return;
		}
		Session& session = cinema.sessions[currentSessionIndex];

		cout << "\n=========================================\n";
		cout << "              YOUR TICKETS\n";
		cout << "\n=========================================\n";
		cout << "Movie: " << session.movieName << endl;
		cout << "Date: day " << session.day << " from today\n";
		cout << "Time: " << session.hour << ":" << session.minute << "0\n";
		cout << "Hall: " << session.hallNumber << endl;
		cout << "Zone: " << currentZone << endl;
		cout << "Cost: " << currentTotalPrice << " rubles\n";
		cout << "----------------------------------------\n";

		for (int i = 0; i < selectedPlaceIndexes.size(); i++) {
			int placeIndex = selectedPlaceIndexes[i];
			Place& p = session.places[placeIndex];
			cout << "Ticket " << i + 1 << ": Row" << p.row << ", Seat " << p.number << endl;
		}
		cout << "\n=========================================\n";
		cout << "      Thank you for your purchase!\n";
		cout << "\n=========================================\n";
		hasOrder = false;
		selectedPlaceIndexes.clear();
 	}
};

int main() {
	Cinema cinema;

	TicketOffice office(cinema);

	int choice;

	do {
		cout << "\n==== TICKET OFFICE ====\n";
		cout << "1. Buy tickets\n";
		cout << "2. Cancel order\n";
		cout << "0. Exit\n";
		cout << "Choose: ";
		cin >> choice;

		if (choice == 1) {
			office.acceptOrder();
			if (office.checkAvailability()) {
				office.calculatePrice();
				cout << "Reserve eats? (1-yes, 0-no): ";
				int confirm;
				cin >> confirm;
				if (confirm == 1) {
					office.reserveSeats();
					office.printTickets();
				}
				else {
					office.cancelOrder();
				}
			}
			else {
				office.cancelOrder();
			}
		}
		else if (choice == 2) {
			office.cancelOrder();
		}
	} while (choice != 0);

	return 0;
}