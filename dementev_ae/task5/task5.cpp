#include <iostream>
#include <string>
#include <map>
#include <cmath>

using namespace std;

struct Client
{
	string accountNumber;
	string fullName;
	int balance;
	string password;
	bool hasActiveCredit;
	double creditAmount;
	double remainingDebt;
	int creditTermMonths;
	double annualRate;
	int monthsPaid;

	Client() : balance(0), hasActiveCredit(false), creditAmount(0),
			   remainingDebt(0), creditTermMonths(0), annualRate(0), monthsPaid(0) {}

	Client(string acc, string name, int bal, string pwd)
		: accountNumber(acc), fullName(name), balance(bal), password(pwd),
		  hasActiveCredit(false), creditAmount(0), remainingDebt(0),
		  creditTermMonths(0), annualRate(0), monthsPaid(0) {}
};

class ProcessingCenter
{
private:
	map<string, Client> clients;

public:
	ProcessingCenter()
	{
		clients["0001"] = Client("0001", "Иванов Иван Иванович", 50000, "1234");
		clients["0002"] = Client("0002", "Петров Петр Петрович", 200000, "5678");
		clients["0003"] = Client("0003", "Сидорова Анна Сергеевна", 150000, "9012");
		clients["0004"] = Client("0004", "Козлов Дмитрий Алексеевич", 500000, "pass");
	}

	bool authorize(string accountNumber, string password)
	{
		auto it = clients.find(accountNumber);
		if (it != clients.end() && it->second.password == password)
		{
			return true;
		}
		return false;
	}

	Client *getClient(string accountNumber)
	{
		auto it = clients.find(accountNumber);
		if (it != clients.end())
		{
			return &(it->second);
		}
		return nullptr;
	}

	void updateClient(Client client)
	{
		clients[client.accountNumber] = client;
	}

	void showAllClients()
	{
		cout << "\n=== БАЗА КЛИЕНТОВ ПРОЦЕССИНГОВОГО ЦЕНТРА ===\n";
		for (auto &pair : clients)
		{
			cout << "Счет: " << pair.first
				 << " | ФИО: " << pair.second.fullName
				 << " | Баланс: " << pair.second.balance << " руб."
				 << " | Кредит: " << (pair.second.hasActiveCredit ? "да" : "нет");
			if (pair.second.hasActiveCredit)
			{
				cout << " | Остаток: " << pair.second.remainingDebt << " руб.";
			}
			cout << endl;
		}
	}
};

class Credit
{
private:
	ProcessingCenter *center;
	Client *currentClient;
	bool isAuthorized;

	const int validTerms[5] = {1, 2, 3, 5, 15};

	bool isValidTerm(int years)
	{
		for (int i = 0; i < 5; i++)
		{
			if (validTerms[i] == years)
				return true;
		}
		return false;
	}

	double getInterestRate(double amount, int years)
	{
		double rates[20][4] = {
			// до 100 тыс. рублей
			{0, 100000, 1, 18},
			{0, 100000, 2, 17},
			{0, 100000, 3, 16},
			{0, 100000, 5, 15},
			{0, 100000, 15, 14},
			// от 100 до 500 тыс. рублей
			{100000, 500000, 1, 15},
			{100000, 500000, 2, 14},
			{100000, 500000, 3, 13},
			{100000, 500000, 5, 12.5},
			{100000, 500000, 15, 12},
			// от 500 тыс. до 1 млн рублей
			{500000, 1000000, 1, 14},
			{500000, 1000000, 2, 13},
			{500000, 1000000, 3, 12},
			{500000, 1000000, 5, 11.5},
			{500000, 1000000, 15, 11},
			// от 1 до 3 млн рублей
			{1000000, 3000000, 1, 13},
			{1000000, 3000000, 2, 12},
			{1000000, 3000000, 3, 11},
			{1000000, 3000000, 5, 10.5},
			{1000000, 3000000, 15, 10}};

		for (int i = 0; i < 20; i++)
		{
			if (amount >= rates[i][0] && amount <= rates[i][1] && years == (int)rates[i][2])
			{
				return rates[i][3];
			}
		}
		return -1;
	}

	double calculateMonthlyPayment(double amount, double annualRate, int months)
	{
		double monthlyRate = annualRate / 100.0 / 12.0;
		if (monthlyRate == 0)
			return amount / months;
		return amount * monthlyRate * pow(1 + monthlyRate, months) / (pow(1 + monthlyRate, months) - 1);
	}

	bool isApproved(double monthlyPayment, int clientBalance)
	{
		return clientBalance >= (monthlyPayment * 6);
	}

public:
	Credit(ProcessingCenter *pc) : center(pc), currentClient(nullptr), isAuthorized(false) {}

	bool authorizeClient(string accountNumber, string password)
	{
		if (center->authorize(accountNumber, password))
		{
			currentClient = center->getClient(accountNumber);
			isAuthorized = true;
			cout << "\nАВТОРИЗАЦИЯ УСПЕШНА!" << endl;
			cout << "  Добро пожаловать, " << currentClient->fullName << "!" << endl;
			cout << "  Номер счета: " << currentClient->accountNumber << endl;
			cout << "  Баланс счета: " << currentClient->balance << " руб." << endl;
			return true;
		}
		else
		{
			cout << "\nОШИБКА АВТОРИЗАЦИИ!" << endl;
			cout << "  Неверный номер счета или пароль." << endl;
			return false;
		}
	}

	void showAvailableCredits()
	{
		if (!isAuthorized)
		{
			cout << "\n✗ Необходима авторизация!" << endl;
			return;
		}

		cout << "\n=== ДОСТУПНЫЕ КРЕДИТЫ ДЛЯ КЛИЕНТА " << currentClient->accountNumber << " ===" << endl;
		cout << "Текущий баланс: " << currentClient->balance << " руб." << endl;
		cout << "\nСрок\tСумма кредита\t\tСтавка\t\tЕжемес. платеж\tОдобрение" << endl;
		cout << "--------------------------------------------------------------------------------" << endl;

		double ranges[4][2] = {
			{0, 100000},
			{100000, 500000},
			{500000, 1000000},
			{1000000, 3000000}};

		string rangeNames[4] = {
			"до 100 тыс.",
			"100-500 тыс.",
			"500 тыс.-1 млн",
			"1-3 млн"};

		for (int t = 0; t < 5; t++)
		{
			int years = validTerms[t];
			for (int r = 0; r < 4; r++)
			{
				double testAmount = (ranges[r][0] + ranges[r][1]) / 2;
				if (testAmount < ranges[r][0])
					testAmount = ranges[r][0] + 10000;
				if (testAmount > ranges[r][1])
					testAmount = ranges[r][1];

				double rate = getInterestRate(testAmount, years);
				if (rate > 0)
				{
					double monthlyPayment = calculateMonthlyPayment(testAmount, rate, years * 12);
					bool approved = isApproved(monthlyPayment, currentClient->balance);

					cout << years << " лет\t";
					printf("%-15s", rangeNames[r].c_str());
					printf("\t%.1f%%\t\t", rate);
					printf("%.0f руб.\t", monthlyPayment);
					cout << (approved ? " МОЖНО" : " НЕЛЬЗЯ") << endl;
				}
			}
		}
		cout << endl;
	}

	void checkExistingCredit()
	{
		if (!isAuthorized)
		{
			cout << "\n Необходима авторизация!" << endl;
			return;
		}

		cout << "\n=== ПРОВЕРКА НАЛИЧИЯ КРЕДИТА ===" << endl;
		if (currentClient->hasActiveCredit)
		{
			cout << " У вас есть активный кредит!" << endl;
			cout << "  Сумма кредита: " << currentClient->creditAmount << " руб." << endl;
			cout << "  Остаток долга: " << currentClient->remainingDebt << " руб." << endl;
			cout << "  Срок кредита: " << currentClient->creditTermMonths << " месяцев" << endl;
			cout << "  Процентная ставка: " << currentClient->annualRate << "% годовых" << endl;
			cout << "  Оплачено месяцев: " << currentClient->monthsPaid << endl;
			cout << "  Осталось месяцев: " << (currentClient->creditTermMonths - currentClient->monthsPaid) << endl;
		}
		else
		{
			cout << " У вас нет активных кредитов." << endl;
		}
	}

	bool checkCreditEligibility(double amount, int years)
	{
		if (!isAuthorized)
		{
			cout << "\n Необходима авторизация!" << endl;
			return false;
		}

		cout << "\n=== ПРОВЕРКА ВОЗМОЖНОСТИ ПОЛУЧЕНИЯ КРЕДИТА ===" << endl;
		cout << "Запрошено: " << amount << " руб. на " << years << " лет" << endl;

		if (currentClient->hasActiveCredit)
		{
			cout << "Отказано: у вас уже есть активный кредит!" << endl;
			return false;
		}
		if (!isValidTerm(years))
		{
			cout << "Отказано: недопустимый срок кредита!" << endl;
			cout << "  Допустимые сроки: 1, 2, 3, 5, 15 лет" << endl;
			return false;
		}

		double rate = getInterestRate(amount, years);
		if (rate < 0)
		{
			cout << " Отказано: недопустимая сумма кредита!" << endl;
			cout << "  Допустимые диапазоны: до 100 тыс., 100-500 тыс., 500 тыс.-1 млн, 1-3 млн руб." << endl;
			return false;
		}

		int months = years * 12;
		double monthlyPayment = calculateMonthlyPayment(amount, rate, months);

		if (isApproved(monthlyPayment, currentClient->balance))
		{
			cout << " КРЕДИТ МОЖЕТ БЫТЬ ОДОБРЕН!" << endl;
			cout << "  Процентная ставка: " << rate << "% годовых" << endl;
			cout << "  Ежемесячный платеж: " << monthlyPayment << " руб." << endl;
			cout << "  Требуется для одобрения (6 платежей): " << (monthlyPayment * 6) << " руб." << endl;
			cout << "  Ваш баланс: " << currentClient->balance << " руб. - ДОСТАТОЧНО" << endl;
			return true;
		}
		else
		{
			cout << " КРЕДИТ НЕ ОДОБРЕН!" << endl;
			cout << "  Процентная ставка: " << rate << "% годовых" << endl;
			cout << "  Ежемесячный платеж: " << monthlyPayment << " руб." << endl;
			cout << "  Требуется для одобрения (6 платежей): " << (monthlyPayment * 6) << " руб." << endl;
			cout << "  Ваш баланс: " << currentClient->balance << " руб. - НЕДОСТАТОЧНО" << endl;
			cout << "  Не хватает: " << (monthlyPayment * 6 - currentClient->balance) << " руб." << endl;
			return false;
		}
	}

	bool takeCredit(double amount, int years)
	{
		if (!isAuthorized)
		{
			cout << "\n Необходима авторизация!" << endl;
			return false;
		}

		cout << "\n=== ОФОРМЛЕНИЕ КРЕДИТА ===" << endl;

		if (!checkCreditEligibility(amount, years))
		{
			return false;
		}

		double rate = getInterestRate(amount, years);
		int months = years * 12;
		double monthlyPayment = calculateMonthlyPayment(amount, rate, months);

		currentClient->hasActiveCredit = true;
		currentClient->creditAmount = amount;
		currentClient->remainingDebt = amount;
		currentClient->creditTermMonths = months;
		currentClient->annualRate = rate;
		currentClient->monthsPaid = 0;

		currentClient->balance += amount;

		center->updateClient(*currentClient);

		cout << "\n КРЕДИТ ОДОБРЕН И ВЫДАН! " << endl;
		cout << "  Сумма кредита: " << amount << " руб." << endl;
		cout << "  Срок: " << years << " лет (" << months << " месяцев)" << endl;
		cout << "  Процентная ставка: " << rate << "% годовых" << endl;
		cout << "  Ежемесячный платеж: " << monthlyPayment << " руб." << endl;
		cout << "  Сумма " << amount << " руб. зачислена на ваш счет." << endl;
		cout << "  Новый баланс счета: " << currentClient->balance << " руб." << endl;

		return true;
	}

	void showCreditState()
	{
		if (!isAuthorized)
		{
			cout << "\n Необходима авторизация!" << endl;
			return;
		}

		cout << "\n=== ТЕКУЩЕЕ СОСТОЯНИЕ КРЕДИТА ===" << endl;

		if (!currentClient->hasActiveCredit)
		{
			cout << "У вас нет активного кредита." << endl;
			return;
		}

		double monthlyRate = currentClient->annualRate / 100.0 / 12.0;
		double accruedInterest = currentClient->remainingDebt * monthlyRate;
		double minPayment = calculateMonthlyPayment(currentClient->creditAmount,
													currentClient->annualRate,
													currentClient->creditTermMonths);

		cout << "  Начальная сумма кредита: " << currentClient->creditAmount << " руб." << endl;
		cout << "  Остаток основного долга: " << currentClient->remainingDebt << " руб." << endl;
		cout << "  Процентная ставка: " << currentClient->annualRate << "% годовых" << endl;
		cout << "  Начислено процентов за текущий месяц: " << accruedInterest << " руб." << endl;
		cout << "  Минимальный ежемесячный платеж: " << minPayment << " руб." << endl;
		cout << "  Всего оплачено месяцев: " << currentClient->monthsPaid << endl;
		cout << "  Осталось месяцев: " << (currentClient->creditTermMonths - currentClient->monthsPaid) << endl;

		double totalPaid = minPayment * currentClient->monthsPaid;
		double overpayment = totalPaid - (currentClient->creditAmount - currentClient->remainingDebt);
		cout << "  Примерная переплата на данный момент: " << overpayment << " руб." << endl;
	}

	bool repayCredit(double amount)
	{
		if (!isAuthorized)
		{
			cout << "\n Необходима авторизация!" << endl;
			return false;
		}

		cout << "\n=== ПОГАШЕНИЕ КРЕДИТА ===" << endl;

		if (!currentClient->hasActiveCredit)
		{
			cout << " У вас нет активного кредита!" << endl;
			return false;
		}

		double minPayment = calculateMonthlyPayment(currentClient->creditAmount,
													currentClient->annualRate,
													currentClient->creditTermMonths);
		double monthlyRate = currentClient->annualRate / 100.0 / 12.0;
		double accruedInterest = currentClient->remainingDebt * monthlyRate;

		cout << "  Минимальный платеж за месяц: " << minPayment << " руб." << endl;
		cout << "  Начислено процентов за месяц: " << accruedInterest << " руб." << endl;
		cout << "  Вы вносите: " << amount << " руб." << endl;

		if (amount < minPayment)
		{
			cout << " ОШИБКА: Сумма платежа меньше минимальной!" << endl;
			cout << "  Минимальный платеж: " << minPayment << " руб." << endl;
			return false;
		}

		if (currentClient->balance < amount)
		{
			cout << " ОШИБКА: Недостаточно средств на счете!" << endl;
			cout << "  Доступно: " << currentClient->balance << " руб." << endl;
			cout << "  Требуется: " << amount << " руб." << endl;
			return false;
		}

		currentClient->balance -= amount;

		double paysInterest = accruedInterest;
		double paysPrincipal = amount - accruedInterest;

		if (paysPrincipal > currentClient->remainingDebt)
		{
			paysPrincipal = currentClient->remainingDebt;
			paysInterest = amount - paysPrincipal;
		}

		currentClient->remainingDebt -= paysPrincipal;
		currentClient->monthsPaid++;

		cout << "  Платеж принят!" << endl;
		cout << "  Из них проценты: " << paysInterest << " руб." << endl;
		cout << "  Погашение тела долга: " << paysPrincipal << " руб." << endl;
		cout << "  Остаток долга: " << currentClient->remainingDebt << " руб." << endl;

		if (currentClient->remainingDebt <= 0.01 ||
			currentClient->monthsPaid >= currentClient->creditTermMonths)
		{
			currentClient->hasActiveCredit = false;
			currentClient->remainingDebt = 0;
			cout << "\n ПОЗДРАВЛЯЕМ! КРЕДИТ ПОЛНОСТЬЮ ПОГАШЕН! " << endl;
		}
		else if (amount > minPayment)
		{
			cout << "   Внесена сумма больше минимальной. Вы переплачиваете по кредиту!" << endl;
		}

		center->updateClient(*currentClient);

		cout << "  Новый баланс счета: " << currentClient->balance << " руб." << endl;
		return true;
	}

	bool earlyRepayCredit()
	{
		if (!isAuthorized)
		{
			cout << "\n Необходима авторизация!" << endl;
			return false;
		}

		cout << "\n=== ДОСРОЧНОЕ ПОГАШЕНИЕ КРЕДИТА ===" << endl;

		if (!currentClient->hasActiveCredit)
		{
			cout << " У вас нет активного кредита!" << endl;
			return false;
		}

		double monthlyRate = currentClient->annualRate / 100.0 / 12.0;
		double accruedInterest = currentClient->remainingDebt * monthlyRate;
		double totalToPay = currentClient->remainingDebt + accruedInterest;

		cout << "  Остаток основного долга: " << currentClient->remainingDebt << " руб." << endl;
		cout << "  Начислено процентов за текущий месяц: " << accruedInterest << " руб." << endl;
		cout << "  Итого к оплате: " << totalToPay << " руб." << endl;

		if (currentClient->balance < totalToPay)
		{
			cout << " НЕДОСТАТОЧНО СРЕДСТВ ДЛЯ ДОСРОЧНОГО ПОГАШЕНИЯ!" << endl;
			cout << "  Доступно: " << currentClient->balance << " руб." << endl;
			cout << "  Требуется: " << totalToPay << " руб." << endl;
			cout << "  Не хватает: " << (totalToPay - currentClient->balance) << " руб." << endl;
			return false;
		}

		double minPayment = calculateMonthlyPayment(currentClient->creditAmount,
													currentClient->annualRate,
													currentClient->creditTermMonths);
		int remainingMonths = currentClient->creditTermMonths - currentClient->monthsPaid;
		double wouldHavePaid = minPayment * remainingMonths;
		double saved = wouldHavePaid - currentClient->remainingDebt;

		currentClient->balance -= totalToPay;

		double oldDebt = currentClient->remainingDebt;
		currentClient->hasActiveCredit = false;
		currentClient->remainingDebt = 0;
		currentClient->monthsPaid = currentClient->creditTermMonths;

		center->updateClient(*currentClient);

		cout << "\n КРЕДИТ ДОСРОЧНО ПОГАШЕН! " << endl;
		cout << "  Погашено: " << totalToPay << " руб." << endl;
		cout << "  Сэкономлено на процентах: ~" << saved << " руб." << endl;
		cout << "  Новый баланс счета: " << currentClient->balance << " руб." << endl;

		return true;
	}

	void logout()
	{
		isAuthorized = false;
		currentClient = nullptr;
		cout << "\nВы вышли из системы." << endl;
	}
};

int main()
{
	ProcessingCenter center;
	Credit credit(&center);

	cout << "================================================" << endl;
	cout << "   ИНТЕРНЕТ-БАНК - УПРАВЛЕНИЕ КРЕДИТАМИ" << endl;
	cout << "================================================" << endl;

	center.showAllClients();

	cout << "\n================================================" << endl;
	cout << "1. АВТОРИЗАЦИЯ КЛИЕНТА" << endl;
	cout << "================================================";
	credit.authorizeClient("0001", "1234");

	cout << "\n================================================" << endl;
	cout << "2. ПРОВЕРКА НАЛИЧИЯ КРЕДИТА" << endl;
	cout << "================================================";
	credit.checkExistingCredit();

	cout << "\n================================================" << endl;
	cout << "3. ДОСТУПНЫЕ КРЕДИТЫ" << endl;
	cout << "================================================";
	credit.showAvailableCredits();

	cout << "\n================================================" << endl;
	cout << "4. ПРОВЕРКА ВОЗМОЖНОСТИ ПОЛУЧЕНИЯ КРЕДИТА" << endl;
	cout << "================================================";
	credit.checkCreditEligibility(100000, 1);

	cout << "\n================================================" << endl;
	cout << "5. ПОЛУЧЕНИЕ КРЕДИТА" << endl;
	cout << "================================================";
	credit.takeCredit(100000, 1);

	cout << "\n================================================" << endl;
	cout << "6. ТЕКУЩЕЕ СОСТОЯНИЕ КРЕДИТА" << endl;
	cout << "================================================";
	credit.showCreditState();

	cout << "\n================================================" << endl;
	cout << "7. ПОГАШЕНИЕ КРЕДИТА" << endl;
	cout << "================================================";
	credit.repayCredit(9000);

	cout << "\n================================================" << endl;
	cout << "СОСТОЯНИЕ ПОСЛЕ ПЛАТЕЖА" << endl;
	cout << "================================================";
	credit.showCreditState();

	cout << "\n================================================" << endl;
	cout << "8. ДОСРОЧНОЕ ПОГАШЕНИЕ КРЕДИТА" << endl;
	cout << "================================================";
	credit.earlyRepayCredit();

	cout << "\n================================================" << endl;
	cout << "ИТОГОВОЕ СОСТОЯНИЕ" << endl;
	cout << "================================================";
	center.showAllClients();

	return 0;
}