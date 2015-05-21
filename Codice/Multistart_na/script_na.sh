	#!/bin/bash
	rm Benchmark_Multi_na.txt
	rm Times_Multi_na.txt
	rm multi_na

	#RICOMPILA I FILE
	g++ -o multi_na main_na.cpp Multi_na.cpp RngStream.cpp

	for file in ../istanze/*
	do

		#GESTIONE MULTISTART NA
		echo -n $file >> Benchmark_Multi_na.txt
		echo -n ";" >> Benchmark_Multi_na.txt
		# param2: strategia scambio
		# param3: numero restart
		./multi_na $file 2 10000
		echo >> Benchmark_Multi_na.txt

	done
