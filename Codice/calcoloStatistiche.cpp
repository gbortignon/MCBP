using namespace std;

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <time.h>
#include <dirent.h>
#include <string.h>


int main(int argc, char* argv[])
{
	DIR *dir = opendir("../istanzeTest");
	struct dirent *dp;

	//rimozione vecchi benchmark
	remove("StatisticheAll.txt");

	//inizializzazione files
	ofstream start_output_greedy;

	start_output_greedy.open("StatisticheAll.txt", ofstream::out | ofstream::app);

	start_output_greedy << ";Varianza A;Varianza B;Scarto;Media A;Media B;\n";
	start_output_greedy.close();


    while ((dp = readdir(dir)) != NULL)
    {
		int index, p1, p2, n, bin_capacity_a, bin_capacity_b,compare_function, strategia;

		ifstream infile;
		string file_name = dp->d_name;

		if(file_name.compare(".") != 0 && file_name.compare("..") != 0)
		{
			string prefix = "../istanzeTest/";
			string buf(prefix.c_str());
			buf.append(file_name.c_str());
			const char* path = buf.c_str();

			infile.open(path);
			infile >> n;
			infile >> bin_capacity_a >> bin_capacity_b;
			int istanza[n][3];

			//inserisce nella matrice istanza i dati presi in input
			int i=0;
			while(infile >> index >> p1 >> p2)
			{
				istanza[i][0] = index;
				istanza[i][1] = p1;
				istanza[i][2] = p2;
				i++;
			}


			//GREEDY
			ofstream output_greedy;
			output_greedy.open("StatisticheAll.txt", ofstream::out | ofstream::app);

			output_greedy << "Istanza " << dp->d_name << ": ;";

			cout << "Istanza: " << dp->d_name <<"\n";

            float media_a,media_b,scarto,var_a,var_b;
            float sum_a = 0,sum_b = 0,sum_scarti = 0;
			for(int i=0;i<n;i++){
               sum_a += istanza[i][1];
               sum_b += istanza[i][2];
               sum_scarti += abs(istanza[i][1]-istanza[i][2]);
			}
			media_a = (float)(sum_a/n)*100/bin_capacity_a;
			media_b = (float)(sum_b/n)*100/bin_capacity_b;
			scarto = (float)(sum_scarti/n)*100/bin_capacity_a;

			double temp_a = 0,temp_b = 0;
            for(int i = 0; i < n; i++)
            {
                 temp_a += (istanza[i][1] - ((float)sum_a/n)) * (float)(istanza[i][1] - ((float)sum_a/n)) ;
                 temp_b += (istanza[i][2] - ((float)sum_b/n)) * (float)(istanza[i][2] - ((float)sum_b/n)) ;
            }
            var_a = ((float)temp_a/n)/bin_capacity_a;
            var_b = ((float)temp_b/n)/bin_capacity_b;

            output_greedy << var_a << ";" << var_b << ";" << scarto << ";" << media_a << ";" << media_b << ";";

			output_greedy << "\n";

			output_greedy.close();
		}
    }
    (void)closedir(dir);
    return 0;
}
