using namespace std;

#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "Multi_na.hpp"

/*
 * Parametri:
 * [1] path dell'istanza selezionata
 * [2] strategia scambio {1,2,3}
 * [3] numero restart
 * 
 */

int main(int argc, char* argv[])
{
	int index=0, p1, p2, n, bin_capacity_a, bin_capacity_b;
    string algoritmo;
    string strategia_scambio;
    string restarts;

    ifstream infile;

    infile.open(argv[1]);
    string istanza_name=argv[1];
    /* strategia di scambio del multistart:
     * 1 - Tenta di riempire al massimo i bin già pieni
     * 2 - Tenta di svuotare al massimo il bin più vuoto basandosi sulla somma delle due dimensioni
     * 3 - Tenta di svuotare al massimo il bin più vuoto basandosi sulla media delle due dimensioni
     */
    strategia_scambio = argv[2];
    restarts = argv[3];

    infile >> n;
    infile >> bin_capacity_a >> bin_capacity_b;
    int istanza[n][3];
    int** sol;

	//inserisce nella matrice istanza i dati presi in input
    int i=0;
    while(infile >> index >> p1 >> p2)
    {
        istanza[i][0] = index;
        istanza[i][1] = p1;
        istanza[i][2] = p2;
        i++;
    }
    int *multisol;

	int tempClock;
	ofstream output_file;
	output_file.open("Times_Multi_na.txt", ofstream::out | ofstream::app);

	tempClock = clock();	
	cout<<argv[1]<<endl;
	solMulti(istanza, n, bin_capacity_a, bin_capacity_b, "2", strategia_scambio, restarts, false);
	output_file << ((float)clock()-tempClock)/CLOCKS_PER_SEC << ";\n";

	output_file.close();
}
