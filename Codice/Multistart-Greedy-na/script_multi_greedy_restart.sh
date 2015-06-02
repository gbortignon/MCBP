	#!/bin/bash
	rm Benchmark_Multi-Greedy.txt
	rm multi-greedy-restart
	rm Times.txt

	#RICOMPILA I FILE
	g++ -o multi-greedy-restart main_multi_greedy-restart.cpp Multi_greedy-restart-na.cpp Greedy.cpp

	for file in ../../istanze/*
	do

		#GESTIONE MULTI GREEDY RESTART
		echo $file
		echo -n $file >> Benchmark_Multi-Greedy.txt
		echo -n ";" >> Benchmark_Multi-Greedy.txt
		./multi-greedy-restart $file 3 8000 2 3
		
	done
