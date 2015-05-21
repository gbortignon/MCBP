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

/***** Determinazione della strategia di base *****/
//Soluzione classica, apre un bin e lo riempie secondo una delle strategie. Quando è pieno ne apre un altro.
int * getClassicSolution(int[][3], int, int, int,int, int*,bool);
//Si calcola il numero minimo di bin utilizzabili considerando gli elementi come continui dividendo per il loro numero
int * getStructuredSolution(int[][3], int, int, int,int, int*, bool);
//Si calcola il numero minimo di bin utilizzabili considerando gli elementi come continui dividendo per il loro numero ricalcolando tale valore ogni volta che si inserisce un elemento
int * getStructuredProSolution(int[][3], int, int, int,int, int*, bool);


/***** Strategie per la soluzione classica *****/
int getMaxFill(int[][3], int, int, int, int*,bool);
int getMax(int[][3], int, int, int, int*,bool);
int getMin(int[][3], int, int, int, int*,bool);
//Si cerca l'elemento più grosso che stia nel bin corrente, secondo la media delle dimensioni dell'oggetto.
//Cerco quindi di inserire elementi che hanno entrambe le dimensioni "grandi"
int getMaxFillMean(int[][3], int, int, int, int*,bool);
//Prende sempre l'oggetto che riempe meglio la dimensione più vuota, in modo da allineare le due dimensioni
int getMaxFillAlternate(int[][3], int, int, int, int*,bool);

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
    //Classico
    case(1):
        bin_assignment = getClassicSolution(istanza,n,bin_capacity_a,bin_capacity_b,compare_function, bin_assignment, DEBUG);
        break;
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



//Soluzione classica, apre un bin e lo riempie secondo una delle strategie. Quando è pieno ne apre un altro.
int * getClassicSolution(int istanza[][3], int n, int bin_capacity_a, int bin_capacity_b,int compare_function, int * bin_assigment, bool DEBUG)
{

    //Inizializzo l'assegnamento a -1, in modo da sapere gli oggetti non ancora assegnati
    for(int i=0;i<n;i++)
        bin_assigment[i] = -1;

    //Numero di bin aperti
    int current_bin = 1;

    int current_bin_capacity_a = bin_capacity_a;
    int current_bin_capacity_b = bin_capacity_b;

    for(int i=0; i<n;i++)
    {
        int index;

        //Estraggo l'elemento da inserire, in base alla compare function
        switch(compare_function)
        {
        //MaxFill
        case(1):
            index = getMaxFill(istanza,n,current_bin_capacity_a,current_bin_capacity_b, bin_assigment, DEBUG);

            //Se non ci sono elementi che stanno nel bin, si prende il più grosso che sarà il primo inserito
            if(index == -1)
            {
                if(DEBUG)
                    cout << "Rimpito bin numero " << current_bin-1 << " !!!\n\n";
                index = getMax(istanza,n,bin_capacity_a,bin_capacity_b, bin_assigment, DEBUG);
            }
            break;
        //Minimo
        case(2):
            index = getMin(istanza,n,bin_capacity_a, bin_capacity_b,  bin_assigment, DEBUG);
            break;
        //Alterno massimo e minimo per mantenere stabile la media
        case(3):
            if(i%2 == 0)
                index = getMax(istanza,n,bin_capacity_a, bin_capacity_b,  bin_assigment, DEBUG);
            if(i%2 == 1)
                index = getMin(istanza,n,bin_capacity_a, bin_capacity_b,  bin_assigment, DEBUG);
            break;
        //Alterno maxFill e minimo
        case(4):
            if(i%2 == 0)
            {
                index = getMaxFill(istanza,n,current_bin_capacity_a,current_bin_capacity_b,  bin_assigment, DEBUG);

                //Se non ci sono elementi che stanno nel bin, si prende il più grosso che sarà il primo inserito
                if(index == -1)
                    index = getMax(istanza,n,bin_capacity_a,bin_capacity_b, bin_assigment, DEBUG);
            }
            if(i%2 == 1)
                index = getMin(istanza,n,bin_capacity_a,bin_capacity_b,  bin_assigment, DEBUG);
            break;
        //Solo Massimo
        case(5):
            index = getMax(istanza,n,bin_capacity_a,bin_capacity_b,  bin_assigment, DEBUG);
            break;
        //maxFill media
        case(6):
            index = getMaxFillMean(istanza,n,current_bin_capacity_a,current_bin_capacity_b, bin_assigment, DEBUG);

            //Se non ci sono elementi che stanno nel bin, si prende il più grosso che sarà il primo inserito
            if(index == -1)
            {
                if(DEBUG)
                    cout << "Rimpito bin numero " << current_bin-1 << " !!!\n\n";
                index = getMax(istanza,n,bin_capacity_a,bin_capacity_b, bin_assigment, DEBUG);
            }
            break;
        //maxFill mindim
        case(7):
            index = getMaxFillAlternate(istanza,n,current_bin_capacity_a,current_bin_capacity_b, bin_assigment, DEBUG);

            //Se non ci sono elementi che stanno nel bin, si prende il più grosso che sarà il primo inserito
            if(index == -1)
            {
                if(DEBUG)
                    cout << "Rimpito bin numero " << current_bin-1 << " !!!\n\n";
                index = getMax(istanza,n,bin_capacity_a,bin_capacity_b, bin_assigment, DEBUG);
            }
            break;
        }


        //Se c'è abbastanza spazio aggiorno le capacità
        if(istanza[index][1] <= current_bin_capacity_a && istanza[index][2] <= current_bin_capacity_b)
        {
            current_bin_capacity_a -= istanza[index][1];
            current_bin_capacity_b -= istanza[index][2];
        }
        //Altrimenti apro un nuovo bin
        else
        {
            current_bin++;
            current_bin_capacity_a = bin_capacity_a - istanza[index][1];
            current_bin_capacity_b = bin_capacity_b - istanza[index][2];
        }
        //Assegno l'oggetto i-esimo al bin corrente
        bin_assigment[index] = current_bin;
    }
    if(DEBUG) cout << "\n\n\n\nNUMERO BIN APERTI: " << current_bin << "\n";

    ofstream output_file;
    output_file.open("Benchmark_Greedy.txt", ofstream::out | ofstream::app);
    output_file << current_bin << ";";
    output_file.close();
    return bin_assigment;
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
    num_bins = floor(num_bins*0.70);
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

        //Decremento la capacità totale per ricalcolare il numero di bin
        global_capacity_a -= istanza[i][1];
        global_capacity_b -= istanza[i][2];

        int num_bins_new = 0,num_bins_new_b = 0,num_bins_new_a = 0;
        //INIZIO SOMMA
        /*//Stima del numero dei bin considerandoli continui
        int temp_sum_a = 0;
        int temp_sum_b = 0;
        for(int j=i;j<n;j++)
        {
            temp_sum_a += istanza[j][1];
            temp_sum_b += istanza[j][2];
        }

        if(temp_sum_a > global_capacity_a)
            num_bins_new_a = num_bins + (int)ceil((double)(temp_sum_a-global_capacity_a)/bin_capacity_a); //ceil: arrotonda all'intero superiore
        if(temp_sum_b > global_capacity_b)
            num_bins_new_b = num_bins + (int)ceil((double)(temp_sum_b-global_capacity_b)/bin_capacity_b); //ceil: arrotonda all'intero superiore
        */
        
        //INIZIO MINIMO
        int min_a = istanza[0][1],min_b = istanza[0][2];
        for(int j=i;j<n;j++){
            if(istanza[j][1] < min_a) min_a = istanza[j][1];
            if(istanza[j][2] < min_b) min_b = istanza[j][2];
        }
        if(min_a*(n-i+1) > global_capacity_a)
            num_bins_new_a = num_bins + (int)ceil((double)((min_a*(n-i+1))-global_capacity_a)/bin_capacity_a);
        if(min_b*(n-i+1) > global_capacity_b)
            num_bins_new_b = num_bins + (int)ceil((double)((min_b*(n-i+1))-global_capacity_b)/bin_capacity_b);

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



//Si cerca l'elemento più grosso che stia nel bin corrente, secondo la media delle dimensioni dell'oggetto.
//Cerco quindi di inserire elementi che hanno entrambe le dimensioni "grandi"
int getMaxFillMean(int istanza[][3],int n, int capacity_a, int capacity_b, int* bin_assignment, bool DEBUG)
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
                    //Il massimo è la media delle due capacità normalizzate
                    max = (((double)istanza[i][1]/capacity_a) + ((double)istanza[i][2]/capacity_b))/2;
                    //Aggiorno l'indice dell'oggetto maxFill
                    index_max = i;
                }
                //Ho già trovato un massimo
                else
                {
                    //Se la media attuale è maggiore della massima trovata
                    if((((double)istanza[i][1]/capacity_a) + ((double)istanza[i][2]/capacity_b))/2 > max)
                    {
                        max = (((double)istanza[i][1]/capacity_a) + ((double)istanza[i][2]/capacity_b))/2;
                        index_max = i;
                    }
                }
            }
        }
    }
    if(DEBUG)
        cout << "MAXFILL: " << index_max+1 << "\n";
    return index_max;
}


//Prende sempre l'oggetto che riempe meglio la domensione più vuota, in modo da allineare le due dimensioni
int getMaxFillAlternate(int istanza[][3],int n, int capacity_a, int capacity_b, int* bin_assignment, bool DEBUG)
{
    double max = 0;
    int index_max = -1;
    int minCapacity;

    //Verifico qual'è la dimensione del bin più "vuota" e la fisso per i confronti
    if(capacity_a < capacity_b)
        minCapacity = 1;
    else minCapacity = 2;

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
                    if(minCapacity == 1)
                    {
                        max = istanza[i][1];
                        index_max = i;
                    }
                    if(minCapacity == 2)
                    {
                        max = istanza[i][2];
                        index_max = i;
                    }
                }
                //Ho già trovato un massimo
                else
                {
                    if(minCapacity == 1)
                    {
                        if(istanza[i][1] > max)
                        {
                            max = istanza[i][1];
                            index_max = i;
                        }
                    }
                    if(minCapacity == 2)
                    {
                        if(istanza[i][2] > max)
                        {
                            max = istanza[i][2];
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


//Inserisce l'oggetto più grande non ancora assegnato, se poi non ci sta si apre un nuovo bin
int getMax(int istanza[][3],int n, int capacity_a, int capacity_b, int* bin_assignment, bool DEBUG)
{
    if(DEBUG)
        cout << "CAPACITY_A: " << capacity_a << " CAPACITY_B: " << capacity_b << "\n";
    double max = 0;
	int index_max = -1;
	for(int i = 0; i<n; i++)
    {
         if(bin_assignment[i] == -1) //Se l'oggetto non è ancora stato assegnato
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
	if(DEBUG)
        cout << "MAX: " << index_max+1 << "\n";
	return index_max;
}


//Inserisce l'oggetto più piccolo non ancora assegnato, se poi non ci sta si apre un nuovo bin
int getMin(int istanza[][3],int n, int capacity_a, int capacity_b, int* bin_assignment, bool DEBUG)
{
    if(DEBUG)
        cout << "CAPACITY_A: " << capacity_a << " CAPACITY_B: " << capacity_b << "\n";
    double min = numeric_limits<double>::max();
	int index_min = -1;
	for(int i = 0; i<n; i++)
    {
         if(bin_assignment[i] == -1) //Se l'oggetto non è ancora stato assegnato
        {
            if((double)istanza[i][1]/capacity_a < (double)istanza[i][2]/capacity_b)
            {
                if((double)istanza[i][1]/capacity_a < min)
                {
                    min = (double)istanza[i][1]/capacity_a;
                    index_min = i;
                }
            }
            else
            {
                if((double)istanza[i][2]/capacity_b < min)
                {
                    min = (double)istanza[i][2]/capacity_b;
                    index_min = i;
                }
            }
        }
	}
	if(DEBUG)
        cout << "MIN: " << index_min+1 << "\n";
	return index_min;
}


