	#!/bin/bash
	rm Benchmark_Multi-Greedy.txt
	rm multi-greedy-restart

	#RICOMPILA I FILE
	g++ -o multi-greedy-restart main_multi_greedy-restart.cpp Multi_greedy-restart.cpp Greedy.cpp

	echo ";Classic;Structured;Pro;" >> Benchmark_Multi-Greedy.txt

	for file in ../istanze/*
	do
		#ESECUZIONE MULTISTART GREEDY RESTART
		echo $file
		echo -n $file >> Benchmark_Multi-Greedy.txt
		echo -n ";" >> Benchmark_Multi-Greedy.txt
		# param1: strategia scambio
		# param2: numero restart
		./multi-greedy-restart $file 3 10000 2 3
		echo >> Benchmark_Multi-Greedy.txt
	done
