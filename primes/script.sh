#!/bin/bash

# script for evalating 
# usage 
# starting ending nthread number_test
rm seq.txt parfor.txt farm.txt
make
echo $1 $2 $3 $4
for ((i = 0 ; i < $4 ; i++)); 
do

   ./primes_factory $1 $2 off $3 >> farm.txt
   ./primes_par_for $1 $2 off $3 >> parfor.txt
   ./primes $1 $2 off $3 >> seq.txt
done