	#!/bin/bash
	#rm Benchmark_Greedy.txt
	rm Benchmark_Multi.txt
	#rm Benchmark_Multi_na.txt
	#rm Benchmark_Multi-greedy.txt

	#echo ";MaxFill;Minimo;Max/Min;MaxFill/Minimo;Massimo;MaxFillMean;MaxFillMinDim;MF;Mean;Sub;Weighted;\n" >> Benchmark_Greedy.txt

	echo ";80%/1-10;80%/2-10;80%/3-10;80%/1-100;80%/2-100;80%/3-100;80%/1-500;80%/2-500;80%/3-500;" >> Benchmark_Multi.txt

	#echo ";10;100;500;" >> Benchmark_Multi.txt

	#RICOMPILA I FILE
	#g++ -o greedy main.cpp Greedy.cpp 
	g++ main2.cpp Multi.cpp RngStream.cpp
	#g++ -o multi-greedy main2.cpp Multi-greedy.cpp Greedy.cpp
	#g++ -o multi_na main_na.cpp Multi_na.cpp RngStream.cpp Greedy.cpp

	for file in ../istanze/*
	do
		#GESTIONE GREEDY
		#echo "Istanza: " $file >> Benchmark_Greedy.txt
		#i=1
		#while((i<8))
		#do
		#	./greedy $file greedy $i 1
		#	((i++))
		#done
	
		#j=0
		#while((j<5))
		#do
		#	./greedy $file greedy $j 2
		#	((j++))
		#done
	
		# GESTIONE MULTISTART
		echo -n $file >> Benchmark_Multi.txt
		echo -n ";" >> Benchmark_Multi.txt
		./a.out $file multi 2 1 10
		echo -n ";" >> Benchmark_Multi.txt
		./a.out $file multi 2 2 10
		echo -n ";" >> Benchmark_Multi.txt
		./a.out $file multi 2 3 10
		echo -n ";" >> Benchmark_Multi.txt
		./a.out $file multi 2 1 100
		echo -n ";" >> Benchmark_Multi.txt
		./a.out $file multi 2 2 100
		echo -n ";" >> Benchmark_Multi.txt
		./a.out $file multi 2 3 100
		echo -n ";" >> Benchmark_Multi.txt
		./a.out $file multi 2 1 500
		echo -n ";" >> Benchmark_Multi.txt
		./a.out $file multi 2 2 500
		echo -n ";" >> Benchmark_Multi.txt
		./a.out $file multi 2 3 500
		echo >> Benchmark_Multi.txt

		#GESTIONE MULTISTART NA
# 		echo -n $file >> Benchmark_Multi_na.txt
# 		echo -n ";" >> Benchmark_Multi_na.txt
# 		./multi_na $file multi_na 2 2 10
# 		echo -n ";" >> Benchmark_Multi_na.txt
# 		./multi_na $file multi_na 2 2 100
# 		echo -n ";" >> Benchmark_Multi_na.txt
# 		./multi_na $file multi_na 2 2 500
# 		echo >> Benchmark_Multi_na.txt

		#GESTIONE MULTISTART-GREEDY
		# echo -n $file >> Benchmark_Multi-Greedy.txt
		# echo -n ";" >> Benchmark_Multi-Greedy.txt
		# ./multi-greedy $file multi 1 1
		# echo -n ";" >> Benchmark_Multi-Greedy.txt
		# ./multi-greedy $file multi 1 2
		# echo -n ";" >> Benchmark_Multi-Greedy.txt
		# ./multi-greedy $file multi 1 3
		# echo -n ";" >> Benchmark_Multi-Greedy.txt
		# ./multi-greedy $file multi 2 1
		# echo -n ";" >> Benchmark_Multi-Greedy.txt
		# ./multi-greedy $file multi 2 2
		# echo -n ";" >> Benchmark_Multi-Greedy.txt
		# ./multi-greedy $file multi 2 3
		# echo >> Benchmark_Multi-Greedy.txt
	done
