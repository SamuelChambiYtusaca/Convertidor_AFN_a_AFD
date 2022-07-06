#include <bits/stdc++.h>
#include "convertidor.hpp"
using namespace std;
int main(){
    Convertidor convertidor("in.txt");
    convertidor.printAFN();
    convertidor.showGrafico(0);
    convertidor.convertir();
    convertidor.printAFD();
    convertidor.generarSalida();
    convertidor.showGrafico(1);
}