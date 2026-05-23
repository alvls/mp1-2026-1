#include <iostream>
#include <vector>
#include <fstream>
#include <cmath>
#include <sstream>

class scales {
private:
    int FirstDay, FirstMonth, FirstYear;
    const double INACCURACY_WEIGHT = 0.05;

    struct Observation {
        int day, month, year;
        std::string name;
        double weight;
    };
    std::vector<Observation> observations;

public:
    scales() : FirstDay(0), FirstMonth(0), FirstYear(0) {}

    void SetFirstDate(int day, int month, int year) {
        FirstDay = day;
        FirstMonth = month;
        FirstYear = year;
    }
    void GetFirstDate(int& day, int& month, int& year) const {
        day = FirstDay;
        month = FirstMonth;
        year = FirstYear;
    }

    //изменение SetObservation
    void SetObservation(int day, int month, int year, const std::string& name, double weight) {
        double rounded_weight = std::round(weight / INACCURACY_WEIGHT) * INACCURACY_WEIGHT;

        for (const scales::Observation& obs : observations) {
            if (obs.day == day && obs.month == month && obs.year == year && obs.name == name) {
                obs.weight = rounded_weight;
                return;
            }
        }
        observations.push_back({ day, month, year, name, rounded_weight });
    }

    bool GetWeight(int day, int month, int year, const std::string& name, double& weight) const {
        for (const scales::Observation& obs : observations) {
            if (obs.day == day && obs.month == month && obs.year == year && obs.name == name) {
                weight = obs.weight;
                return true;
            }
        }
        return false;
    }

    bool AverageWeight(const std::string& name, double& average_weight, int month = 0, int year = 0) const {
        double sum = 0.0;
        int count = 0;

        for (const scales::Observation& obs : observations) {
            if (obs.name == name) {
                if ((month == 0 && year == 0) || (obs.month == month && obs.year == year)) {
                    sum += obs.weight;
                    count++;
                }
            }
        }

        if (count > 0) {
            average_weight = sum / count;
            return true;
        }
        return false;
    }

    bool MinMaxWeight(const std::string& name,
        double& min_weight, int& min_day, int& min_month, int& min_year,
        double& max_weight, int& max_day, int& max_month, int& max_year,
        int filter_month = 0, int filter_year = 0) const {

        bool found = false;
        int count = 0;

        for (const scales::Observation& obs : observations) {
            if (obs.name == name && ((filter_month == 0 && filter_year == 0) || (obs.month == filter_month && obs.year == filter_year))) {

                if (!found) {
                    min_weight = max_weight = obs.weight;
                    min_day = max_day = obs.day;
                    min_month = max_month = obs.month;
                    min_year = max_year = obs.year;
                    found = true;
                }
                else {
                    if (obs.weight < min_weight) {
                        min_weight = obs.weight;
                        min_day = obs.day;
                        min_month = obs.month;
                        min_year = obs.year;
                    }
                    if (obs.weight > max_weight) {
                        max_weight = obs.weight;
                        max_day = obs.day;
                        max_month = obs.month;
                        max_year = obs.year;
                    }
                }
                count++;
            }
        }
        return count;
    }
    bool SaveHistoryToFile(const std::string& filename) const {
        std::ofstream file(filename);

        if (!file.is_open()) {
            return false;
        }

        for (const scales::Observation& obs : observations) {
            file << obs.day << "." << obs.month << "." << obs.year << ": "
                << obs.name << " - " << obs.weight << " кг" << std::endl;
        }

        file.close();
        return true;
    }

    bool LoadHistory(const std::string& filename) {

        std::ifstream file(filename);

        if (!file.is_open()) {
            return false;
        }
        std::string line;
        while (std::getline(file, line)) {
            std::istringstream iss(line);

            int day, month, year;
            double weight;
            std::string name;

            char separator1, separator2;

            if (iss >> day >> separator1 >> month >> separator1 >> year >> separator2 >> name >> separator2 >> weight) {
                observations.push_back({ day, month, year, name, weight });
            }
        }
        return true;
    }
};
int main() {
    scales family_scales;

    family_scales.SetFirstDate(1, 1, 2026);

    family_scales.SetObservation(1, 4, 2026, "Иван", 75.123);
    family_scales.SetObservation(5, 4, 2026, "Мария", 60.4);
    family_scales.SetObservation(10, 4, 2026, "Иван", 75.6);
    family_scales.SetObservation(15, 4, 2026, "Иван", 74.6);
    family_scales.SetObservation(20, 4, 2026, "Иван", 75.5);
    family_scales.SetObservation(10, 5, 2026, "Иван", 76.0);

    family_scales.SetObservation(10, 4, 2026, "Иван", 75.8); //обновить данные Ивана за 10 апреля

    double weight; //получить вес по дате
    if (family_scales.GetWeight(15, 4, 2026, "Иван", weight)) {
        std::cout << "Вес Ивана 15.04.2026: " << weight << " кг\n";
    }
    else {
        std::cout << "Error.\n";
    }

    // вычислить средний вес за апрель
    double avg_weight;
    if (family_scales.AverageWeight("Иван", avg_weight, 4, 2026)) {
        std::cout << "Средний вес: " << avg_weight << " кг\n";
    }
    else {
        std::cout << "Error.\n";
    }

    // минимальный и максимальный вес за апрель
    double min_w, max_w;
    int min_d, min_m, min_y;
    int max_d, max_m, max_y;

    if (family_scales.MinMaxWeight("Иван", min_w, min_d, min_m, min_y, max_w, max_d, max_m, max_y, 4, 2026)) {
        std::cout << "Минимальный вес: " << min_w << " кг (" << min_d << "." << min_m << "." << min_y << ")\n";
        std::cout << "Максимальный вес: " << max_w << " кг (" << max_d << "." << max_m << "." << max_y << ")\n";
    }
    else {
        std::cout << "Error.\n";
    }

    // сохранить историю в файл
    if (family_scales.SaveHistoryToFile("history.txt")) {
        std::cout << "Right.\n";

        family_scales = scales();

        // загрузить данные из файла
        if (family_scales.LoadHistory("history.txt")) {
            std::cout << "Right.\n";

            if (family_scales.GetWeight(10, 4, 2026, "Иван", weight)) {
                std::cout << "Вес Ивана 10.04.2026: " << weight << " кг\n";
            }
        }
        else {
            std::cout << "Error.\n";
        }

    }
    else {
        std::cout << "Error.\n";
    }
    return 0;
}