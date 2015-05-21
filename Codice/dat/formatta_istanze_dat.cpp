using namespace std;

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <time.h>


int main(int argc, char* argv[])
{
    bool DEBUG = false;
	int index, p1, p2, n, bin_capacity_a, bin_capacity_b,compare_function, strategia;
    string algoritmo,temp1, temp2, strategia_scambio;

    ifstream infile;

    infile.open(argv[1]);

    infile >> n;
    int a [n];
    int b [n];
    infile >> bin_capacity_a >> bin_capacity_b;

	//inserisce nella matrice istanza i dati presi in input
    int i=0;
    while(infile >> index >> p1 >> p2)
    {
        a[i] = p1;
        b[i] = p2;
        i++;
    }
	
    ofstream output_file;
    char* path = argv[1];
    string pathString(path);
    string name = pathString.substr(14,12);
    string dat = "";
    if(name.at(name.size()-1) == '.')
		dat = "dats/"+name+"dat";
	else
		dat = "dats/"+name+".dat";
    output_file.open((char*)dat.c_str(), ofstream::out | ofstream::app);
    output_file << "N = " << n << ";\n";
    output_file << "C1 = " << bin_capacity_a << ";\n";
    output_file << "C2 = " << bin_capacity_b << ";\n";
    output_file << "p1 = [";
    for(int i = 0; i<n; i++)
        output_file << a[i] << " ";
    output_file << "];\n";
    output_file << "p2 = [";
    for(int i = 0; i<n; i++)
        output_file << b[i] << " ";
    output_file << "];\n";
    output_file.close();

	
	
    return 0;
}
