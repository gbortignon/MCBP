	#!/bin/bash

	#RICOMPILA I FILE
	g++ -o dat formatta_istanze_dat.cpp 
	

	for file in ../../istanze/*
	do
		./dat $file
	done
