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
#include <algorithm>
#include <vector>

/***** Determinazione della strategia di base *****/
//Soluzione classica, apre un bin e lo riempie secondo una delle strategie. Quando è pieno ne apre un altro.
int * getClassicSolution(int[][3], int, int, int,int, int*,bool);
//Si calcola il numero minimo di bin utilizzabili considerando gli elementi come continui dividendo per il loro numero
int * getStructuredSolution(int[][3], int, int, int,int, int*, bool);
//Si calcola il numero minimo di bin utilizzabili considerando gli elementi come continui dividendo per il loro numero ricalcolando tale valore ogni volta che si inserisce un elemento
int * getStructuredProSolution(int[][3], int, int, int,int, int*, bool);


/***** Strategie per la soluzione classica *****/
//Si cerca l'elemento più grosso che stia nel bin corrente, secondo la media delle dimensioni dell'oggetto.
//Cerco quindi di inserire elementi che hanno entrambe le dimensioni "grandi"
int getMaxFillMean(int[][3], int, int, int, int, int, int*,bool);
int getMax(int[][3], int, int, int, int*,bool);

/***** Strategie per la soluzione strutturata *****/
int * getStructuredMaxFillMean(int[][3], int, int[][2], int, int, int, int*, bool, int*);

void checkSolutionValues(int[][3], int*, int, int, int);


//Interfaccia esterna per determinare se utilizzare soluzione classica o strutturata
int* solGreedy(int istanza[][3], int n, int bin_capacity_a, int bin_capacity_b,int compare_function,int structured, bool DEBUG)
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
			if(soluzione[i] == -1){
				cout << "OGGETTO NON ASSEGNATO!!" << endl;
				exit(1);
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

        index = getMaxFillMean(istanza,n,current_bin_capacity_a,current_bin_capacity_b,bin_capacity_a,bin_capacity_b, bin_assigment, DEBUG);

        //Se non ci sono elementi che stanno nel bin, si prende il più grosso che sarà il primo inserito
        if(index == -1)
        {
            if(DEBUG)
                cout << "Rimpito bin numero " << current_bin-1 << " !!!\n\n";
            index = getMax(istanza,n,bin_capacity_a,bin_capacity_b, bin_assigment, DEBUG);
            if(index == -1){
				cout << "NON SO DOVE METTERLO CLASSIC!!" << endl;
				exit(1);
			}
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

        index = getStructuredMaxFillMean(istanza,n,bin_capacities, bin_capacity_a, bin_capacity_b,num_bins, bin_assigment, DEBUG, index);
        
		//Se non ci sono elementi che stanno nei bin aperti, si prende il più grosso che sarà il primo inserito nel nuovo bin
        if(index[1] == -1)
        {
            if(DEBUG)
                cout << "Rimpiti i primi " << num_bins << " bin!!!\n\n";
            if(numEsclusi == 0) numEsclusi=n-i+1;
            index[0] = num_bins; //Inizio a riempire un nuovo bin
            index[1] = getMaxFillMean(istanza,n,bin_capacity_a,bin_capacity_b,bin_capacity_a,bin_capacity_b, bin_assigment, DEBUG);
			if(index[1] == -1){
				cout << "NON SO DOVE METTERLO!!" << endl;
				exit(1);
			}
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
        /*
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
        */
        /*
        int temp_sum_a = 0;
        int temp_sum_b = 0;
        for(int j=i;j<n;j++)
        {
            temp_sum_a += istanza[j][1];
            temp_sum_b += istanza[j][2];
        }

        if((double)temp_sum_a/(n-i) > (double)global_capacity_a/num_bins)
            num_bins_new_a = (int)ceil((double)((n-i)*global_capacity_a)/temp_sum_a); //ceil: arrotonda all'intero superiore
        if((double)temp_sum_b/(n-i) > (double)global_capacity_a/num_bins)
            num_bins_new_b = (int)ceil((double)((n-i)*global_capacity_b)/temp_sum_b); //ceil: arrotonda all'intero superiore
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

        index = getStructuredMaxFillMean(istanza,n,bin_capacities, bin_capacity_a, bin_capacity_b,num_bins, bin_assigment, DEBUG, index);
	        
		//Se non ci sono elementi che stanno nei bin aperti, si prende il più grosso che sarà il primo inserito nel nuovo bin
        if(index[1] == -1)
        {
            if(DEBUG)
                cout << "Rimpiti i primi " << num_bins << " bin!!!\n\n";
            if(numEsclusi == 0) numEsclusi=n-i+1;
            index[0] = num_bins; //Inizio a riempire un nuovo bin
            index[1] = getMaxFillMean(istanza,n,bin_capacity_a,bin_capacity_b,bin_capacity_a,bin_capacity_b, bin_assigment, DEBUG);
			if(index[1] == -1){
				cout << "NON SO DOVE METTERLO PRO!!" << endl;
				exit(1);
			}
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
int * getStructuredMaxFillMean(int istanza[][3],int n, int bin_capacities[][2], int bin_capacity_a, int bin_capacity_b, int num_bins, int* bin_assignment, bool DEBUG, int* index)
{
    int k = ceil(n*0.1);
    //Contiene il valore dell'euristica degli oggetti da assegnare;
    double* objs = new double[n]; 
    //In posizione i-esima contiene l'indice del bin a cui assegnare l'oggetto i; 
    int* bin_objs_index = new int[n];
    //In posizione i-esima contiene l'indice dell'oggetto; 
    int* objs_index = new int[n];
    int num_objs = 0;
	bool max_trovato = false;
	
    //Per ognuno degli oggetti
    for(int i=0;i<n;i++)
    {
    	double max = -1;
    	int bin_obj_index = -1;
        if(bin_assignment[i] == -1) //Se l'oggetto non è ancora stato assegnato
        {
        	//Per ognuno dei bin aperti
		    for(int j=0; j<num_bins;j++)
		    {
				//Se l'oggetto sta nella capacità corrente
                if(istanza[i][1] <= bin_capacities[j][0] && istanza[i][2] <= bin_capacities[j][1]){ 
			    	double obj_dim = (((double)istanza[i][1]/bin_capacity_a) + ((double)istanza[i][2]/bin_capacity_b))/2;
			    	if(max < obj_dim){
			    		max = obj_dim;
			    		bin_obj_index = j;
			    		max_trovato = true;
			    	}
                }
		    }
		    //Se l'oggetto sta in qualche bin
		    if(bin_obj_index != -1){
				// Salvo il max valore di euristica per l'oggetto num_objs-esimo
				objs[num_objs] = max;
				// Salvo l'indice migliore a cui assegnare l'oggetto num_objs-esimo
				bin_objs_index[num_objs] = bin_obj_index;
				// Abbino l'inidice num_objs a i;
				objs_index[num_objs] = i;
				num_objs++;
			}
		}
	}
	if(num_objs > 0){
		double* ordered_objs = new double[num_objs];
		double* original_objs = new double[num_objs];
		for(int i=0;i<num_objs;i++){
			ordered_objs[i] = objs[i];
			original_objs[i] = objs[i];
		}
		
		sort(ordered_objs, ordered_objs+num_objs); 
		
		int random_index;
		if(k < num_objs)
			random_index = rand()%k;
		else 
			random_index = rand()%num_objs;
		double random_obj = ordered_objs[num_objs - random_index - 1];
		
		int indice_oggetto;
		for(int i=0;i<num_objs;i++){
			if(original_objs[i] == random_obj)
				indice_oggetto = i;
		}

		int index_bin_max = bin_objs_index[indice_oggetto];
		int index_max = objs_index[indice_oggetto];
		
		if(max_trovato){
			index[0] = index_bin_max;
			index[1] = index_max;
		}
		else{
			index[0] = -1;
			index[1] = -1;
		}
		if(DEBUG)
			cout << "MAXFILLMEAN: " << index_max+1 << "\n";
		return index;
	}
	else{
		index[0] = -1;
		index[1] = -1;
		return index;
	}
}


//Si cerca l'elemento più grosso che stia nel bin corrente, secondo la media delle dimensioni dell'oggetto.
//Cerco quindi di inserire elementi che hanno entrambe le dimensioni "grandi"
int getMaxFillMean(int istanza[][3],int n, int capacity_a, int capacity_b, int bin_capacity_a, int bin_capacity_b, int* bin_assignment, bool DEBUG)
{
    int k = ceil(n*0.1);
    //Contiene gli oggetti da assegnare;
    double* objs = new double[n]; 
    //In posizione i-esima contiene l'indice dell'oggetto; 
    int* objs_index = new int[n];
    int num_objs = 0;
	bool max_trovato = false;
	
    //Per ognuno degli oggetti
    for(int i=0;i<n;i++)
    {
        if(bin_assignment[i] == -1) //Se l'oggetto non è ancora stato assegnato
        {
			if(istanza[i][1] <= capacity_a && istanza[i][2] <= capacity_b){ //Se l'oggetto sta nella capacità corrente
				double obj_dim = (((double)istanza[i][1]/capacity_a) + ((double)istanza[i][2]/capacity_b))/2;
				// Salvo il max valore di euristica per l'oggetto num_objs-esimo
				objs[num_objs] = obj_dim;
				// Abbino l'inidice num_objs a i;
				objs_index[num_objs] = i;
				num_objs++;
				max_trovato = true;
			}
		}
	}
	if(num_objs > 0){
		double* ordered_objs = new double[num_objs];
		double* original_objs = new double[num_objs];
		for(int i=0;i<num_objs;i++){
			ordered_objs[i] = objs[i];
			original_objs[i] = objs[i];
		}
		
		sort(ordered_objs, ordered_objs+num_objs); 
		
		int random_index;
		if(k < num_objs){ 
			random_index = rand() % k;
		}
		else{ 
			random_index = rand() % num_objs;
		}
		double random_obj = ordered_objs[num_objs - random_index - 1];
		
		//cout << k << " " << num_objs << " " << random_index << " " << num_objs - random_index - 1 << endl;
		
		int indice_oggetto;
		for(int i=0;i<num_objs;i++){
			if(original_objs[i] == random_obj)
				indice_oggetto = i;
		}
		

		int index_max = objs_index[indice_oggetto];
		
		if(!max_trovato){
			index_max = -1;
		}
		if(DEBUG)
			cout << "MAXFILLMEAN: " << index_max+1 << "\n";
		return index_max;
	}
	else
		return -1;
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

