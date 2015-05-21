using namespace std;

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <time.h>

#include "Greedy.hpp"
#include "Multi_greedy-restart.hpp"

/*
 * Parametri:
 * [1] path dell'istanza selezionata
 * [2] strategia scambio {1,2,3}
 * [3] numero restart
 * [4] greedy compare function {1,2,3,4}
 * [5] greedy structured {1,2,3}
 * 
 */

void solMulti(int[][3], int, int, int, string, string, string, string, bool);

int main(int argc, char* argv[])
{
    bool DEBUG = false;
    int index, p1, p2, n, bin_capacity_a, bin_capacity_b;
    string algoritmo, strategia_scambio, restarts, greedy_compare_function, greedy_structured;;
    int tempClock;

    // inizializzazione parametri esecuzione
    /* strategia di scambio del multistart:
     * 1 - Tenta di riempire al massimo i bin già pieni
     * 2 - Tenta di svuotare al massimo il bin più vuoto basandosi sulla somma delle due dimensioni
     * 3 - Tenta di svuotare al massimo il bin più vuoto basandosi sulla media delle due dimensioni
     */
    strategia_scambio = argv[2];
    /* 
     * numero di restart
    */
    restarts = argv[3];
    /*
     * Strategia di scambio del greedy:
     * 1 - StructuredMaxFill
     * 2 - StructuredMaxFillMean
     * 3 - StructuredMaxFillSub
     * 4 - StructuredWeightedMean
     */
    greedy_compare_function = argv[4];
    /*
     * Imposta la modalità di esecuzione del greedy
     * 1 - Greedy Classic
     * 2 - Greedy Structured
     * 3 - Greedy StructuredPro
     */
    greedy_structured = argv[5];

    ifstream infile;
    infile.open(argv[1]);

    infile >> n;
    infile >> bin_capacity_a >> bin_capacity_b;
    int istanza[n][3];
    int* sol;

    //inserisce nella matrice istanza i dati presi in input
    int i=0;
    while(infile >> index >> p1 >> p2)
    {
        istanza[i][0] = index;
        istanza[i][1] = p1;
        istanza[i][2] = p2;
        i++;
    }
    
    tempClock = clock();
    
    ofstream output_file;
    output_file.open("Times.txt", ofstream::out | ofstream::app);
    
    solMulti(istanza, n, bin_capacity_a, bin_capacity_b, strategia_scambio, restarts, greedy_compare_function, greedy_structured, false);

    output_file << (float)(clock()-tempClock)/CLOCKS_PER_SEC << ";\n";
    output_file.close();
    

    return 0;
}
