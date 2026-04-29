#include <iostream>
#include <string>
#include <cmath>
#include <locale.h>

using namespace std;

struct CreditInfo {
    bool hasCredit = false;
    double amount = 0;
    int months = 0;
    double ratePerYear = 0;
    double monthlyPayment = 0;
    double remainingDebt = 0;
    int monthsPaid = 0;
};

struct Client {
    string accountNumber;
    string fullName;
    double salaryAmount;
    CreditInfo credit;
    string password;
};

class ProtsessingCentr;

class Credit {
private:
    ProtsessingCentr* center;

    int terms[5] = { 1, 2, 3, 5, 15 };

    double rates[5][4] = {
        {0.10, 0.15, 0.18, 0.20},
        {0.11, 0.14, 0.17, 0.19},
        {0.12, 0.14, 0.16, 0.18},
        {0.13, 0.15, 0.16, 0.17},
        {0.14, 0.15, 0.16, 0.17}
    };

    int getAmountRange(double amount) {
        if (amount < 100000) return 0;
        if (amount < 500000) return 1;
        if (amount < 1000000) return 2;
        if (amount <= 3000000) return 3;
        return -1;
    }

    int getTermIndex(int years) {
        for (int i = 0; i < 5; i++) {
            if (terms[i] == years) return i;
        }
        return -1;
    }

    double calculateMonthlyPayment(double amount, double ratePerYear, int months) {
        double monthlyRate = ratePerYear / 12;
        if (monthlyRate == 0) return amount / months;
        double factor = pow(1 + monthlyRate, months);
        return amount * monthlyRate * factor / (factor - 1);
    }

public:
    Credit(ProtsessingCentr* c) : center(c) {}

    Client* authorize(string accountNumber, string password);
    void showAvailableCredits(Client* client);
    bool hasCredit(Client* client);
    bool checkCreditPossibility(Client* client, double amount, int years);
    bool takeCredit(Client* client, double amount, int years);
    void showCreditStatus(Client* client);
    bool payMonthly(Client* client, double paymentAmount);
    bool earlyRepayment(Client* client);
};

class ProtsessingCentr {
private:
    const int MAX_CLIENTS = 100;
    Client clients[100];
    int clientCount;

    int findClientIndex(const string& accountNumber) {
        for (int i = 0; i < clientCount; i++) {
            if (clients[i].accountNumber == accountNumber) {
                return i;
            }
        }
        return -1;
    }

public:
    ProtsessingCentr() : clientCount(0) {}

    bool addClient(const string& accNum, const string& name,
        double salary, const string& pwd) {
        if (clientCount >= MAX_CLIENTS) return false;
        if (pwd.length() <= 3) return false;

        clients[clientCount].accountNumber = accNum;
        clients[clientCount].fullName = name;
        clients[clientCount].salaryAmount = salary;
        clients[clientCount].password = pwd;
        clients[clientCount].credit = CreditInfo();
        clientCount++;
        return true;
    }

    bool checkPassword(const string& accountNumber, const string& pwd) {
        int idx = findClientIndex(accountNumber);
        if (idx == -1) return false;
        return clients[idx].password == pwd;
    }

    Client* getClient(const string& accountNumber) {
        int idx = findClientIndex(accountNumber);
        if (idx == -1) return nullptr;
        return &clients[idx];
    }

    bool updateClient(const Client& client) {
        int idx = findClientIndex(client.accountNumber);
        if (idx == -1) return false;
        clients[idx] = client;
        return true;
    }
};

Client* Credit::authorize(string accountNumber, string password) {
    if (!center->checkPassword(accountNumber, password)) {
        cout << "Ошибка авторизации: неверный номер счета или пароль\n";
        return nullptr;
    }
    Client* client = center->getClient(accountNumber);
    cout << "Авторизация успешна. Добро пожаловать, " << client->fullName << "!\n";
    return client;
}

void Credit::showAvailableCredits(Client* client) {
    if (!client) {
        cout << "Клиент не авторизован\n";
        return;
    }

    cout << "\n=== Доступные кредитные предложения ===\n";
    cout << "Срок (лет)\tСумма\t\tСтавка (% годовых)\n";

    string rangeNames[4] = { "до 100 тыс.", "100-500 тыс.", "500 тыс.-1 млн", "1-3 млн" };

    for (int t = 0; t < 5; t++) {
        for (int r = 0; r < 4; r++) {
            cout << terms[t] << "\t\t" << rangeNames[r] << "\t\t" << rates[t][r] * 100 << "%\n";
        }
        cout << "-----------------------------------\n";
    }
}

bool Credit::hasCredit(Client* client) {
    if (!client) return false;
    return client->credit.hasCredit;
}

bool Credit::checkCreditPossibility(Client* client, double amount, int years) {
    if (!client) {
        cout << "Клиент не авторизован\n";
        return false;
    }

    int rangeIdx = getAmountRange(amount);
    if (rangeIdx == -1) {
        cout << "Сумма кредита должна быть от 0 до 3 млн рублей\n";
        return false;
    }

    int termIdx = getTermIndex(years);
    if (termIdx == -1) {
        cout << "Срок кредита должен быть: 1, 2, 3, 5 или 15 лет\n";
        return false;
    }

    double rate = rates[termIdx][rangeIdx];
    int months = years * 12;
    double monthlyPayment = calculateMonthlyPayment(amount, rate, months);

    double requiredForSix = monthlyPayment * 6;

    if (client->salaryAmount < requiredForSix) {
        cout << "Кредит не одобрен. На счету недостаточно средств для 6 ежемесячных выплат.\n";
        cout << "Необходимо: " << requiredForSix << " руб., на счету: " << client->salaryAmount << " руб.\n";
        return false;
    }

    cout << "Кредит может быть одобрен. Ежемесячный платёж составит: " << monthlyPayment << " руб.\n";
    return true;
}

bool Credit::takeCredit(Client* client, double amount, int years) {
    if (!client) {
        cout << "Клиент не авторизован\n";
        return false;
    }

    if (client->credit.hasCredit) {
        cout << "У клиента уже есть активный кредит\n";
        return false;
    }

    if (!checkCreditPossibility(client, amount, years)) {
        return false;
    }

    int rangeIdx = getAmountRange(amount);
    int termIdx = getTermIndex(years);
    double rate = rates[termIdx][rangeIdx];
    int months = years * 12;
    double monthlyPayment = calculateMonthlyPayment(amount, rate, months);

    client->credit.hasCredit = true;
    client->credit.amount = amount;
    client->credit.months = months;
    client->credit.ratePerYear = rate;
    client->credit.monthlyPayment = monthlyPayment;
    client->credit.remainingDebt = amount;
    client->credit.monthsPaid = 0;

    client->salaryAmount += amount;

    center->updateClient(*client);

    cout << "Кредит одобрен и выдан!\n";
    cout << "Сумма " << amount << " руб. перечислена на ваш счёт.\n";
    cout << "Ежемесячный платёж: " << monthlyPayment << " руб.\n";
    cout << "Процентная ставка: " << rate * 100 << "% годовых\n";

    return true;
}

void Credit::showCreditStatus(Client* client) {
    if (!client) {
        cout << "Клиент не авторизован\n";
        return;
    }

    if (!client->credit.hasCredit) {
        cout << "У клиента нет активного кредита\n";
        return;
    }

    cout << "\n=== Состояние кредита ===\n";
    cout << "Сумма кредита: " << client->credit.amount << " руб.\n";
    cout << "Остаток долга: " << client->credit.remainingDebt << " руб.\n";
    cout << "Срок кредита: " << client->credit.months / 12 << " лет (" << client->credit.months << " мес.)\n";
    cout << "Оплачено месяцев: " << client->credit.monthsPaid << "\n";
    cout << "Осталось месяцев: " << client->credit.months - client->credit.monthsPaid << "\n";
    cout << "Ежемесячный платёж: " << client->credit.monthlyPayment << " руб.\n";
    cout << "Годовая ставка: " << client->credit.ratePerYear * 100 << "%\n";
}

bool Credit::payMonthly(Client* client, double paymentAmount) {
    if (!client) {
        cout << "Клиент не авторизован\n";
        return false;
    }

    if (!client->credit.hasCredit) {
        cout << "Нет активного кредита\n";
        return false;
    }

    if (paymentAmount < client->credit.monthlyPayment) {
        cout << "Сумма платежа меньше начисленной (" << client->credit.monthlyPayment << " руб.)\n";
        return false;
    }

    if (paymentAmount > client->salaryAmount) {
        cout << "Недостаточно средств на счету\n";
        return false;
    }

    client->salaryAmount -= paymentAmount;

    double overpayment = paymentAmount - client->credit.monthlyPayment;
    double principalPaid = client->credit.monthlyPayment;

    if (overpayment > 0) {
        principalPaid += overpayment;
        cout << "Внесена переплата " << overpayment << " руб. - она пойдёт на досрочное погашение основного долга\n";
    }

    client->credit.remainingDebt -= principalPaid;
    client->credit.monthsPaid++;

    if (client->credit.remainingDebt <= 0.01) {
        client->credit.hasCredit = false;
        cout << "Кредит полностью погашен!\n";
    }
    else {
        cout << "Платёж принят. Остаток долга: " << client->credit.remainingDebt << " руб.\n";
    }

    center->updateClient(*client);
    return true;
}

bool Credit::earlyRepayment(Client* client) {
    if (!client) {
        cout << "Клиент не авторизован\n";
        return false;
    }

    if (!client->credit.hasCredit) {
        cout << "Нет активного кредита\n";
        return false;
    }

    double remaining = client->credit.remainingDebt;

    if (client->salaryAmount < remaining) {
        cout << "Недостаточно средств для досрочного погашения\n";
        cout << "Требуется: " << remaining << " руб., на счету: " << client->salaryAmount << " руб.\n";
        return false;
    }

    client->salaryAmount -= remaining;
    client->credit.remainingDebt = 0;
    client->credit.hasCredit = false;

    center->updateClient(*client);

    cout << "Кредит досрочно погашен! Сумма: " << remaining << " руб.\n";
    return true;
}

int main() {
    setlocale(LC_ALL, "Russian");
    ProtsessingCentr center;
    Credit credit(&center);

    center.addClient("0001", "Иванов Иван Иванович", 500000, "pass1234");
    center.addClient("0002", "Петров Петр Петрович", 30000, "mypass");

    Client* client = credit.authorize("0001", "pass1234");

    if (client) {
        credit.showAvailableCredits(client);

        if (!credit.hasCredit(client)) {
            credit.checkCreditPossibility(client, 300000, 3);
            credit.takeCredit(client, 300000, 3);
        }

        credit.showCreditStatus(client);
        credit.payMonthly(client, 15000);
        credit.showCreditStatus(client);
    }

    return 0;
}