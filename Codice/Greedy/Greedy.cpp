using namespace std;

#include <fstream>
#include <cstdlib>
#include <climits>
#include <iostream>
#include <string>
#include <string.h>
#include <limits>
#include <math.h>
#include <typeinfo>
#include <cmath>        // std::abs

//Si calcola il numero minimo di bin utilizzabili considerando gli elementi come continui dividendo per il loro numero
int * getStructuredSolution(int[][3], int, int, int,int, int*, bool);
//Si calcola il numero minimo di bin utilizzabili considerando gli elementi come continui dividendo per il loro numero ricalcolando tale valore ogni volta che si inserisce un elemento
int * getStructuredProSolution(int[][3], int, int, int,int, int*, bool);

/***** Strategie per la soluzione classica *****/
int getMaxFill(int[][3], int, int, int, int*,bool);

/***** Strategie per la soluzione strutturata *****/
int * getStructuredMaxFill(int[][3], int, int[][2], int, int, int, int*, bool, int*);
int * getStructuredMaxFillMean(int[][3], int, int[][2], int, int, int, int*, bool, int*);
// Pesa la somma delle due dimensioni secondo quella più grande, in modo da valorizzare oggetti con una dimensione molto grande
int * getStructuredWeightedMean(int[][3], int, int[][2], int, int, int, int*, bool, int*);
//Si cerca l'oggetto con la maggiore differenza delle dimensioni
int * getStructuredMaxFillSub(int[][3], int, int[][2], int, int, int, int*, bool, int*);

void checkSolutionValues(int[][3], int*, int, int, int);

//Interfaccia esterna per determinare se utilizzare soluzione classica o strutturata
int* solGreedy(int istanza[][3], int n, int bin_capacity_a, int bin_capacity_b, int compare_function,int structured, bool DEBUG)
{
    int * bin_assignment = new int[n];
    switch(structured)
    {
    //Structured
    case(2):
        bin_assignment = getStructuredSolution(istanza,n,bin_capacity_a,bin_capacity_b,compare_function, bin_assignment, DEBUG);
        break;
    //StructuredPro
    case(3):
        bin_assignment = getStructuredProSolution(istanza,n,bin_capacity_a,bin_capacity_b,compare_function, bin_assignment, DEBUG);
        break;
    }
    checkSolutionValues(istanza,bin_assignment,n,bin_capacity_a,bin_capacity_b);
    return bin_assignment;
}

// Controllo eventuali difetti per ammissibilità, coerenza assegnamento e relativi valori di spazio vuoto
void checkSolutionValues(int istanza[][3], int* soluzione, int n, int bin_capacity_a, int bin_capacity_b){
	int sum_a;
	int sum_b;
	int max = INT_MIN;
	for(int j=0; j<n; j++){
		if(soluzione[j]>max){
			max=soluzione[j];
		}
	}
	for(int bin=0; bin<max; bin++)
	{
		sum_a = 0;
		sum_b = 0;
		for(int i=0; i<n; i++)
		{
			if(soluzione[i]==bin)
			{
				sum_a += istanza[i][1];
				sum_b += istanza[i][2];
			}
		}
		if(sum_a > bin_capacity_a || sum_b > bin_capacity_b){
			cout << "CAPACITA' SFORATA!!" << endl;
			exit(1);
		}
	}
}


//Si calcola il numero minimo di bin utilizzabili considerando gli elementi come continui dividendo per
//il loro numero
int * getStructuredSolution(int istanza[][3], int n, int bin_capacity_a, int bin_capacity_b,int compare_function, int * bin_assigment, bool DEBUG)
{
    bool DEBUG_PRO = false;
    bool found =false;

    //Inizializzo l'assegnamento a -1, in modo da sapere gli indici non ancora usati
    for(int i=0;i<n;i++)
        bin_assigment[i] = -1;

    //Stima del numero dei bin considerandoli continui
    int sum_a = 0;
    int sum_b = 0;
    for(int i=0;i<n;i++)
    {
        sum_a += istanza[i][1];
        sum_b += istanza[i][2];
    }
    int max, num_bins;
    if((double)sum_a/bin_capacity_a > (double)sum_b/bin_capacity_b)
    {
        max = sum_a;
        num_bins = (int)ceil((double)sum_a/bin_capacity_a); //ceil: arrotonda all'intero superiore
    }
    else
    {
        max = sum_b;
        num_bins = (int)ceil((double)sum_b/bin_capacity_b);
    }

    if(DEBUG) cout << "NUMERO BIN CONTINUO: " << num_bins << "\n";

    int numEsclusi = 0;
    int binIniziali = num_bins;

    //Inizializzo le capacità dei bin alle capacità iniziali
    int bin_capacities[n][2];
    for(int j=0;j<num_bins;j++)
    {
        bin_capacities[j][0] = bin_capacity_a;
        bin_capacities[j][1] = bin_capacity_b;
    }

    //Ciclo di assegnamento degli oggetti ai bin
    for(int i=0; i<n;i++)
    {
        //Index[0] contiene l'indice del bin scelto per inserire l'oggetto
        //Index[1] contiene l'indice dell'oggetto scelto
        int * index = new int[2];

        switch(compare_function)
	    {
	    case(1):
	        //Riempie tutti i bin secondo una strategia maxFill
	        index = getStructuredMaxFill(istanza,n,bin_capacities, bin_capacity_a, bin_capacity_b,num_bins, bin_assigment, DEBUG, index);
	        break;
	    case(2):
	        index = getStructuredMaxFillMean(istanza,n,bin_capacities, bin_capacity_a, bin_capacity_b,num_bins, bin_assigment, DEBUG, index);
	        break;
        case(3):
	        index = getStructuredMaxFillSub(istanza,n,bin_capacities, bin_capacity_a, bin_capacity_b,num_bins, bin_assigment, DEBUG, index);
	        break;
        case(4):
	        index = getStructuredWeightedMean(istanza,n,bin_capacities, bin_capacity_a, bin_capacity_b,num_bins, bin_assigment, DEBUG, index);
	        break;
		}
		//Se non ci sono elementi che stanno nei bin aperti, si prende il più grosso che sarà il primo inserito nel nuovo bin
        if(index[1] == -1)
        {
            if(DEBUG)
                cout << "Rimpiti i primi " << num_bins << " bin!!!\n\n";
            if(numEsclusi == 0) numEsclusi=n-i+1;
            index[0] = num_bins; //Inizio a riempire un nuovo bin
            index[1] = getMaxFill(istanza,n,bin_capacity_a,bin_capacity_b, bin_assigment, DEBUG);

            if(DEBUG_PRO && !found)
            {
                for(int j=0;j<n;j++)
                {
                    if(bin_assigment[j] == -1 && j != num_bins)
                        cout << "Oggetto " << j << " -> " << istanza[j][1] << " , " << istanza[j][2] << endl;
                }
                found = true;
            }


            bin_capacities[num_bins][0] = bin_capacity_a;
            bin_capacities[num_bins][1] = bin_capacity_b;
            num_bins++;
        }

        int obj_index = index[1];
        int bin_index = index[0];

        //Aggiorno le capacità del bin selezionato
        bin_capacities[bin_index][0] -= istanza[obj_index][1];
        bin_capacities[bin_index][1] -= istanza[obj_index][2];

        //Assegno l'oggetto i-esimo al bin
        bin_assigment[obj_index] = bin_index;
    }

    if(DEBUG || DEBUG_PRO) cout << "\n\n\n\nNUMERO MINIMO BIN: " << binIniziali << "\n\n";
    if(DEBUG || DEBUG_PRO) cout << "NUMERO BIN APERTI: " << num_bins << "\n";
    ofstream output_file;
    output_file.open("Benchmark_Greedy.txt", ofstream::out | ofstream::app);
    if(compare_function == 4)
        output_file << num_bins << ";\n";
    else
        output_file << num_bins << ";";
    output_file.close();
    return bin_assigment;

}



//Si calcola il numero minimo di bin utilizzabili considerando gli elementi come continui dividendo per il loro numero
//Ricalcolando tale valore ogni volta che si inserisce un elemento
int * getStructuredProSolution(int istanza[][3], int n, int bin_capacity_a, int bin_capacity_b,int compare_function, int * bin_assigment, bool DEBUG)
{
    bool DEBUG_PRO = false;
    bool found =false;


    //Inizializzo l'assegnamento a -1, in modo da sapere gli indici non ancora usati
    for(int i=0;i<n;i++)
        bin_assigment[i] = -1;

    //Stima del numero dei bin considerandoli continui
    int sum_a = 0;
    int sum_b = 0;
    for(int i=0;i<n;i++)
    {
        sum_a += istanza[i][1];
        sum_b += istanza[i][2];
    }
    int max, num_bins;
    if((double)sum_a/bin_capacity_a > (double)sum_b/bin_capacity_b)
    {
        max = sum_a;
        num_bins = (int)ceil((double)sum_a/bin_capacity_a); //ceil: arrotonda all'intero superiore
    }
    else
    {
        max = sum_b;
        num_bins = (int)ceil((double)sum_b/bin_capacity_b);
    }
    if(DEBUG) cout << "NUMERO BIN CONTINUO: " << num_bins << "\n";

    int numEsclusi = 0;
    int binIniziali = num_bins;


    //Inizializzo capacità globali
    int global_capacity_a = bin_capacity_a*num_bins;
    int global_capacity_b = bin_capacity_b*num_bins;

    //Inizializzo le capacità dei bin alle capacità iniziali
    int bin_capacities[n][2];
    for(int j=0;j<num_bins;j++)
    {
        bin_capacities[j][0] = bin_capacity_a;
        bin_capacities[j][1] = bin_capacity_b;
    }

    //Ciclo di assegnamento degli oggetti ai bin
    for(int i=0; i<n;i++)
    {
      //Index[0] contiene l'indice del bin scelto per inserire l'oggetto
      //Index[1] contiene l'indice dell'oggetto scelto
      int * index = new int[2];

      switch(compare_function)
	    {
	    case(1):
	        //Riempe tutti i bin secondo una strategia maxFill
	        index = getStructuredMaxFill(istanza,n,bin_capacities, bin_capacity_a, bin_capacity_b,num_bins, bin_assigment, DEBUG, index);
	        break;
	    case(2):
	        index = getStructuredMaxFillMean(istanza,n,bin_capacities, bin_capacity_a, bin_capacity_b,num_bins, bin_assigment, DEBUG, index);
	        break;
        case(3):
	        index = getStructuredMaxFillSub(istanza,n,bin_capacities, bin_capacity_a, bin_capacity_b,num_bins, bin_assigment, DEBUG, index);
	        break;
        case(4):
	        index = getStructuredWeightedMean(istanza,n,bin_capacities, bin_capacity_a, bin_capacity_b,num_bins, bin_assigment, DEBUG, index);
	        break;
  		}

      int obj_index = index[1];
      int bin_index = index[0];

      //Decremento la capacità totale per ricalcolare il numero di bin
      global_capacity_a -= istanza[obj_index][1];
      global_capacity_b -= istanza[obj_index][2];

      int num_bins_new = 0,num_bins_new_b = 0,num_bins_new_a = 0;
      //INIZIO SOMMA
      //Stima del numero dei bin considerandoli continui
      int temp_sum_a = 0;
      int temp_sum_b = 0;
      for(int j=0;j<n;j++)
      {
        //Calcolo la somma della dimensione degli oggetti non ancora inseriti
        if(bin_assigment[j] == -1){
          temp_sum_a += istanza[j][1];
          temp_sum_b += istanza[j][2];
        }
      }

      if(temp_sum_a > global_capacity_a)
          num_bins_new_a = num_bins + (int)ceil((double)(temp_sum_a-global_capacity_a)/bin_capacity_a); //ceil: arrotonda all'intero superiore
      if(temp_sum_b > global_capacity_b)
          num_bins_new_b = num_bins + (int)ceil((double)(temp_sum_b-global_capacity_b)/bin_capacity_b); //ceil: arrotonda all'intero superiore


      //INIZIO MINIMO
      /*
      int min_a = istanza[0][1],min_b = istanza[0][2];
      for(int j=i;j<n;j++){
          if(istanza[j][1] < min_a) min_a = istanza[j][1];
          if(istanza[j][2] < min_b) min_b = istanza[j][2];
      }
      if(min_a*(n-i+1) > global_capacity_a)
          num_bins_new_a = num_bins + (int)ceil((double)((min_a*(n-i+1))-global_capacity_a)/bin_capacity_a);
      if(min_b*(n-i+1) > global_capacity_b)
          num_bins_new_b = num_bins + (int)ceil((double)((min_b*(n-i+1))-global_capacity_b)/bin_capacity_b);
      */

      if(num_bins_new_a > num_bins_new_b)
          num_bins_new = num_bins_new_a;
      else num_bins_new = num_bins_new_b;


      if(num_bins_new > num_bins){
          cout << "INCREMENTO PRO!! Passo da " << num_bins << " a " << num_bins_new << endl;

          for(int z=num_bins;z<num_bins_new;z++){
              bin_capacities[z][0] = bin_capacity_a;
              bin_capacities[z][1] = bin_capacity_b;
              global_capacity_a += bin_capacity_a;
              global_capacity_b += bin_capacity_b;
          }
          num_bins = num_bins_new;
      }

      //Aggiorno le capacità del bin selezionato
      bin_capacities[bin_index][0] -= istanza[obj_index][1];
      bin_capacities[bin_index][1] -= istanza[obj_index][2];

      //Assegno l'oggetto i-esimo al bin
      bin_assigment[obj_index] = bin_index;
    }

    if(DEBUG || DEBUG_PRO) cout << "\n\n\n\nNUMERO MINIMO BIN: " << binIniziali << "\n\n";
    if(DEBUG || DEBUG_PRO) cout << "NUMERO BIN APERTI: " << num_bins << "\n";
    ofstream output_file;
    output_file.open("Benchmark_Greedy.txt", ofstream::out | ofstream::app);
    if(compare_function == 4)
        output_file << num_bins << ";\n";
    else
        output_file << num_bins << ";";
    output_file.close();
    return bin_assigment;

}




//Si cerca l'oggetto più grosso che sta in uno dei bin
int * getStructuredMaxFill(int istanza[][3],int n, int bin_capacities[][2], int bin_capacity_a, int bin_capacity_b, int num_bins, int* bin_assignment, bool DEBUG, int* index)
{
    double max = 0;
    int index_max = -1;
    int index_bin_max = -1;

    //Per ognuno dei bin aperti
    for(int j=0; j<num_bins;j++)
    {
        if(DEBUG)
            cout << "BIN: " << j << " CAPACITY_A: " << bin_capacities[j][0] << " CAPACITY_B: " << bin_capacities[j][1] << "\n";

        //Per ognuno degli oggetti
        for(int i=0;i<n;i++)
        {
            //Se l'oggetto non è ancora stato assegnato
            if(bin_assignment[i] == -1)
            {
                //Se l'oggetto sta nella capacità corrente
                if(istanza[i][1] <= bin_capacities[j][0] && istanza[i][2] <= bin_capacities[j][1])
                {
                    //Non ho ancora trovato il maxFill
                    if(index_max == -1)
                    {
                        //Se la dimensione 1 è dominante assegno il max al valore della 1
                        if((double)istanza[i][1]/bin_capacity_a > (double)istanza[i][2]/bin_capacity_b)
                            max = (double)istanza[i][1]/bin_capacity_a;
                        //Se la dimensione 2 è dominante
                        else
                            max = (double)istanza[i][2]/bin_capacity_b;
                        //Aggiorno l'indice dell'oggetto maxFill
                        index_max = i;
                        index_bin_max = j;
                    }
                    //Ho già trovato un massimo
                    else
                    {
                        //Se la dimensione 1 è dominante
                        if((double)istanza[i][1]/bin_capacity_a > (double)istanza[i][2]/bin_capacity_b)
                        {
                            //Se la dimensione è maggiore del massimo attuale aggiorno gli indici
                            if((double)istanza[i][1]/bin_capacities[j][0] > max)
                            {
                                max = (double)istanza[i][1]/bin_capacity_a;
                                index_max = i;
                                index_bin_max = j;
                            }
                        }
                        //Se la dimensione 2 è dominante
                        else
                        {
                            if((double)istanza[i][2]/bin_capacity_b > max)
                            {
                                max = (double)istanza[i][2]/bin_capacity_b;
                                index_max = i;
                                index_bin_max = j;
                            }
                        }
                    }
                }
            }
        }
    }
    index[0] = index_bin_max;
    index[1] = index_max;
    if(DEBUG)
        cout << "MAXFILL: " << index_max+1 << ";";
    return index;
}


//Si cerca l'oggetto più grosso che sta in uno dei bin
int * getStructuredMaxFillMean(int istanza[][3],int n, int bin_capacities[][2], int bin_capacity_a, int bin_capacity_b, int num_bins, int* bin_assignment, bool DEBUG, int* index)
{
    double max = 0;
    int index_max = -1;
    int index_bin_max = -1;

    //Per ognuno dei bin aperti
    for(int j=0; j<num_bins;j++)
    {
        if(DEBUG)
            cout << "BIN: " << j << " CAPACITY_A: " << bin_capacities[j][0] << " CAPACITY_B: " << bin_capacities[j][1] << "\n";

        //Per ognuno degli oggetti
        for(int i=0;i<n;i++)
        {
            if(bin_assignment[i] == -1) //Se l'oggetto non è ancora stato assegnato
            {
                if(istanza[i][1] <= bin_capacities[j][0] && istanza[i][2] <= bin_capacities[j][1]) //Se l'oggetto sta nella capacità corrente
                {
                    //Non ho ancora trovato il maxFill
                    if(index_max == -1)
	                {
	                    //Il massimo è la media delle due capacità normalizzate
	                    max = (((double)istanza[i][1]/bin_capacity_a) + ((double)istanza[i][2]/bin_capacity_b))/2;
	                    //Aggiorno l'indice dell'oggetto maxFill
	                    index_max = i;
                        index_bin_max = j;
                    }
                    //Ho già trovato un massimo
                    else
                    {
                        if((((double)istanza[i][1]/bin_capacity_a) + ((double)istanza[i][2]/bin_capacity_b))/2 > max)
	                    {
	                        max = (((double)istanza[i][1]/bin_capacity_a) + ((double)istanza[i][2]/bin_capacity_b))/2;
	                        index_max = i;
                        	index_bin_max = j;
	                    }
	                }
                }
            }
        }
    }
    index[0] = index_bin_max;
    index[1] = index_max;
    if(DEBUG)
        cout << "MAXFILLMEAN: " << index_max+1 << "\n";
    return index;
}


//Si cerca l'oggetto più grosso che sta in uno dei bin
int * getStructuredWeightedMean(int istanza[][3],int n, int bin_capacities[][2], int bin_capacity_a, int bin_capacity_b, int num_bins, int* bin_assignment, bool DEBUG, int* index)
{
    double max = 0;
    int index_max = -1;
    int index_bin_max = -1;

    //Per ognuno dei bin aperti
    for(int j=0; j<num_bins;j++)
    {
        if(DEBUG)
            cout << "BIN: " << j << " CAPACITY_A: " << bin_capacities[j][0] << " CAPACITY_B: " << bin_capacities[j][1] << "\n";

        //Per ognuno degli oggetti
        for(int i=0;i<n;i++)
        {
            if(bin_assignment[i] == -1) //Se l'oggetto non è ancora stato assegnato
            {
                if(istanza[i][1] <= bin_capacities[j][0] && istanza[i][2] <= bin_capacities[j][1]) //Se l'oggetto sta nella capacità corrente
                {
                    double max_el;
                    if((double)istanza[i][1]/bin_capacity_a > (double)istanza[i][2]/bin_capacity_b) max_el = (double)istanza[i][1]/bin_capacity_a;
                    else (double)istanza[i][2]/bin_capacity_b;

                    //Non ho ancora trovato il maxFill
                    if(index_max == -1)
	                {
	                    //Il massimo è la media delle due capacità normalizzate
	                    max = max_el*(((double)istanza[i][1]/bin_capacity_a) + ((double)istanza[i][2]/bin_capacity_b))/2;
	                    //Aggiorno l'indice dell'oggetto maxFill
	                    index_max = i;
                        index_bin_max = j;
                    }
                    //Ho già trovato un massimo
                    else
                    {
                        if(max_el*(((double)istanza[i][1]/bin_capacity_a) + ((double)istanza[i][2]/bin_capacity_b))/2 > max)
	                    {
	                        max = max_el*(((double)istanza[i][1]/bin_capacity_a) + ((double)istanza[i][2]/bin_capacity_b))/2;
	                        index_max = i;
                        	index_bin_max = j;
	                    }
	                }
                }
            }
        }
    }
    index[0] = index_bin_max;
    index[1] = index_max;
    if(DEBUG)
        cout << "MAXFILLMEAN: " << index_max+1 << "\n";
    return index;
}


//Si cerca l'oggetto con la maggiore differenza delle dimensioni
int * getStructuredMaxFillSub(int istanza[][3],int n, int bin_capacities[][2], int bin_capacity_a, int bin_capacity_b, int num_bins, int* bin_assignment, bool DEBUG, int* index)
{
    double max = 0;
    int index_max = -1;
    int index_bin_max = -1;

    //Per ognuno dei bin aperti
    for(int j=0; j<num_bins;j++)
    {
        if(DEBUG)
            cout << "BIN: " << j << " CAPACITY_A: " << bin_capacities[j][0] << " CAPACITY_B: " << bin_capacities[j][1] << "\n";

        //Per ognuno degli oggetti
        for(int i=0;i<n;i++)
        {
            if(bin_assignment[i] == -1) //Se l'oggetto non è ancora stato assegnato
            {
                if(istanza[i][1] <= bin_capacities[j][0] && istanza[i][2] <= bin_capacities[j][1]) //Se l'oggetto sta nella capacità corrente
                {
                    //Non ho ancora trovato il maxFill
                    if(index_max == -1)
	                {
	                    //Il massimo è la differenza delle due capacità normalizzate
	                    max = abs(((double)istanza[i][1]/bin_capacity_a) - ((double)istanza[i][2]/bin_capacity_b));
	                    //Aggiorno l'indice dell'oggetto maxFill
	                    index_max = i;
                        index_bin_max = j;
                    }
                    //Ho già trovato un massimo
                    else
                    {
                        if(abs(((double)istanza[i][1]/bin_capacity_a) - ((double)istanza[i][2]/bin_capacity_b)) > max)
	                    {
	                        max = abs(((double)istanza[i][1]/bin_capacity_a) - ((double)istanza[i][2]/bin_capacity_b));
	                        index_max = i;
                        	index_bin_max = j;
	                    }
	                }
                }
            }
        }
    }
    index[0] = index_bin_max;
    index[1] = index_max;
    if(DEBUG)
        cout << "MAXFILLMEAN: " << index_max+1 << "\n";
    return index;
}


//Si cerca l'elemento più grosso che stia nel bin corrente, secondo la dimensione dominante dell'oggetto
int getMaxFill(int istanza[][3],int n, int capacity_a, int capacity_b, int* bin_assignment, bool DEBUG)
{
    double max = 0;
    int index_max = -1;
    if(DEBUG)
        cout << "CAPACITY_A: " << capacity_a << " CAPACITY_B: " << capacity_b << "\n";
    for(int i=0;i<n;i++)
    {
        if(bin_assignment[i] == -1) //Se l'oggetto non è ancora stato assegnato
        {
            if(istanza[i][1] <= capacity_a && istanza[i][2] <= capacity_b) //Se l'oggetto sta nella capacità corrente
            {
                //Non ho ancora trovato il maxFill
                if(index_max == -1)
                {
                    //Se la dimensione 1 è relativamente più "grossa" della 2 assegno il max al valore della 1
                    if((double)istanza[i][1]/capacity_a > (double)istanza[i][2]/capacity_b)
                        max = (double)istanza[i][1]/capacity_a;
                    else
                        max = (double)istanza[i][2]/capacity_b;
                    //Aggiorno l'indice dell'oggetto maxFill
                    index_max = i;
                }
                //Ho già trovato un massimo
                else
                {
                    if((double)istanza[i][1]/capacity_a > (double)istanza[i][2]/capacity_b)
                    {
                        if((double)istanza[i][1]/capacity_a > max)
                        {
                            max = (double)istanza[i][1]/capacity_a;
                            index_max = i;
                        }
                    }
                    else
                    {
                        if((double)istanza[i][2]/capacity_b > max)
                        {
                            max = (double)istanza[i][2]/capacity_b;
                            index_max = i;
                        }
                    }
                }
            }
        }
    }
    if(DEBUG)
        cout << "MAXFILL: " << index_max+1 << "\n";
    return index_max;
}
