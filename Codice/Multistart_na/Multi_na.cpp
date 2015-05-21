using namespace std;

#include <iostream>
#include <cstdlib>
#include <climits>
#include <fstream>
#include <time.h>
#include <math.h>

#include "../Greedy/Greedy.hpp"


int* genera_sol_casuale(int[][3], int, int, int, int*);
int getPiuVuotoIndex();
bool sposto(int[][3]);
bool scambio(int[][3]);
bool checkSolutionValues(int[][3]);
void stampaSol();
void stampaBestSol();
void stampaSpaziLiberi();
int contaBinsAperti();
void checkClosedBins();
bool applica_strategia_scambio(int[][3], int, int);
int get_best_bin_maxFillMean(int[][3], int);
bool esci_ammissibilita_piu_vuoto(int[][3]);
void svuota_bin_piu_vuoto(int[][3]);
bool scambio_na(int[][3]);
bool sposto_na(int[][3]);

bool DEBUG_VERBOSE = 0;
bool stampa_su_file = 1;

// # restart di default
int restart = 10;

static int best_bins;
static int max_bins;
static int n;
static int bin_capacity_a;
static int bin_capacity_b;
static int *curr_sol;
static int *best_sol;
static int *closedBins;
static int *sp_lib_a;
static int *sp_lib_b;
static int bins_aperti;
static int bin_piu_vuoto;
static int loop;
static int inammissibilita_tot;
static int last_scambio[2];

static int strategia_generazione;
static int strategia_scambio;

static int spostamenti;
static int scambi;

void solMulti(int istanza[][3], int num_obj, int capacita_a, int capacita_b, string strategia_g, string strategia_s, string restarts, bool DEBUG)
{

	int i,j,k;
	n=num_obj;
	bin_capacity_a = capacita_a;
	bin_capacity_b = capacita_b;

	best_sol = new int[n];
	best_bins = INT_MAX;
	bool mosso;

	/*
	 * 1 - Tenta di riempire al massimo i bin già pieni
	 * 2 - Tenta di svuotare al massimo il bin più vuoto basandosi sulla somma delle due dimensioni
	 * 3 - Tenta di svuotare al massimo il bin più vuoto basandosi sulla media delle due dimensioni
	 */
	strategia_scambio = atoi(strategia_s.c_str());
	/*
	 * 1 - La soluzione casuale viene generata per intero in modo casuale
	 * 2 - La soluzione casuale viene generata per l'80% casualmente, quindi riempiendo i bin in ordine
	 */
	strategia_generazione = atoi(strategia_g.c_str());
	// Numero di restart per questa esecuzione
	restart = atoi(restarts.c_str());
	
	// Valore di default per il calcolo unico dell'upper bound di bins
	max_bins=-1;
	
	// Per ogni restart
	for(int r=0; r<restart; r++)
	{	
		spostamenti=0;
		scambi=0;
		curr_sol = new int[n];
		
		// Genero una soluzione random
		curr_sol = genera_sol_casuale(istanza, n, bin_capacity_a, bin_capacity_b, curr_sol);
		closedBins = new int[max_bins];
		
		// Chiusura dei bins non utilizzati
		checkClosedBins();

		// Sequenza di spostamenti e scambi: uno spostamento è sempre favorito a uno scambio
		mosso = true;
		while(mosso)
		{
			mosso=false;
			mosso = sposto(istanza);
			if(mosso) continue;
			mosso = scambio(istanza);
		}

		// Aggiorno il conto dei bin aperti e chiusi
		checkClosedBins();
		
		int best_bins_amm = bins_aperti;
		
		// Ricerca di una soluzione provando a uscire dall'ammissibilità
		mosso = true;
		// Continuo finchè non riesco a fare ne mosse ammissibili ne non ammissibili
		while(mosso)
		{
			mosso=false;
			mosso=esci_ammissibilita_piu_vuoto(istanza);
			bool mosso_interno=true;
			// Provo a spostare/scambiare per migliorare la soluzione ma SOLO nell'ammissibilità
			while(mosso_interno){
				mosso_interno = false;
				mosso_interno = sposto(istanza);
				if(mosso_interno){
					mosso = true;
					continue;
				}
				mosso_interno = scambio(istanza);
				if(mosso_interno) mosso = true;
			}
			mosso = checkSolutionValues(istanza) && mosso;
		}
		
		// Controllo eventuali difetti per ammissibilità, coerenza assegnamento e relativi valori di spazio vuoto
		bool sol_valida = checkSolutionValues(istanza);

		// Aggiornamento conto bin aperti e chiusi
		checkClosedBins();
		
		// Se la soluzione non è ammissibile si tiene quella ammissibile calcolata in precedenza
		// oppure uscendo dall'ammissibilità si ottiene un risultato peggiore
		if(!sol_valida || best_bins_amm < bins_aperti ){
			bins_aperti = best_bins_amm;
		}
		
		// Controllo se la soluzione che ho trovato è migliore delle precedenti
		if(bins_aperti<best_bins)
		{
			best_bins=bins_aperti;
			best_sol=curr_sol;
		}
	}
	
	if(DEBUG_VERBOSE){
		cout<<"SOLUZIONE: "<<best_bins<<"\tcon "<<restart<<" restart"<<endl;
		stampaSpaziLiberi();
		stampaSol();
		cout<<"Spostamenti: "<<spostamenti<<endl;
		cout<<"Scambi: "<<scambi<<endl;
	}

	if(stampa_su_file)
	{
		ofstream output_file;
		output_file.open("Benchmark_Multi_na.txt", ofstream::out | ofstream::app);
		output_file << best_bins;
		output_file.close();
	}
    return;
}

// Genera una soluzione casuale generando posizioni random per l'80% dei casi,
// dopodichè cerca ordinatamente una posizione ammissibile per i rimanenti oggetti
int* genera_sol_casuale(int istanza[][3], int n, int bin_capacity_a, int bin_capacity_b, int *result)
{
	// Si selezionano i massimi tra tutti i due tipi di peso per calcolare l'upper bound
	if(max_bins == -1){
		float max1 = istanza[0][1];
		float max2 = istanza[0][2];
		for(int i = 0; i<n; i++)
		{
			if(istanza[i][1] > max1) max1 = istanza[i][1];
			if(istanza[i][2] > max2) max2 = istanza[i][2];
		}
		
		// Determino l'upper bound
		if((double)max1/bin_capacity_a > (double)max2/bin_capacity_b)
			max_bins = n/floor(bin_capacity_a/max1);
		else
			max_bins = n/floor(bin_capacity_b/max2);
	}
	
	// Si inizializzano a vuoti i bin
	sp_lib_a = new int[max_bins];
	sp_lib_b = new int[max_bins];
	for(int i = 0; i<max_bins; i++)
	{
		sp_lib_a[i] = bin_capacity_a;
		sp_lib_b[i] = bin_capacity_b;
	}

	int bin_casuale;
	float perc = n*0.8;
	bool assegnato;

	// Per ogni oggetto
	for(int i = 0; i<n; i++)
	{
		// Continuo a estrarre numeri casuali finchè non riesco a piazzare l'oggetto i in un bin
		assegnato = false;
		// Devo continuare a ciclare finchè non riesco ad assegnare l'oggetto a un bin abbastanza vuoto da contenerlo
		while(!assegnato){
			// Strategia di generazione 80-20
			if(strategia_generazione==2)
			{
				// Se si è sotto la percentuale scelta si inseriscono casualmente gli oggetti nei bins
				if(i < perc)
				{
					//bin_casuale = g1.RandInt(0, max_bins);
					bin_casuale = rand() % max_bins;
					// se l'elemento può essere messo nel bin scelto (a caso)
					if(istanza[i][1] <= sp_lib_a[bin_casuale] && istanza[i][2] <= sp_lib_b[bin_casuale])
					{
						result[i] = bin_casuale;
						sp_lib_a[bin_casuale] -= istanza[i][1];
						sp_lib_b[bin_casuale] -= istanza[i][2];
						assegnato = true;
					}
				}
				// Quando si supera la soglia si inseriscono gli oggetti ordinatamente (sempre a patto che ci stiano in quel bin)
				else
				{
					if(istanza[i][1] <= sp_lib_a[bin_casuale] && istanza[i][2] <= sp_lib_b[bin_casuale])
					{
						result[i] = bin_casuale;
						sp_lib_a[bin_casuale] -= istanza[i][1];
						sp_lib_b[bin_casuale] -= istanza[i][2];
						assegnato = true;
					}
					bin_casuale = (bin_casuale+1)%max_bins;
				}
			}
		}
	}
	return result;
}

// Effettua lo spostamento di un oggetto dal bin più vuoto
bool sposto(int istanza[][3]){
	// prendo l'indice del bin più vuoto
	bin_piu_vuoto = getPiuVuotoIndex();
	int altro_bin;

	// per ogni oggetto
	for(int i=0; i<n; i++)
	{
		// se questo è nel bin più vuoto
		if(curr_sol[i] == bin_piu_vuoto)
		{
			// lo metto in un altro bin rispettando il max fill mean
			altro_bin = get_best_bin_maxFillMean(istanza, i);
			// Se non ho trovato un bin con cui fare lo spostamento ritorno false
			if(altro_bin < 0) return false;

			spostamenti++;

			// occupo lo spazio nel nuovo bin
			sp_lib_a[altro_bin] = sp_lib_a[altro_bin]-istanza[i][1];
			sp_lib_b[altro_bin] = sp_lib_b[altro_bin]-istanza[i][2];
			// libero lo spazio dal vecchio bin
			sp_lib_a[bin_piu_vuoto] = sp_lib_a[bin_piu_vuoto]+istanza[i][1];
			sp_lib_b[bin_piu_vuoto] = sp_lib_b[bin_piu_vuoto]+istanza[i][2];

			// sposto effettivamente
			curr_sol[i] = altro_bin;

			// se ho svuotato il bin più vuoto lo chiudo
			if(sp_lib_a[bin_piu_vuoto] == bin_capacity_a && sp_lib_b[bin_piu_vuoto] == bin_capacity_b)
				closedBins[bin_piu_vuoto]=1;

			// controllo che i cambiamenti che ho fatto siano consistenti
			if(sp_lib_a[curr_sol[i]] > bin_capacity_a || sp_lib_b[curr_sol[i]] > bin_capacity_b ||
				sp_lib_a[altro_bin] > bin_capacity_a || sp_lib_b[altro_bin] > bin_capacity_b ||
				sp_lib_a[curr_sol[i]] < 0 || sp_lib_b[curr_sol[i]] < 0 ||
				sp_lib_a[altro_bin] < 0 || sp_lib_b[altro_bin] < 0
			)
			{
				cout<<"oggetto "<<i<<" in spostamento"<<endl;
				cout<<"spazio bin di i A: "<<sp_lib_a[curr_sol[i]]<<endl;
				cout<<"spazio bin di i B: "<<sp_lib_b[curr_sol[i]]<<endl;
				cout<<"spazio bin j A: "<<sp_lib_a[altro_bin]<<endl;
				cout<<"spazio bin j B: "<<sp_lib_b[altro_bin]<<endl;
				exit(1);
			}			
			return true;
		}
	}
	return false;
}

// Effettua lo scambio secondo la strategia passata a parametro
bool scambio(int istanza[][3]){
	// parto tentando di scambiare elementi con il bin più vuoto in modo da renderlo ancora più vuoto
	bin_piu_vuoto = getPiuVuotoIndex();

	// per ogni oggetto
	for(int i=0; i<n; i++)
	{
		// se questo è nel bin più vuoto
		if(curr_sol[i] == bin_piu_vuoto)
		{
			// confrontalo con ogni altro oggetto...
			for(int j=0; j<n; j++)
			{
				// ...che non è nello stesso bin
				if(curr_sol[j] != bin_piu_vuoto)
				{
					// se uno scambio tra i due oggetti segue la strategia di scambio indicata
					if(applica_strategia_scambio(istanza, i, j))
					{
						scambi++;

						if(DEBUG_VERBOSE)
						{
							cout<<"PRIMA:"<<endl;
							cout<<"spazio bin i A: "<<sp_lib_a[curr_sol[i]]<<endl;
							cout<<"spazio bin i B: "<<sp_lib_b[curr_sol[i]]<<endl;
							cout<<"spazio bin j A: "<<sp_lib_a[curr_sol[j]]<<endl;
							cout<<"spazio bin j B: "<<sp_lib_b[curr_sol[j]]<<endl;
							cout<<"oggetto i A: "<<istanza[i][1]<<endl;
							cout<<"oggetto i B: "<<istanza[i][2]<<endl;
							cout<<"oggetto j A: "<<istanza[j][1]<<endl;
							cout<<"oggetto j B: "<<istanza[j][2]<<endl;
						}
						// aggiorno i valori di spazio libero per i bin
						sp_lib_a[curr_sol[i]] = sp_lib_a[curr_sol[i]]+istanza[i][1]-istanza[j][1];
						sp_lib_b[curr_sol[i]] = sp_lib_b[curr_sol[i]]+istanza[i][2]-istanza[j][2];
						sp_lib_a[curr_sol[j]] = sp_lib_a[curr_sol[j]]+istanza[j][1]-istanza[i][1];
						sp_lib_b[curr_sol[j]] = sp_lib_b[curr_sol[j]]+istanza[j][2]-istanza[i][2];

						if(DEBUG_VERBOSE)
						{
							cout<<"DOPO:"<<endl;
							cout<<"spazio bin i A: "<<sp_lib_a[curr_sol[i]]<<endl;
							cout<<"spazio bin i B: "<<sp_lib_b[curr_sol[i]]<<endl;
							cout<<"spazio bin j A: "<<sp_lib_a[curr_sol[j]]<<endl;
							cout<<"spazio bin j B: "<<sp_lib_b[curr_sol[j]]<<endl;
							cout<<"oggetto i A: "<<istanza[i][1]<<endl;
							cout<<"oggetto i B: "<<istanza[i][2]<<endl;
							cout<<"oggetto i A: "<<istanza[j][1]<<endl;
							cout<<"oggetto i B: "<<istanza[j][2]<<endl;
							cout<<"-----------------------------------------------------"<<endl<<endl;
						}

						// Se c'è un errore nell'aggiornamento degli spazi fermo l'esecuzione
						if(sp_lib_a[curr_sol[i]] > bin_capacity_a || sp_lib_b[curr_sol[i]] > bin_capacity_b ||
							sp_lib_a[curr_sol[j]] > bin_capacity_a || sp_lib_b[curr_sol[j]] > bin_capacity_b ||
							sp_lib_a[curr_sol[i]] < 0 || sp_lib_b[curr_sol[i]] < 0 ||
							sp_lib_a[curr_sol[j]] < 0 || sp_lib_b[curr_sol[j]] < 0
						)
						{
							cout<<"oggetto "<<i<<" in scambio"<<endl;
							cout<<"spazio bin i A: "<<sp_lib_a[curr_sol[i]]<<endl;
							cout<<"spazio bin i B: "<<sp_lib_b[curr_sol[i]]<<endl;
							cout<<"spazio bin j A: "<<sp_lib_a[curr_sol[j]]<<endl;
							cout<<"spazio bin j B: "<<sp_lib_b[curr_sol[j]]<<endl;
							cout<<"spazio i A: "<<sp_lib_a[curr_sol[i]]<<endl;
							cout<<"spazio i B: "<<sp_lib_b[curr_sol[i]]<<endl;
							cout<<"spazio j A: "<<sp_lib_a[curr_sol[j]]<<endl;
							cout<<"spazio j B: "<<sp_lib_b[curr_sol[j]]<<endl;
							exit(1);
						}

						//effettuo lo scambio
						curr_sol[i] = curr_sol[j];
						curr_sol[j] = bin_piu_vuoto;						
						return true;
					}
				}
			}
		}
	}
	return false;
}

// Restituisce l'indice del bin più vuoto ma ancora aperto
int getPiuVuotoIndex(){
	int maxsum=-1, max=-1;
	for(int i=0; i<max_bins; i++)
	{
		if(sp_lib_a[i]/bin_capacity_a + sp_lib_b[i]/bin_capacity_b > maxsum && !closedBins[i])
		{
			max = i;
			maxsum = sp_lib_a[i]/bin_capacity_a + sp_lib_b[i]/bin_capacity_b;
		}
	}
	return max;
}

// Stampa la soluzione corrente
void stampaSol(){
	for(int i=0; i<n; i++)
		cout << i <<":"<< curr_sol[i] << " ";
	cout << endl;
}
// Stampa la soluzione migliore trovata fino al momento della chiamata
void stampaBestSol(){
	for(int i=0; i<n; i++)
		cout << best_sol[i] << " ";
	cout << endl;
}

// Stampa le configurazioni di spazio libero al momento della chiamata
void stampaSpaziLiberi(){
	for(int i=0; i<max_bins; i++)
	{
		cout<<"BIN "<<i<<" LIBERO_A: "<<sp_lib_a[i]<<endl;
		cout<<"BIN "<<i<<" LIBERO_B: "<<sp_lib_b[i]<<endl;
	}
}

// Conta i bin aperti al momento della chiamata
int contaBinsAperti(){
	int count=max_bins;
	for(int i=0; i<max_bins; i++)
	{
		if(sp_lib_a[i]<bin_capacity_a || sp_lib_b[i]<bin_capacity_b)
		{
			count--;
		}
	}
	return count;
}

// Aggiornamento forzato dell'array closedBins in base allo spazio libero in ogni bin
void checkClosedBins(){
	bins_aperti=max_bins;
	for(int i=0; i<max_bins; i++)
	{
		if(sp_lib_a[i]==bin_capacity_a && sp_lib_b[i]==bin_capacity_b)
		{
			closedBins[i]=1;
			bins_aperti--;
		}
	}
}

bool applica_strategia_scambio(int istanza[][3], int i, int j){
	if(last_scambio[0] == i && last_scambio[1] == j)
		return false;
	switch(strategia_scambio)
	{
		// ---SOMMA
		// Cerco di riempire al massimo i bin che attualmente sono pieni scambiando i relativi oggetti con
		// quelli del bin più vuoto. A ogni fase di spostamento cerco di spostare tutto
		case 1:
			if(sp_lib_a[curr_sol[j]]+sp_lib_b[curr_sol[j]] >
				sp_lib_a[curr_sol[j]]+sp_lib_b[curr_sol[j]]+istanza[j][1]+istanza[j][2]-istanza[i][1]-istanza[i][2] &&
				// se lo scambio è nei limiti della fattibilità
				sp_lib_a[curr_sol[i]]+istanza[i][1]-istanza[j][1] > 0 && sp_lib_a[curr_sol[i]]+istanza[i][1]-istanza[j][1] < bin_capacity_a &&
				sp_lib_b[curr_sol[i]]+istanza[i][2]-istanza[j][2] > 0 && sp_lib_b[curr_sol[i]]+istanza[i][2]-istanza[j][2] < bin_capacity_b &&
				sp_lib_a[curr_sol[j]]+istanza[j][1]-istanza[i][1] > 0 && sp_lib_a[curr_sol[j]]+istanza[j][1]-istanza[i][1] < bin_capacity_a &&
				sp_lib_b[curr_sol[j]]+istanza[j][2]-istanza[i][2] > 0 && sp_lib_b[curr_sol[j]]+istanza[j][2]-istanza[i][2] < bin_capacity_b &&
				// e il bin con cui voglio scambiare non è già chiuso
				!closedBins[curr_sol[j]]
			)
				return true;
			break;
		// ---SOMMA
		// Cerco di svuotare il più possibile il bin più vuoto in modo da tentare alla prossima fase di spostamento
		// di portare il rimasuglio di questo in qualche altro bin
		case 2:
			// la somma degli spazi è ottimizzata dallo scambio che sto valutando
			double dim_obj_i;
			double dim_obj_j;
			dim_obj_i = (double)istanza[i][1]/bin_capacity_a + (double)istanza[i][2]/bin_capacity_b;
			dim_obj_j = (double)istanza[j][1]/bin_capacity_a + (double)istanza[j][2]/bin_capacity_b;
			if( (dim_obj_i > dim_obj_j) &&
				// se lo scambio è nei limiti della fattibilità
				sp_lib_a[curr_sol[i]]+istanza[i][1]-istanza[j][1] >= 0 && sp_lib_a[curr_sol[i]]+istanza[i][1]-istanza[j][1] <= bin_capacity_a &&
				sp_lib_b[curr_sol[i]]+istanza[i][2]-istanza[j][2] >= 0 && sp_lib_b[curr_sol[i]]+istanza[i][2]-istanza[j][2] <= bin_capacity_b &&
				sp_lib_a[curr_sol[j]]+istanza[j][1]-istanza[i][1] >= 0 && sp_lib_a[curr_sol[j]]+istanza[j][1]-istanza[i][1] <= bin_capacity_a &&
				sp_lib_b[curr_sol[j]]+istanza[j][2]-istanza[i][2] >= 0 && sp_lib_b[curr_sol[j]]+istanza[j][2]-istanza[i][2] <= bin_capacity_b
			){
				last_scambio[0] = i;
				last_scambio[1] = j;
				return true;
			}
			break;
		// ---MEDIA
		// Cerco di svuotare il più possibile il bin più vuoto in modo da tentare alla prossima fase di spostamento
		// di portare il rimasuglio di questo in qualche altro bin non completamente pieno
		case 3:
			if((((istanza[i][1]+istanza[i][2])/2) > ((istanza[j][1]+istanza[j][2])/2)) &&
				// se lo scambio è nei limiti della fattibilità
				sp_lib_a[curr_sol[i]]+istanza[i][1]-istanza[j][1] > 0 && sp_lib_a[curr_sol[i]]+istanza[i][1]-istanza[j][1] < bin_capacity_a &&
				sp_lib_b[curr_sol[i]]+istanza[i][2]-istanza[j][2] > 0 && sp_lib_b[curr_sol[i]]+istanza[i][2]-istanza[j][2] < bin_capacity_b &&
				sp_lib_a[curr_sol[j]]+istanza[j][1]-istanza[i][1] > 0 && sp_lib_a[curr_sol[j]]+istanza[j][1]-istanza[i][1] < bin_capacity_a &&
				sp_lib_b[curr_sol[j]]+istanza[j][2]-istanza[i][2] > 0 && sp_lib_b[curr_sol[j]]+istanza[j][2]-istanza[i][2] < bin_capacity_b
			){
				return true;
			}
			break;
		default:
			cout<<"Strategia non implementata"<<endl;
			exit(1);
			break;
	}
	return false;
}

// Controllo eventuali difetti per ammissibilità, coerenza assegnamento e relativi valori di spazio vuoto
bool checkSolutionValues(int istanza[][3]){
	int sum_a;
	int sum_b;
	for(int bin=0; bin<max_bins; bin++)
	{
		sum_a = 0;
		sum_b = 0;
		// Per ogni oggetto
		for(int i=0; i<n; i++)
		{
			if(curr_sol[i]==bin)
			{
				sum_a += istanza[i][1];
				sum_b += istanza[i][2];
			}
		}
		// Se i valori di spazio libero non combaciano con gli oggetti che ci ho messo dentro
		if(bin_capacity_a-sum_a != sp_lib_a[bin] || bin_capacity_b-sum_b != sp_lib_b[bin]){
			//cout<<"DIMENSIONI NON COMBACIANO: "<< endl;
			return false;
		}

		if(sum_a > bin_capacity_a || sum_a > bin_capacity_a){
			//cout<<"SFORATO "<< endl;
			return false;
		}

		// Se qualche bin sfora la capienza massima con valori maggiori della capienza o minori di 0
		if(sp_lib_a[bin] > bin_capacity_a || sp_lib_a[bin] < 0 || sp_lib_b[bin] > bin_capacity_b || sp_lib_b[bin] < 0)
		{
		//	cout << "ERR" << endl;
			/*
			cout<<"BIN: "<<bin<<endl;
			cout<<"BC:"<<bin_capacity_a<<endl;
			cout<<"sum_a2:"<<sum_a<<endl;
			cout<<bin_capacity_a-sum_a<<endl;
			cout<<sp_lib_a[bin]<<endl;
			cout<<"sum_b:"<<sum_b<<endl;
			cout<<bin_capacity_b-sum_b<<endl;
			cout<<sp_lib_b[bin]<<endl;
			cout<<sp_lib_a[bin]<<" > "<<bin_capacity_a<<endl;;
			cout<<sp_lib_a[bin]<<" < "<<0<<endl;
			cout<<sp_lib_b[bin]<<" > "<<bin_capacity_b<<endl;
			cout<<sp_lib_b[bin]<<" < "<<0<<endl;
			stampaSol();
			stampaSpaziLiberi();
			cout<<"Errore nelle soluzioni"<<endl;*/
			return false;
		}
		return true;
	}
}

// Restituisce l'indice del bin che secondo la strategia maxFillMean è il candidato per lo spostamento
int get_best_bin_maxFillMean(int istanza[][3], int i){
	int min = -1;
	int mean = 0;
	for(int bin=0; bin<max_bins; bin++)
	{
		// Se l'oggetto ci sta...
		if(istanza[i][1]<=sp_lib_a[bin] && istanza[i][2]<=sp_lib_b[bin] &&
		// ...rispetta il best fill mean...
			(double)istanza[i][1]/sp_lib_a[bin] + (double)istanza[i][2]/sp_lib_b[bin] > mean &&
		// ... il bin in cui lo voglio mettere è aperto...
			!closedBins[bin] &&
		// ... e non è il bin che sto cercando di svuotare...
			bin != curr_sol[i]
		){
			min=bin;
			mean = (double)istanza[i][1]/sp_lib_a[bin] + (double)istanza[i][2]/sp_lib_b[bin];
		}
	}
	if(min>max_bins) return -1;
	return min;
}

// Esegue mosse di spostamento e scambio al di fuori dell'ammissibilità, ritorna true
// solo se dopo esserne uscito, l'algoritmo rientra nell'ammissibilità
bool esci_ammissibilita_piu_vuoto(int istanza[][3]){
	inammissibilita_tot = 0;
	svuota_bin_piu_vuoto(istanza);
	bool mosso = true;
	bool mossa_fatta = false;
	while(mosso)
	{
		mosso=false;
		mosso = sposto_na(istanza);

		if(mosso){
			mossa_fatta = true;
			continue;
		}
		mosso = scambio_na(istanza);
		if(mosso) mossa_fatta = true;
	}
	if(inammissibilita_tot<=0 && mossa_fatta && checkSolutionValues(istanza)) return true;
	return false;
}

// Sposto tutti gli oggetti contenuti nel bin più vuoto in altri tale che sia minima la fuoriuscita globale dall'ammissibilità
void svuota_bin_piu_vuoto(int istanza[][3]){
	double temp_max;
	int bin_max=-1;
	double max;
	bin_piu_vuoto = getPiuVuotoIndex();
	// i è l'oggetto corrente del bin più vuoto
	for(int i=0; i<n; i++){
		max = INT_MIN;
		if(curr_sol[i] == bin_piu_vuoto){
			// j sono tutti gli altri bin
			for(int j=0; j<max_bins; j++){
				if(j!=bin_piu_vuoto){
					temp_max = (double)(sp_lib_a[j]-istanza[i][1])/bin_capacity_a + (double)(sp_lib_b[j]-istanza[i][2])/bin_capacity_b;
					if(temp_max > max){
						bin_max = j;
						max = temp_max;
					}
				}
			}
			// ora abbiamo il bin che meglio ci tiene l'oggetto i-esimo
			curr_sol[i] = bin_max;
			sp_lib_a[bin_max] -= istanza[i][1];
			sp_lib_b[bin_max] -= istanza[i][2];
			sp_lib_a[bin_piu_vuoto] += istanza[i][1];
			sp_lib_b[bin_piu_vuoto] += istanza[i][2];
			
			// registro nell'inammissibilità totale di quanto sono uscito
			if(sp_lib_a[bin_max]<0) inammissibilita_tot -= sp_lib_a[bin_max]/bin_capacity_a;
			if(sp_lib_b[bin_max]<0) inammissibilita_tot -= sp_lib_b[bin_max]/bin_capacity_b;
		}
	}
	closedBins[bin_piu_vuoto] = 1;
}

// Effettua lo spostamento di un oggetto dal bin più vuoto al di fuori dell'ammissibilità
bool sposto_na(int istanza[][3]){
	int maxGuadagno = INT_MIN;
	int inamm_globale_iniziale, inamm_globale_finale;
	int min_bin_index=-1;
	int min_obj_index=-1;
	int inamm_1=0, inamm_2=0;
	// per ogni oggetto i
	for(int i=0; i<n; i++)
	{
		// se il bin in cui sta correntemente i è al difuori dell'ammissibilità
		if(sp_lib_a[curr_sol[i]]<0 || sp_lib_b[curr_sol[i]]<0){
			// per ogni altro bin j
			for(int j=0; j<max_bins; j++)
			{
				if(j!=curr_sol[i]){
					// inamm_1 è l'inammissibilità corrente del bin che contiene l'oggetto i...
					inamm_1 = sp_lib_a[curr_sol[i]]/bin_capacity_a + sp_lib_b[curr_sol[i]]/bin_capacity_b;
					// inamm_2 è l'inammissibilità corrente del bin j-esimo...
					inamm_2 = sp_lib_a[j]/bin_capacity_a + sp_lib_b[j]/bin_capacity_b;
					inamm_globale_iniziale = abs(inamm_1 + inamm_2);
					
					// ...ora vediamo quanto sarebbe togliendo l'oggetto i
					inamm_1 = inamm_1 + istanza[i][1]/bin_capacity_a + istanza[i][2]/bin_capacity_b;
					
					if(inamm_2 > 0){
						inamm_2 = 0;
					}
					inamm_2 = inamm_2 - istanza[i][1]/bin_capacity_a - istanza[i][2]/bin_capacity_b;
					if(inamm_2 > 0){
						inamm_2 = 0;
					}
					
					inamm_globale_finale = abs(inamm_1 + inamm_2);
					if(inamm_globale_finale < inamm_globale_iniziale){
						// Controlliamo se l'inammissibilità dei due bin scenderebbe di più dello scambio possibile trovato prima
						if(inamm_1+inamm_2 > maxGuadagno){
							maxGuadagno = inamm_1+inamm_2;
							// Questo diventa lo spostamento candidato dell'oggetto i nel bin j
							min_bin_index = j;
							min_obj_index = i;
						}
					}
				}
			}
		}
	}
	if(min_bin_index != -1){
		sp_lib_a[curr_sol[min_obj_index]] += istanza[min_obj_index][1];
		sp_lib_b[curr_sol[min_obj_index]] += istanza[min_obj_index][2];
		sp_lib_a[min_bin_index] -= istanza[min_obj_index][1];
		sp_lib_b[min_bin_index] -= istanza[min_obj_index][2];
		curr_sol[min_obj_index] = min_bin_index;
		inammissibilita_tot -= maxGuadagno;
		return true;
	}
	return false;
}

// Effettua lo scambio al di fuori dell'ammissibilità secondo la strategia scelta
bool scambio_na(int istanza[][3]){
	int maxGuadagno = INT_MIN;
	int inamm_globale_iniziale, inamm_globale_finale;
	int min_obj1_index=-1;
	int min_obj2_index=-1;
	int inamm_1=0, inamm_2=0;
	// per ogni oggetto i
	for(int i=0; i<n; i++)
	{
		// se il bin in cui sta correntemente i è al difuori dell'ammissibilità
		if(sp_lib_a[curr_sol[i]]<0 || sp_lib_b[curr_sol[i]]<0){
			// per ogni altro oggetto j
			for(int j=0; j<n; j++)
			{
				// Se i e j non stanno nello dtesso bin
				if(curr_sol[j] != curr_sol[i]){
					// inamm_1 è l'inammissibilità corrente del bin che contiene l'oggetto i...
					inamm_1 = sp_lib_a[curr_sol[i]]/bin_capacity_a + sp_lib_b[curr_sol[i]]/bin_capacity_b;
					// inamm_2 è l'inammissibilità corrente del bin j-esimo...
					inamm_2 = sp_lib_a[curr_sol[j]]/bin_capacity_a + sp_lib_b[curr_sol[j]]/bin_capacity_b;
					inamm_globale_iniziale = abs(inamm_1 + inamm_2);
					
					// ...ora vediamo quanto sarebbe togliendo l'oggetto i e mettendoci j
					inamm_1 = inamm_1 + istanza[i][1]/bin_capacity_a + istanza[i][2]/bin_capacity_b -
								istanza[j][1]/bin_capacity_a - istanza[j][2]/bin_capacity_b;
					if(inamm_1 > 0){
						inamm_1 = 0;
					}
					
					// Se il bin è già nell'ammissibilità allora setto la variabile di inammissibilità a 0
					if(inamm_2 > 0){
						inamm_2 = 0;
					}
					inamm_2 = inamm_2 - istanza[i][1]/bin_capacity_a - istanza[i][2]/bin_capacity_b +
								istanza[j][1]/bin_capacity_a + istanza[j][2]/bin_capacity_b;
					// Se scambiando riesco a far rientrare questo bin nell'ammissibilità allora azzero l'inammissibilità
					if(inamm_2 > 0){
						inamm_2 = 0;
					}
					
					inamm_globale_finale = abs(inamm_1 + inamm_2);
					if(inamm_globale_finale < inamm_globale_iniziale){
						// Controlliamo se l'inammissibilità dei due bin scenderebbe di più dello scambio possibile trovato prima
						if(inamm_1+inamm_2 > maxGuadagno){
							maxGuadagno = inamm_1+inamm_2;
							// Questo diventa lo spostamento candidato dell'oggetto i nel bin j
							min_obj1_index = i;
							min_obj2_index = j;
						}
					}
				}
			}
		}
	}
	if(min_obj1_index != -1){
		// Aggiorno lo spazio libero del bin che conteneva obj1
		sp_lib_a[curr_sol[min_obj1_index]] = sp_lib_a[curr_sol[min_obj1_index]] + istanza[min_obj1_index][1] - istanza[min_obj2_index][1];
		sp_lib_b[curr_sol[min_obj1_index]] = sp_lib_b[curr_sol[min_obj1_index]] + istanza[min_obj1_index][2] - istanza[min_obj2_index][2];

		// Aggiorno lo spazio libero del bin che conteneva obj2
		sp_lib_a[curr_sol[min_obj2_index]] = sp_lib_a[curr_sol[min_obj2_index]] + istanza[min_obj2_index][1] - istanza[min_obj1_index][1];
		sp_lib_b[curr_sol[min_obj2_index]] = sp_lib_b[curr_sol[min_obj2_index]] + istanza[min_obj2_index][2] - istanza[min_obj1_index][2];
		
		int temp = curr_sol[min_obj1_index];
		curr_sol[min_obj1_index] = curr_sol[min_obj2_index];
		curr_sol[min_obj2_index] = temp;
		inammissibilita_tot -= maxGuadagno;
		return true;
	}
	return false;
}
