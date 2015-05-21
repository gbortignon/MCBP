using namespace std;

#include <string>
#include <fstream>
#include <iostream>
#include "GetInstance.hpp"

int** read(char* path)
{
    int index, p1, p2, n, bin_capacity_a, bin_capacity_b;

    ifstream infile;

    infile.open(path);

    infile >> n;
    infile >> bin_capacity_a >> bin_capacity_b;
    int istanza [n][2];

    while(infile >> index >> p1 >> p2)
    {
        istanza[index][0] = p1;
        istanza[index][1] = p2;
        printf("%d\t%d\t%d\n", index, istanza[index][0], istanza[index][1]);
        //std::cout << index << "\t" << istanza[index][0] << "\t" << istanza[index][1] << endl;
    }
}
