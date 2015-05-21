#!/bin/bash
rm Benchmark_Greedy.txt
rm Times_Greedy.txt

echo ";MF;Mean;Sub;Weighted;MF;Mean;Sub;Weighted;" >> Benchmark_Greedy.txt

#RICOMPILA I FILE
g++ -o greedy main_greedy.cpp Greedy.cpp

for file in ../../istanze/*
do
	echo $file;

	#ESECUZIONE TUTTI I GREEDY
	echo -n "Istanza: " $file ";" >> Benchmark_Greedy.txt

	./greedy $file greedy 1 2
	./greedy $file greedy 2 2
	./greedy $file greedy 3 2
	./greedy $file greedy 4 2

	./greedy $file greedy 1 3
	./greedy $file greedy 2 3
	./greedy $file greedy 3 3
	./greedy $file greedy 4 3

	#ESECUZIONE BEST GREEDY
	# param1: funzione di comparazione
	# param2: strategia
	#./greedy $file greedy 2 2
done
