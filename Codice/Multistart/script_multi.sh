	#!/bin/bash
	rm Benchmark_Multi.txt
	rm Times_Multi.txt
	
	echo ";80%/2-5000;80%/2-10000;" >> Benchmark_Multi.txt
	echo ";" >> Benchmark_Multi.txt

	#RICOMPILA I FILE
	g++ main_multi.cpp Multi.cpp RngStream.cpp

	for file in ../istanze/*
	do
		echo $file;

		# GESTIONE MULTISTART
		echo -n ";" >> Benchmark_Multi.txt
		# param1: strategia generazione
		# param2: strategia scambio
		# param3: numero restart
		./a.out $file multi 2 2 5000
		echo >> Benchmark_Multi.txt

	done
