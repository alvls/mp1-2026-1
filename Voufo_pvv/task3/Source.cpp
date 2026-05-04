#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <tuple>

class Time {
private:
    int hours_;
    int minutes_;
    int seconds_;

    static bool isValid(int h, int m, int s) {
        return (h >= 0 && h <= 23) && (m >= 0 && m <= 59) && (s >= 0 && s <= 59);
    }

    static int toTotalSeconds(int h, int m, int s) {
        return h * 3600 + m * 60 + s;
    }

    static std::tuple<int, int, int> fromTotalSeconds(int totalSeconds) {
        // Keep value in one day range [0, 86399]
        const int day = 24 * 3600;
        totalSeconds = ((totalSeconds % day) + day) % day;

        int h = totalSeconds / 3600;
        totalSeconds %= 3600;
        int m = totalSeconds / 60;
        int s = totalSeconds % 60;
        return {h, m, s};
    }

public:
    // Default constructor: 00:00:00
    Time() : hours_(0), minutes_(0), seconds_(0) {}

    // Constructor with values
    Time(int h, int m, int s) {
        setTime(h, m, s);
    }

    // 1) Set time
    void setTime(int h, int m, int s) {
        if (!isValid(h, m, s)) {
            throw std::invalid_argument("Time must be in range 00:00:00 to 23:59:59");
        }
        hours_ = h;
        minutes_ = m;
        seconds_ = s;
    }

    // 2) Get time
    std::tuple<int, int, int> getTime() const {
        return {hours_, minutes_, seconds_};
    }

    // 3) Difference between this (set) time and another time
    // Returns absolute difference as (hours, minutes, seconds)
    std::tuple<int, int, int> difference(int h, int m, int s) const {
        if (!isValid(h, m, s)) {
            throw std::invalid_argument("Given time must be in range 00:00:00 to 23:59:59");
        }

        int current = toTotalSeconds(hours_, minutes_, seconds_);
        int given = toTotalSeconds(h, m, s);

        int diff = current - given;
        if (diff < 0) {
            diff = -diff;
        }

        int dh = diff / 3600;
        diff %= 3600;
        int dm = diff / 60;
        int ds = diff % 60;

        return {dh, dm, ds};
    }

    // 4) Shift time forward by (h, m, s)
    void shiftForward(int h, int m, int s) {
        int current = toTotalSeconds(hours_, minutes_, seconds_);
        int offset = toTotalSeconds(h, m, s);
        auto [nh, nm, ns] = fromTotalSeconds(current + offset);
        hours_ = nh;
        minutes_ = nm;
        seconds_ = ns;
    }

    // 4) Shift time backward by (h, m, s)
    void shiftBackward(int h, int m, int s) {
        int current = toTotalSeconds(hours_, minutes_, seconds_);
        int offset = toTotalSeconds(h, m, s);
        auto [nh, nm, ns] = fromTotalSeconds(current - offset);
        hours_ = nh;
        minutes_ = nm;
        seconds_ = ns;
    }

    // Console output support
    friend std::ostream& operator<<(std::ostream& os, const Time& t) {
        os << std::setfill('0') << std::setw(2) << t.hours_ << ':' << std::setw(2) << t.minutes_ << ':'
           << std::setw(2) << t.seconds_;
        return os;
    }
};

int main() {
    Time t(10, 20, 30);
    std::cout << "Initial time: " << t << '\n';

    t.setTime(23, 59, 50);
    std::cout << "After setTime: " << t << '\n';

    auto [h, m, s] = t.getTime();
    std::cout << "getTime -> " << h << ':' << m << ':' << s << '\n';

    auto [dh, dm, ds] = t.difference(22, 30, 10);
    std::cout << "Difference with 22:30:10 -> " << dh << "h " << dm << "m " << ds << "s\n";

    t.shiftForward(0, 0, 15);
    std::cout << "After shiftForward(0,0,15): " << t << '\n';

    t.shiftBackward(1, 0, 20);
    std::cout << "After shiftBackward(1,0,20): " << t << '\n';

    return 0;
}
