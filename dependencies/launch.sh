#!/bin/bash

echo "[1]Build..."

if ! [ -d bin ] ; then
  mkdir bin
fi

if ! [ -d data ] ; then
  mkdir data
fi

#pthread build
for I in 1 4
do
  gcc -Werror ./simple/$I.c -lm -o ./bin/$I.out
  gcc -Werror -pthread ./parallel/parallel-$I.c -lm -o ./bin/parallel-$I.out
done

#OpenMP build
for I in 3 6
do
  gcc -Werror ./simple/$I.c -lm -o ./bin/$I.out
  gcc -Werror -fopenmp ./parallel/parallel-$I.c -lm -o ./bin/parallel-$I.out
done

#Compare simple and parallel versions
echo "[2]Comparison tests..."

for I in 1 3 4 6
do
  ./bin/$I.out 100 1 &> /dev/null
  ./bin/parallel-$I.out 4 100 1 &> /dev/null
  mv $I.txt par-$I.txt data
  difference=`diff data/$I.txt data/par-$I.txt`
  echo "The difference for $I.c (must be empty): "$difference
done

N=10000
echo "[3]Acceleration test for N ="$N"..."
for I in 1 3 4 6
do
  time_ref=`bin/$I.out $N 0`
  pow=2
  while [ $pow -lt 8 ]
  do
    exec_time=0
    count=0
    avg=2
    while [ $count -lt $avg ]; do
      var=`bin/parallel-$I.out $pow $N 0`
      exec_time=`bc <<< 'scale=3; '$exec_time'+'$var`
      count=$((count+1))
    done
    acc=`bc <<< 'scale=3; '$time_ref'*'$avg'/'$exec_time`
    echo $pow" "$acc >> acceleration
    pow=$((pow*2))
  done
  echo -e "\nfor $I.c:\nn, accel"
  cat acceleration

gnuplot <<- EOF
    set xlabel "np"
    set ylabel "acceleration"
    set term png
    set output "acceleration_${I}.png"
    plot "acceleration" with lines title "${I}.c"
    exit
EOF

  mv acceleration data/acceleration_$I
  echo -e "\n"
done
mv *.png data/
