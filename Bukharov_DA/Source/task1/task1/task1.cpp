#include <iostream>

using namespace std;

class Time {
private:
	int hours;
	int minutes;
	int seconds;

	void normalize() {
		if (seconds >= 60) {
			minutes += seconds / 60;
			seconds %= 60;
		}
		if (seconds < 0) {
			minutes -= 1;
			seconds += 60;
		}
		if (minutes >= 60) {
			hours += minutes / 60;
			minutes %= 60;
		}
		if (minutes < 0) {
			hours -= 1;
			minutes += 60;
		}
		if (hours >= 24) {
			hours %= 24;
		}
		if (hours < 0) {
			hours += 24;
		}
	}

public:
	Time() {
		hours = 0;
		minutes = 0;
		seconds = 0;
	}

	Time(int h, int m, int s) {
		hours = h;
		minutes = m;
		seconds = s;
		normalize();
	}

	void set_time(int h, int m, int s) {
		hours = h;
		minutes = m;
		seconds = s;
		normalize();
	}

	void print_time() {
		cout << (hours < 10 ? "0" : "") << hours << ":"
			<< (minutes < 10 ? "0" : "") << minutes << ":"
			<< (seconds < 10 ? "0" : "") << seconds;
	}

	void difference(int h, int m, int s) {
		int current_total_seconds = hours * 3600 + minutes * 60 + seconds;
		int new_total_seconds = h * 3600 + m * 60 + s;

		int diff_seconds = new_total_seconds - current_total_seconds;

		int diff_hours = diff_seconds / 3600;
		diff_seconds = diff_seconds % 3600;
		int diff_minutes = diff_seconds / 60;
		diff_seconds = diff_seconds % 60;
		cout << "Difference: " << diff_hours << "hours, "
			<< diff_minutes << "minutes, " << diff_seconds << "seconds" << endl;
	}

	void add(int h, int m, int s) {
		hours += h;
		minutes += m;
		seconds += s;
		normalize();
	}

	void subtract(int h, int m, int s) {
		hours -= h;
		minutes -= m;
		seconds -= s;
		normalize();
	}
};

int main() {
	cout << "=== Time Program ===\n\n";

	Time t1(5, 40, 35);
	cout << "t1 = ";
	t1.print_time();
	cout << endl;

	Time t2(13, 55, 27);
	cout << "t2 = ";
	t2.print_time();
	cout << endl;


	cout << "Difference: t2 - t1 = ";
	t1.difference(13, 55, 27);
	cout << endl;


	cout << "t1 subtract 2 hours 45 minutes 30 seconds: ";
	t1.subtract(2, 45, 30);
	t1.print_time();
	cout << endl;

	cout << "t1 add 3 hours 15 minutes 57 seconds: ";
	t1.add(3, 15, 57);
	t1.print_time();
	cout << endl;

	return 0;
}