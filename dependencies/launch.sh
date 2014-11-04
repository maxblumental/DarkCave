#!/bin/bash

echo "[1]Build..."

if ! [ -d bin ] ; then
  mkdir bin
fi

if ! [ -d data ] ; then
  mkdir data
fi

for I in 1 4
do
  gcc -Werror $I.c -lm -o ./bin/$I.out
  gcc -Werror -pthread parallel-$I.c -lm -o ./bin/parallel-$I.out
done

#Compare scalar and parallel versions
echo "[2]Comparison tests..."

for I in 1 4
do
  ./bin/$I.out 100 1 &> /dev/null
  ./bin/parallel-$I.out 4 100 1 &> /dev/null
  mv $I.txt par-$I.txt data
  difference=`diff data/$I.txt data/par-$I.txt`
  echo "The difference for $I.c (must be empty): "$difference
done

N=10000
echo "[3]Acceleration test for N ="$N"..."
for I in 1 4
do
  time_ref=`bin/$I.out $N 0`
  pow=2
  while [ $pow -lt 8 ]
  do
    time=`bin/parallel-$I.out $pow $N 0`
    acc=`bc <<< 'scale=3; '$time_ref'/'$time`
    echo $pow" "$acc >> time_data
    pow=$((pow*2))
  done
  echo -e "\nfor $I.c:\nn, accel"
  cat time_data
  mv time_data data/time_data_$I
  echo -e "\n"
done
