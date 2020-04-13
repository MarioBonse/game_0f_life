#!/bin/bash
rm par.txt parfor.txt seq.txt
g++ -o parfor game_of_life_parallelfor.cpp -O3 -fopenmp
g++ -o par game_of_life_thread.cpp -O3 -pthread
g++ -o seq game_of_life_seq.cpp -O3
echo $1 $2 $3 $4 $5
for ((i = 0 ; i < $5 ; i++)); 
do
   echo $1 $2 $3 $i $4
   ./par $1 $2 $3 $i $4 >> par.txt
   ./parfor $1 $2 $3 $i $4 >> parfor.txt
   ./seq $1 $2 $3 $i >> seq.txt
done
python3 mean_std.py