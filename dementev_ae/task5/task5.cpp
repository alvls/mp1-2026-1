#include <iostream>
#include <string>
#include <map>
#include <cmath>
#include <vector>

using namespace std;

struct Client
{
	string acc, name, pwd;
	int balance;
	bool hasCredit = false;
	double creditAmount = 0, remainingDebt = 0, annualRate = 0;
	int creditMonths = 0, monthsPaid = 0;

	Client(string a, string n, int b, string p) : acc(a), name(n), balance(b), pwd(p) {}
	Client() {}
};

class ProcessingCenter
{
	map<string, Client> clients;

public:
	ProcessingCenter()
	{
		clients["0001"] = Client("0001", "Иванов Иван", 50000, "1234");
		clients["0002"] = Client("0002", "Петров Петр", 200000, "5678");
		clients["0003"] = Client("0003", "Сидорова Анна", 150000, "9012");
		clients["0004"] = Client("0004", "Козлов Дмитрий", 500000, "pass");
	}

	bool auth(string acc, string pwd)
	{
		auto it = clients.find(acc);
		return it != clients.end() && it->second.pwd == pwd;
	}

	Client *getClient(string acc)
	{
		auto it = clients.find(acc);
		return it != clients.end() ? &it->second : nullptr;
	}

	void update(Client c) { clients[c.acc] = c; }

	map<string, Client> &getAll() { return clients; }
};

class Credit
{
	ProcessingCenter *center;
	Client *current = nullptr;
	bool authorized = false;

	double getRate(double amount, int years)
	{
		double rates[][4] = {{0, 100000, 1, 18}, {0, 100000, 2, 17}, {0, 100000, 3, 16}, {0, 100000, 5, 15}, {0, 100000, 15, 14}, {100000, 500000, 1, 15}, {100000, 500000, 2, 14}, {100000, 500000, 3, 13}, {100000, 500000, 5, 12.5}, {100000, 500000, 15, 12}, {500000, 1000000, 1, 14}, {500000, 1000000, 2, 13}, {500000, 1000000, 3, 12}, {500000, 1000000, 5, 11.5}, {500000, 1000000, 15, 11}, {1000000, 3000000, 1, 13}, {1000000, 3000000, 2, 12}, {1000000, 3000000, 3, 11}, {1000000, 3000000, 5, 10.5}, {1000000, 3000000, 15, 10}};
		for (auto &r : rates)
			if (amount >= r[0] && amount <= r[1] && years == (int)r[2])
				return r[3];
		return -1;
	}

	double monthlyPayment(double amount, double rate, int months)
	{
		double mr = rate / 100 / 12;
		return mr == 0 ? amount / months : amount * mr * pow(1 + mr, months) / (pow(1 + mr, months) - 1);
	}

public:
	Credit(ProcessingCenter *pc) : center(pc) {}

	bool login(string acc, string pwd)
	{
		if (center->auth(acc, pwd))
		{
			current = center->getClient(acc);
			authorized = true;
			return true;
		}
		return false;
	}

	bool isAuthorized() { return authorized; }
	Client *getCurrent() { return current; }

	bool hasActiveCredit()
	{
		return authorized && current->hasCredit;
	}

	double getRemainingDebt()
	{
		return authorized && current->hasCredit ? current->remainingDebt : 0;
	}

	double getCreditAmount()
	{
		return authorized && current->hasCredit ? current->creditAmount : 0;
	}

	double getAnnualRate()
	{
		return authorized && current->hasCredit ? current->annualRate : 0;
	}

	int getMonthsPaid()
	{
		return authorized && current->hasCredit ? current->monthsPaid : 0;
	}

	int getCreditMonths()
	{
		return authorized && current->hasCredit ? current->creditMonths : 0;
	}

	struct AvailableCredit
	{
		int years;
		string rangeName;
		double amount;
		double rate;
		double monthlyPayment;
		bool approved;
	};

	vector<AvailableCredit> getAvailableCredits()
	{
		vector<AvailableCredit> result;
		if (!authorized)
			return result;

		int terms[] = {1, 2, 3, 5, 15};
		double ranges[][2] = {{0, 100000}, {100000, 500000}, {500000, 1000000}, {1000000, 3000000}};
		string names[] = {"do 100k", "100-500k", "500k-1mln", "1-3mln"};

		for (int t : terms)
		{
			for (int i = 0; i < 4; i++)
			{
				double amount = (ranges[i][0] + ranges[i][1]) / 2;
				double rate = getRate(amount, t);
				if (rate > 0)
				{
					double mp = monthlyPayment(amount, rate, t * 12);
					result.push_back({t, names[i], amount, rate, mp, current->balance >= mp * 6});
				}
			}
		}
		return result;
	}

	struct EligibilityResult
	{
		bool eligible;
		double rate;
		double monthlyPayment;
		double requiredBalance;
		string reason;
	};

	EligibilityResult checkEligibility(double amount, int years)
	{
		if (!authorized)
			return {false, 0, 0, 0, "No authorization"};
		if (current->hasCredit)
			return {false, 0, 0, 0, "Active credit exists"};

		double rate = getRate(amount, years);
		if (rate < 0)
			return {false, 0, 0, 0, "Invalid amount"};

		int months = years * 12;
		double mp = monthlyPayment(amount, rate, months);
		double need = mp * 6;

		if (current->balance >= need)
		{
			return {true, rate, mp, need, ""};
		}
		else
		{
			return {false, rate, mp, need, "Insufficient funds"};
		}
	}

	struct TakeResult
	{
		bool success;
		double rate;
		double monthlyPayment;
		string error;
	};

	TakeResult takeCredit(double amount, int years)
	{
		if (!authorized)
			return {false, 0, 0, "No authorization"};
		if (current->hasCredit)
			return {false, 0, 0, "Active credit exists"};

		double rate = getRate(amount, years);
		if (rate < 0)
			return {false, 0, 0, "Invalid amount"};

		int months = years * 12;
		double mp = monthlyPayment(amount, rate, months);
		if (current->balance < mp * 6)
			return {false, rate, mp, "Insufficient funds"};

		current->hasCredit = true;
		current->creditAmount = amount;
		current->remainingDebt = amount;
		current->creditMonths = months;
		current->annualRate = rate;
		current->balance += amount;
		center->update(*current);

		return {true, rate, mp, ""};
	}

	struct PaymentResult
	{
		bool success;
		double interest;
		double principal;
		double newDebt;
		bool fullyPaid;
		string error;
	};

	PaymentResult repayCredit(double amount)
	{
		if (!authorized || !current->hasCredit)
			return {false, 0, 0, 0, false, "No active credit"};

		double mr = current->annualRate / 100 / 12;
		double interest = current->remainingDebt * mr;
		double minPmt = monthlyPayment(current->creditAmount, current->annualRate, current->creditMonths);

		if (amount < minPmt)
			return {false, 0, 0, 0, false, "Amount less than minimum payment"};
		if (current->balance < amount)
			return {false, 0, 0, 0, false, "Insufficient funds"};

		current->balance -= amount;
		double principal = amount - interest;
		if (principal > current->remainingDebt)
			principal = current->remainingDebt;
		if (principal < 0)
			principal = 0;

		current->remainingDebt -= principal;
		current->monthsPaid++;

		bool fullyPaid = current->remainingDebt <= 0.01;
		if (fullyPaid)
		{
			current->hasCredit = false;
			current->remainingDebt = 0;
		}

		center->update(*current);
		return {true, interest, principal, current->remainingDebt, fullyPaid, ""};
	}

	struct EarlyRepayResult
	{
		bool success;
		double totalAmount;
		double saved;
		string error;
	};

	EarlyRepayResult earlyRepay()
	{
		if (!authorized || !current->hasCredit)
			return {false, 0, 0, "No active credit"};

		double mr = current->annualRate / 100 / 12;
		double interest = current->remainingDebt * mr;
		double total = current->remainingDebt + interest;

		if (current->balance < total)
			return {false, total, 0, "Insufficient funds"};

		double mp = monthlyPayment(current->creditAmount, current->annualRate, current->creditMonths);
		int remaining = current->creditMonths - current->monthsPaid;
		double saved = mp * remaining - current->remainingDebt;

		current->balance -= total;
		current->hasCredit = false;
		current->remainingDebt = 0;
		current->monthsPaid = current->creditMonths;
		center->update(*current);

		return {true, total, saved, ""};
	}

	void logout()
	{
		authorized = false;
		current = nullptr;
	}
};

class ConsoleView
{
public:
	static void showHeader()
	{
		cout << "========================================\n";
		cout << "   INTERNET BANK - CREDIT MANAGEMENT\n";
		cout << "========================================\n";
	}

	static void showAllClients(ProcessingCenter &center)
	{
		cout << "\n=== CLIENTS ===\n";
		for (auto &p : center.getAll())
		{
			cout << p.first << " | " << p.second.name << " | Balance: " << p.second.balance
				 << " | Credit: " << (p.second.hasCredit ? "yes (" + to_string((int)p.second.remainingDebt) + ")" : "no") << endl;
		}
	}

	static void showLoginResult(bool success, Client *client)
	{
		if (success)
		{
			cout << "\nAUTHORIZATION SUCCESSFUL\n";
			cout << "  " << client->name << " | Account: " << client->acc << " | Balance: " << client->balance << " rub.\n";
		}
		else
		{
			cout << "\nAUTHORIZATION ERROR\n";
		}
	}

	static void showExistingCredit(Credit &credit)
	{
		cout << "\n=== CREDIT CHECK ===\n";
		if (credit.hasActiveCredit())
		{
			cout << "  Active credit:\n";
			cout << "  Amount: " << credit.getCreditAmount() << " rub.\n";
			cout << "  Remaining: " << (int)credit.getRemainingDebt() << " rub.\n";
			cout << "  Rate: " << credit.getAnnualRate() << "%\n";
			cout << "  Paid " << credit.getMonthsPaid() << " of " << credit.getCreditMonths() << " months\n";
		}
		else
		{
			cout << "  No active credits\n";
		}
	}

	static void showAvailableCredits(Credit &credit)
	{
		if (!credit.isAuthorized())
			return;

		cout << "\n=== AVAILABLE CREDITS ===\n";
		cout << "Balance: " << credit.getCurrent()->balance << " rub.\n";

		auto credits = credit.getAvailableCredits();
		for (auto &c : credits)
		{
			cout << c.years << "y " << c.rangeName << " | rate " << c.rate << "% | payment " << (int)c.monthlyPayment
				 << " rub. | " << (c.approved ? "YES" : "NO") << endl;
		}
	}

	static void showEligibilityCheck(double amount, int years, Credit::EligibilityResult &res, Credit &credit)
	{
		cout << "\n=== CREDIT ELIGIBILITY ===\n";
		cout << "  Amount: " << amount << " rub. for " << years << " years\n";

		if (!res.eligible)
		{
			cout << "  REJECTED: " << res.reason << "\n";
			if (res.rate > 0)
			{
				cout << "  Rate: " << res.rate << "% | Payment: " << (int)res.monthlyPayment << " rub.\n";
				cout << "  Required: " << (int)res.requiredBalance << " rub. | Balance: "
					 << (credit.isAuthorized() ? credit.getCurrent()->balance : 0) << " rub.\n";
			}
		}
		else
		{
			cout << "  CAN BE APPROVED\n";
			cout << "  Rate: " << res.rate << "% | Payment: " << (int)res.monthlyPayment << " rub.\n";
			cout << "  Required: " << (int)res.requiredBalance << " rub. | Balance sufficient\n";
		}
	}

	static void showTakeResult(double amount, int years, Credit::TakeResult &res, Credit &credit)
	{
		if (!res.success)
		{
			cout << "\nFAILED: " << res.error << "\n";
			return;
		}

		cout << "\nCREDIT APPROVED AND ISSUED\n";
		cout << "  Amount: " << amount << " rub. for " << years << " years (" << years * 12 << " months)\n";
		cout << "  Rate: " << res.rate << "% | Payment: " << (int)res.monthlyPayment << " rub.\n";
		cout << "  New balance: " << (credit.isAuthorized() ? credit.getCurrent()->balance : 0) << " rub.\n";
	}

	static void showCreditState(Credit &credit)
	{
		cout << "\n=== CREDIT STATE ===\n";
		if (!credit.hasActiveCredit())
		{
			cout << "  No active credit\n";
			return;
		}

		double mr = credit.getAnnualRate() / 100 / 12;
		double interest = credit.getRemainingDebt() * mr;

		cout << "  Remaining debt: " << (int)credit.getRemainingDebt() << " rub.\n";
		cout << "  Monthly interest: " << (int)interest << " rub.\n";
		cout << "  Paid " << credit.getMonthsPaid() << " of " << credit.getCreditMonths() << " months\n";
	}

	static void showPaymentResult(double amount, Credit::PaymentResult &res)
	{
		cout << "\n=== CREDIT REPAYMENT ===\n";

		if (!res.success)
		{
			cout << "  ERROR: " << res.error << "\n";
			return;
		}

		cout << "  Payment accepted\n";
		cout << "  Interest: " << (int)res.interest << " rub. | Principal: " << (int)res.principal << " rub.\n";
		cout << "  Remaining debt: " << (int)res.newDebt << " rub.\n";

		if (res.fullyPaid)
		{
			cout << "\n  CONGRATULATIONS! CREDIT FULLY REPAID\n";
		}
	}

	static void showEarlyRepayResult(Credit::EarlyRepayResult &res, Credit &credit)
	{
		cout << "\n=== EARLY REPAYMENT ===\n";

		if (!res.success)
		{
			cout << "  ERROR: " << res.error;
			if (res.totalAmount > 0)
				cout << " (need " << (int)res.totalAmount << " rub.)";
			cout << endl;
			return;
		}

		cout << "  CREDIT EARLY REPAID\n";
		cout << "  Paid: " << (int)res.totalAmount << " rub.\n";
		cout << "  Interest saved: ~" << (int)res.saved << " rub.\n";
		cout << "  New balance: " << (credit.isAuthorized() ? credit.getCurrent()->balance : 0) << " rub.\n";
	}

	static void showLogout()
	{
		cout << "\nLogged out\n";
	}
};

int main()
{
	ProcessingCenter center;
	Credit credit(&center);

	ConsoleView::showHeader();
	ConsoleView::showAllClients(center);

	bool loginSuccess = credit.login("0001", "1234");
	ConsoleView::showLoginResult(loginSuccess, credit.getCurrent());

	if (loginSuccess)
	{
		ConsoleView::showExistingCredit(credit);
		ConsoleView::showAvailableCredits(credit);

		auto eligibility = credit.checkEligibility(100000, 1);
		ConsoleView::showEligibilityCheck(100000, 1, eligibility, credit);

		auto takeResult = credit.takeCredit(100000, 1);
		ConsoleView::showTakeResult(100000, 1, takeResult, credit);

		ConsoleView::showCreditState(credit);

		auto paymentResult = credit.repayCredit(9000);
		ConsoleView::showPaymentResult(9000, paymentResult);

		ConsoleView::showCreditState(credit);

		auto earlyResult = credit.earlyRepay();
		ConsoleView::showEarlyRepayResult(earlyResult, credit);

		credit.logout();
		ConsoleView::showLogout();
	}

	cout << "\n=== FINAL STATE ===\n";
	ConsoleView::showAllClients(center);

	return 0;
}