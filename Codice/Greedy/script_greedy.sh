	#!/bin/bash
	rm Benchmark_Greedy.txt
	rm Times_Greedy.txt

	echo ";MF;Mean;Sub;Weighted;MF;Mean;Sub;Weighted;" >> Benchmark_Greedy.txt

	#RICOMPILA I FILE
	g++ -o greedy main_greedy.cpp Greedy.cpp

	for file in ../istanze/*
	do
		echo $file;

		#ESECUZIONE TUTTI I GREEDY
 		echo -n "Istanza: " $file ";" >> Benchmark_Greedy.txt
		
 		j=1
 		while((j<5))
 		do
 			./greedy $file greedy $j 2
 			((j++))
 		done

 		j=1
 		while((j<5))
 		do
 			./greedy $file greedy $j 3
 			((j++))
 		done

		#ESECUZIONE BEST GREEDY
		# param1: funzione di comparazione
		# param2: strategia
		#./greedy $file greedy 2 2
	done
