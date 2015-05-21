using namespace std;

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <time.h>

#include "Greedy.hpp"

/*
 * Parametri:
 * [1] nome istanza selezionata
 * [2] algoritmo scelto
 * [3] Compare Functions:
 *      1 = max
 *      2 = min
 *      3 = alterna 1 e 2
 * [4] Structered/StructuredPro (2/3)
 * */

int main(int argc, char* argv[])
{
    bool DEBUG = false;
	int index, p1, p2, n, bin_capacity_a, bin_capacity_b,compare_function, strategia;
    string algoritmo,temp1, temp2, strategia_scambio;

    ifstream infile;

    infile.open(argv[1]);
    algoritmo = argv[2];

    if(algoritmo.compare("greedy") == 0)
    {
        temp1 = argv[3];
        compare_function = atoi(temp1.c_str());
        temp2 = argv[4];
        strategia = atoi(temp2.c_str());
    }

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
        //printf("%d\t%d\t%d\n", istanza[i][0], istanza[i][1], istanza[i][2]);
        i++;
    }
    /*
     * [6] Structured:
     * 0 = classico
     * 1 = strutturato
     */
    int tempClock;
	ofstream output_file;
	output_file.open("Times_Greedy.txt", ofstream::out | ofstream::app);

    if(algoritmo.compare("greedy") == 0){
        tempClock = clock();
        sol = solGreedy(istanza, n, bin_capacity_a, bin_capacity_b,compare_function,strategia, DEBUG);
        output_file << ((float)clock()-tempClock)/CLOCKS_PER_SEC << ";\n";
	}
	output_file.close();

    if(DEBUG)
    {
        for(int i = 0; i<n; i++)
        cout << "Oggetto: " << i+1 << "-> Bin: " << sol[i] <<"\n";
    }

    return 0;
}
