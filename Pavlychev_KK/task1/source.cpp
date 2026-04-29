#include <numeric>
#include <iostream>
#include <locale.h>
using namespace std;
int nod(int x1, int x2) {
	x1 = abs(x1);
	x2 = abs(x2);
	while (x2) {
		int a = x2;
		x2 = x1 % x2;
		x1 = a;
	}
	return x1;
}
class racion
{
	int chis;
	int znam;
	void socr()
	{
		if (znam == 0) {
			cout << "знаменатель не может быть равен 0." << endl;
			exit(1);
		}
		int n = nod(chis, znam);
		if (n > 1) {
			chis /= n;
			znam /= n;
		}

	}
public:
	racion() {
		chis = 0;
		znam = 1;
	}
	racion(int c) {
		chis = c;
		znam = 1;
		socr();

	}
	racion(int c, int z)
	{
		chis = c;
		znam = z;
		socr();
	}
	racion(const racion& r) {
		chis = r.chis;
		znam = r.znam;
	}
	racion operator+ (const racion& r) {
		int new_chis = chis * r.znam + r.chis * znam;
		int new_znam = znam * r.znam;
		return racion(new_chis, new_znam);
	}
	racion operator- (const racion& r) {
		int new_chis = chis * r.znam - r.chis * znam;
		int new_znam = znam * r.znam;
		return racion(new_chis, new_znam);
	}
	racion operator*(const racion& r) {
		int new_chis = chis * r.chis;
		int new_znam = znam * r.znam;
		return racion(new_chis, new_znam);
	}
	racion operator/(const racion& r) {
		int new_chis = chis * r.znam;
		int new_znam = znam * r.chis;
		return racion(new_chis, new_znam);
	}
	void print() {
		if (znam == 1) {
			cout << chis << endl;
		}
		else {
			cout << chis << "/" << znam << endl;
		}
	}

};
int main() {
	setlocale(LC_ALL, "Ru_ru.UTF8");
	racion a;
	racion c(2);
	racion d(6, 7);
	racion e(3, 6);
	cout << "a = "; a.print(); cout << endl;
	cout << "c = "; c.print(); cout << endl;
	cout << "d = "; d.print(); cout << endl;
	cout << "e = "; e.print(); cout << endl;
	racion oper_sum = d + e;
	cout << "d + e = "; oper_sum.print(); cout << endl;
	racion oper_minus = d - e;
	cout << "d - e = "; oper_minus.print(); cout << endl;
	racion oper_pr = d * e;
	cout << "d*e = "; oper_pr.print(); cout << endl;
	racion oper_del = d / e;
	cout << "d/e = "; oper_del.print(); cout << endl;
	return 0;
}