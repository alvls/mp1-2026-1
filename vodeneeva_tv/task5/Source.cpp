#include <iostream>
#include <vector>
#include <string>
#include <map>

using namespace std;

struct Bilet {
    string data;
    int nomerPoezda;
    int nomerVagona;
    int nomerMesta;
    string tipMesta;     
    string fio;
    string stanciaOtpravlenia;
    string stanciaPribytia;
};

class Vagon {
public:
    int nomer;
    string tipVagona;     
   
    map<string, vector<bool>> mesta;
    
    Vagon(int num, string tip) {
        nomer = num;
        tipVagona = tip;
        
        if (tip == "sid") {
            mesta["sid"] = vector<bool>(100, false);  
        }
        else if (tip == "platzkart") {
            mesta["verh"] = vector<bool>(27, false);  
            mesta["niz"] = vector<bool>(27, false);  
        }
        else if (tip == "kupe") {
            mesta["verh"] = vector<bool>(18, false);  
            mesta["niz"] = vector<bool>(18, false);   
        }
        else if (tip == "sv") {
            mesta["niz"] = vector<bool>(18, false);  
        }
    }

    bool mestaSvobodno(string tipMesta, int nomerMesta) {
        if (mesta.count(tipMesta) == 0) return false;
        if (nomerMesta < 1 || nomerMesta > mesta[tipMesta].size()) return false;
        return !mesta[tipMesta][nomerMesta - 1];
    }

    bool zanyatMesto(string tipMesta, int nomerMesta) {
        if (mestaSvobodno(tipMesta, nomerMesta)) {
            mesta[tipMesta][nomerMesta - 1] = true;
            return true;
        }
        return false;
    }

    void osvoboditMesto(string tipMesta, int nomerMesta) {
        if (mesta.count(tipMesta) && nomerMesta >= 1 && nomerMesta <= mesta[tipMesta].size()) {
            mesta[tipMesta][nomerMesta - 1] = false;
        }
    }
};

class Poezd {
public:
    int nomer;
    string nazvanie;
    vector<Vagon> vagony;
   
    map<string, map<string, int>> ceny;
    
    Poezd(int num, string nazv) {
        nomer = num;
        nazvanie = nazv;
        
        if (nazvanie == "Lastochka") {
            for (int i = 1; i <= 8; i++) {
                vagony.push_back(Vagon(i, "sid"));
            }
            ceny["sid"]["sid"] = 800;
        }
        else if (nazvanie == "Firmenniy") {
            for (int i = 1; i <= 2; i++) {
                vagony.push_back(Vagon(i, "sv"));
            }
    
            for (int i = 3; i <= 8; i++) {
                vagony.push_back(Vagon(i, "kupe"));
            }
        
            for (int i = 9; i <= 12; i++) {
                vagony.push_back(Vagon(i, "platzkart"));
            }
            
            ceny["platzkart"]["verh"] = 1000;
            ceny["platzkart"]["niz"] = 1200;
            ceny["kupe"]["verh"] = 1600;
            ceny["kupe"]["niz"] = 1800;
            ceny["sv"]["niz"] = 3000;
        }
        else if (nazvanie == "Skoriy") {
            
            for (int i = 1; i <= 4; i++) {
                vagony.push_back(Vagon(i, "kupe"));
            }
        
            for (int i = 5; i <= 12; i++) {
                vagony.push_back(Vagon(i, "platzkart"));
            }
            
        
            ceny["platzkart"]["verh"] = 800;
            ceny["platzkart"]["niz"] = 950;
            ceny["kupe"]["verh"] = 1300;
            ceny["kupe"]["niz"] = 1450;
        }
    }
    
    vector<Vagon*> naytiVagonyPoTipu(string tipVagona) {
        vector<Vagon*> result;
        for (int i = 0; i < vagony.size(); i++) {
            if (vagony[i].tipVagona == tipVagona) {
                result.push_back(&vagony[i]);
            }
        }
        return result;
    }
    
    int cenaBileta(string tipVagona, string tipMesta) {
        return ceny[tipVagona][tipMesta];
    }
};

class GorkovskayaDoroga {
public:
    vector<Poezd> vsePoezda;
    vector<Bilet> vseBilety;
    
    GorkovskayaDoroga() {

        int nomeraPoezdov[5] = {701, 702, 703, 704, 705};
        string nazvania[5] = {"Lastochka", "Lastochka", "Lastochka", "Firmenniy", "Skoriy"};
        
        for (int den = 1; den <= 30; den++) {
            for (int i = 0; i < 5; i++) {
                vsePoezda.push_back(Poezd(nomeraPoezdov[i], nazvania[i]));
            }
        }
    }
   
    Poezd* naytiPoezd(int nomer, string data) {
   
        for (int i = 0; i < vsePoezda.size(); i++) {
            if (vsePoezda[i].nomer == nomer) {
                return &vsePoezda[i];
            }
        }
        return nullptr;
    }
};

class Kassa {
private:
    GorkovskayaDoroga& doroga;
    vector<Bilet> tekushiyZakaz;
    
public:
    Kassa(GorkovskayaDoroga& d) : doroga(d) {}
  
    bool prinyatDannye(string data, int nomerPoezda, string tipVagona,
                       map<string, int> trebuetsyaMest, vector<string> fios) {
 
        tekushiyZakaz.clear();
   
        int vsegoMest = 0;
        for (auto& item : trebuetsyaMest) {
            vsegoMest += item.second;
        }
        if (vsegoMest != fios.size()) {
            return false;
        }
 
        Poezd* poezd = doroga.naytiPoezd(nomerPoezda, data);
        if (poezd == NULL) {
            return false;
        }
        
        vector<Vagon*> podhodyashieVagony = poezd->naytiVagonyPoTipu(tipVagona);
        if (podhodyashieVagony.empty()) {
            return false;
        }
       
        Vagon* vybranniyVagon = NULL;
        vector<pair<string, int>> vybrannyeMesta;  
        
        for (int v = 0; v < podhodyashieVagony.size(); v++) {
            Vagon* vagon = podhodyashieVagony[v];
            vybrannyeMesta.clear();
            bool vsePomestilis = true;
         
            for (auto& treb : trebuetsyaMest) {
                string tipMesta = treb.first;
                int nuzhno = treb.second;
                int naydeno = 0;
                
                for (int m = 1; m <= vagon->mesta[tipMesta].size(); m++) {
                    if (vagon->mestaSvobodno(tipMesta, m)) {
                        vybrannyeMesta.push_back({tipMesta, m});
                        naydeno++;
                        if (naydeno == nuzhno) break;
                    }
                }
                
                if (naydeno < nuzhno) {
                    vsePomestilis = false;
                    break;
                }
            }
            
            if (vsePomestilis) {
                vybranniyVagon = vagon;
                break;
            }
        }
        
        if (vybranniyVagon == NULL) {
            return false;
        }
        
        int indexFIO = 0;
        for (int i = 0; i < vybrannyeMesta.size(); i++) {
            string tipMesta = vybrannyeMesta[i].first;
            int nomerMesta = vybrannyeMesta[i].second;
        
            vybranniyVagon->zanyatMesto(tipMesta, nomerMesta);
       
            Bilet b;
            b.data = data;
            b.nomerPoezda = nomerPoezda;
            b.nomerVagona = vybranniyVagon->nomer;
            b.nomerMesta = nomerMesta;
            b.tipMesta = tipMesta;
            b.fio = fios[indexFIO];
            b.stanciaOtpravlenia = "Nizniy Novgorod";
            b.stanciaPribytia = "Moskva";
            
            tekushiyZakaz.push_back(b);
            indexFIO++;
        }
        
        return true;
    }
  
    int raschitatStoimost() {
        if (tekushiyZakaz.empty()) {
            return 0;
        }
        
        int summa = 0;
        Poezd* poezd = doroga.naytiPoezd(tekushiyZakaz[0].nomerPoezda, tekushiyZakaz[0].data);
        
        if (poezd != NULL) {
            for (int i = 0; i < tekushiyZakaz.size(); i++) {
    
                string tipVagona;
                for (int v = 0; v < poezd->vagony.size(); v++) {
                    if (poezd->vagony[v].nomer == tekushiyZakaz[i].nomerVagona) {
                        tipVagona = poezd->vagony[v].tipVagona;
                        break;
                    }
                }
                summa += poezd->cenaBileta(tipVagona, tekushiyZakaz[i].tipMesta);
            }
        }
        return summa;
    }

    void sformirovatBilety() {
        if (tekushiyZakaz.empty()) {
            return;
        }
      
        for (int i = 0; i < tekushiyZakaz.size(); i++) {
            doroga.vseBilety.push_back(tekushiyZakaz[i]);
            
            cout << "\n--- Bilet " << i+1 << " ---" << endl;
            cout << "Data: " << tekushiyZakaz[i].data << endl;
            cout << "Poezd №" << tekushiyZakaz[i].nomerPoezda << endl;
            cout << "Vagon №" << tekushiyZakaz[i].nomerVagona << endl;
            cout << "Mesto №" << tekushiyZakaz[i].nomerMesta << " (" << tekushiyZakaz[i].tipMesta << ")" << endl;
            cout << "Passazhir: " << tekushiyZakaz[i].fio << endl;
            cout << "Otpravlenie: " << tekushiyZakaz[i].stanciaOtpravlenia << endl;
            cout << "Pribytie: " << tekushiyZakaz[i].stanciaPribytia << endl;
        }
        
        tekushiyZakaz.clear();
    }
   
    void otmenitZakaz() {
        if (tekushiyZakaz.empty()) {
            return;
        }
    
        Poezd* poezd = doroga.naytiPoezd(tekushiyZakaz[0].nomerPoezda, tekushiyZakaz[0].data);
        if (poezd != NULL) {
            for (int v = 0; v < poezd->vagony.size(); v++) {
                for (int b = 0; b < tekushiyZakaz.size(); b++) {
                    if (poezd->vagony[v].nomer == tekushiyZakaz[b].nomerVagona) {
                        poezd->vagony[v].osvoboditMesto(tekushiyZakaz[b].tipMesta, 
                                                         tekushiyZakaz[b].nomerMesta);
                    }
                }
            }
        }
        
        tekushiyZakaz.clear();
    }
    

    void proveritNalichie(string data, int nomerPoezda, string tipVagona, 
                          map<string, int> trebuetsyaMest) {
        
        Poezd* poezd = doroga.naytiPoezd(nomerPoezda, data);
        if (poezd == NULL) {
            return;
        }
        
        vector<Vagon*> vagony = poezd->naytiVagonyPoTipu(tipVagona);
        if (vagony.empty()) {
            return;
        }
        
        bool naydeno = false;
        for (int v = 0; v < vagony.size(); v++) {
            bool vmeshaetsa = true;
            for (auto& treb : trebuetsyaMest) {
                int svobodno = 0;
                for (int m = 1; m <= vagony[v]->mesta[treb.first].size(); m++) {
                    if (vagony[v]->mestaSvobodno(treb.first, m)) svobodno++;
                }
                if (svobodno < treb.second) {
                    vmeshaetsa = false;
                    break;
                }
            }
        }
        
    }
};

int main() {
    GorkovskayaDoroga doroga;
    Kassa kassa(doroga);
    
    cout << "Покупка билетов на фирменный поезд: 2 нижних, 1 верхнее в купе" << endl;
    map<string, int> a;
    a["niz"] = 2;
    a["verh"] = 1;
    vector<string> fios2 = {"Sidorov S.S.", "Kuznetsov K.K.", "Mihaylov M.M."};
    
    if (kassa.prinyatDannye("25.04.2026", 704, "kupe", a, fios2)) {
        kassa.raschitatStoimost();
        kassa.sformirovatBilety();
    }
    
    cout << "\n";
     
    cout << "Проверка наличия мест" << endl;
    map<string, int> b;
    b["niz"] = 3;
    b["verh"] = 2;
    kassa.proveritNalichie("15.04.2026", 704, "kupe", b);
    
    return 0;
}