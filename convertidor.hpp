#pragma once
#include <bits/stdc++.h>
using namespace std;
typedef pair<int, char> state;
typedef pair<char, char> subConj;

class Convertidor
{

public:
    Convertidor(string);
    vector<int> transiciones(vector<int>, char);
    void transiciones(vector<int>, vector<int> &, char);
    vector<int> e_clausura(vector<int>);
    void e_clausura(int, vector<int> &, vector<int> &);
    bool find(const vector<int> &, map<char, vector<int>>::iterator &sC);
    void convertir();
    void printAFN();
    void printAFD();
    void generarSalida();
    void showGrafico(bool);

    ~Convertidor();

private:
    int nStates;
    int s_inicial = 0;
    int n_Trans = 0;
    vector<int> s_Finales;
    vector<vector<state>> AFN;
    map<char, vector<int>> states_AFD;
    map<char, vector<subConj>> AFD;
    string alfabeto;
    string accept_AFD;
    queue<vector<int>> marcados;
};

Convertidor::Convertidor(string nArch)
{
    ifstream file;

    file.open(nArch);
    string line = "";
    while (!file.eof())
    {
        if (line == "Estados")
        {
            int n;
            file >> n;
            nStates = n;
            AFN = vector<vector<state>>(nStates);
        }
        else if (line == "Estados_de_Aceptacion")
        {
            int n;
            file >> n;
            for (int i = 0; i < n; ++i)
            {
                int x;
                file >> x;
                s_Finales.push_back(x);
            }
        }
        else if (line == "Entradas")
        {
            int n;
            file >> n;
            for (int i = 0; i < n; i++)
            {
                int x;
                file >> x;
                char x1 = x + '0';
                alfabeto.push_back(x1);
            }
        }
        else if (line == "Transiciones_(x,y,z)_epsilon=-1")
        {
            int n;
            file >> n;
            for (int i = 0; i < n; i++)
            {
                int ini, fin;
                int trans;
                file >> ini >> trans >> fin;
                char t;
                if (trans == -1)
                    t = 'e';
                else
                    t = trans + '0';
                AFN[ini].push_back({fin, t});
            }
        }
        getline(file, line);
    }
    file.close();
}

bool Convertidor::find(const vector<int> &sB, map<char, vector<int>>::iterator &sC)
{
    for (; sC != states_AFD.end(); sC++)
    {
        if (sB == sC->second)
            return true;
    }
    return false;
}

void Convertidor::printAFN()
{
    for (int i = 0; i < nStates; ++i)
    {
        cout << i << "->";
        for (state j : AFN[i])
        {
            cout << "{" << j.first << "," << j.second << "} ";
        }
        cout << endl;
    }
}

void Convertidor::convertir()
{
    vector<int> T = e_clausura({s_inicial});
    char key = 'A';
    char ini = key;
    states_AFD[key] = T;
    AFD[ini] = {};
    key++;
    marcados.push(T);
    while (!marcados.empty())
    {
        T = marcados.front();
        for (char a : alfabeto)
        {
            vector<int> T_a = transiciones(T, a);
            vector<int> state = e_clausura(T_a);
            auto it = states_AFD.begin();
            if (find(state, it))
                AFD[ini].push_back({it->first, a});
            else
            {
                states_AFD[key] = state;
                AFD[ini].push_back({key, a});
                AFD[key] = {};
                marcados.push(state);
                key++;
            }
        }
        marcados.pop();
        auto it2 = states_AFD.begin();
        find(marcados.front(), it2);
        ini = it2->first;
    }
    for (auto par : states_AFD)
    {
        for (int i : s_Finales)
            if (count(par.second.begin(), par.second.end(), i))
                accept_AFD.push_back(par.first);
    }

    for (auto par : AFD)
    {
        n_Trans += par.second.size();
    }
}

vector<int> Convertidor::transiciones(vector<int> T, char a)
{
    vector<int> T_a;
    transiciones(T, T_a, a);
    sort(T_a.begin(), T_a.end());
    return T_a;
}

void Convertidor::transiciones(vector<int> T, vector<int> &T_a, char a)
{
    for (int i : T)
    {
        auto estados = AFN[i];
        for (state s : estados)
        {
            if (s.second == a)
                T_a.push_back(s.first);
        }
    }
}

vector<int> Convertidor::e_clausura(vector<int> estados)
{
    vector<int> subConjunto;
    vector<int> used(nStates + 1, 0);
    for (int i : estados)
    {
        subConjunto.push_back(i);
        e_clausura(i, subConjunto, used);
    }
    sort(subConjunto.begin(), subConjunto.end());
    return subConjunto;
}

void Convertidor::e_clausura(int estado, vector<int> &sConjunto, vector<int> &used)
{
    auto trans = AFN[estado];
    if (used[estado])
        return;
    used[estado] = 1;
    for (state i : trans)
    {
        if (i.second == 'e')
        {
            sConjunto.push_back(i.first);
            e_clausura(i.first, sConjunto, used);
        }
    }
    return;
}

void Convertidor::printAFD()
{
    for (auto par : AFD)
    {
        cout << par.first << ": ";
        for (auto a : states_AFD[par.first])
        {
            cout << a << " ";
        }
        cout << endl;
        cout << par.first << "->";
        for (subConj elemento : par.second)
        {
            cout << "{" << elemento.first << ";" << elemento.second << "} ";
        }
        cout << endl;
    }
    cout << "Estados Finales\n";
    for (auto a : accept_AFD)
    {
        cout << a << " ";
    }
    cout << endl;
    cout << "Numero de Transiciones: " << n_Trans << endl;
}

void Convertidor::generarSalida()
{
    ofstream file;
    file.open("out.txt");
    vector<string> operaciones = {"Estados", "Estados de Aceptacion", "Entradas", "Transiciones de Estados (x,y,z)"};
    for (string aux : operaciones)
    {
        file << aux << endl;
        if (aux == "Estados")
        {
            file << states_AFD.size() << endl;
            for (auto par : states_AFD)
            {
                file << par.first << "= {";
                for (int i : par.second)
                {
                    file << i << " ";
                }
                file << "}\n";
            }
        }
        else if (aux == "Estados de Aceptacion")
        {
            file << accept_AFD.size() << endl;
            for (auto a : accept_AFD)
            {
                file << a << " ";
            }
            file << endl;
        }
        else if (aux == "Entradas")
        {
            file << alfabeto.size() << endl;
            for (char a : alfabeto)
            {
                file << a << " ";
            }
            file << endl;
        }
        else if (aux == "Transiciones de Estados (x,y,z)")
        {
            file << n_Trans << endl;
            for (auto par : AFD)
            {
                for (subConj transicion : par.second)
                {
                    file << par.first << " " << transicion.second << " " << transicion.first << endl;
                }
            }
        }
    }
    file.close();
}

void Convertidor::showGrafico(bool op)
{
    
    ofstream file;
    vector<string> opcion = {"in.dot","out.dot"};
    file.open(opcion[op]);
    file << "digraph finite_state_machine {\n";
    file << "fontname=\"Helvetica,Arial,sans-serif\"\n";
    file << "node [fontname=\"Helvetica,Arial,sans-serif\"]\n";
    file << "edge [fontname=\"Helvetica,Arial,sans-serif\"]\n";
    file << "rankdir=LR;\n";
    file << "node [shape = doublecircle];";
    if(op){
        if (AFD.empty())
            return;
        for (char i : accept_AFD)
            file << i << " ";
        file << ";\n";
        file << "node [shape = circle];\n";
        for (auto par : AFD)
        {
            for (subConj aux : par.second)
            {
                file << par.first << " -> " << aux.first << "[label = \"" << aux.second << "\"];\n";
            }
        }
    }
    else{
        for (int i : s_Finales)
            file << i << " ";
        file << ";\n";
        file << "node [shape = circle];\n";
        for (int i = 0; i < nStates; i++)
        {
            for (state aux : AFN[i])
            {
                file << i << " -> " << aux.first << "[label = \"" << aux.second << "\"];\n";
            }
        }
    }
    file << "}";
    file.close();
    if(op){
        system("dot out.dot -o out.png -Tpng");
        system("out.png");
    }else{
        system("dot in.dot -o in.png -Tpng");
        system("in.png");
    }
}

Convertidor::~Convertidor()
{
}
