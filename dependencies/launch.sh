#!/bin/bash

echo "[1]Build..."

if ! [ -d bin ] ; then
  mkdir bin
fi

if ! [ -d data ] ; then
  mkdir data
fi

gcc -Werror -pthread 1.c -lm -o ./bin/first
gcc -Werror -pthread parallel-1.c -lm -o ./bin/parallel-first

#Compare scalar and parallel versions
echo "[2]Comparison test..."
./bin/first 100 1
./bin/parallel-first 4 100 1
difference=`diff data/1.txt data/par-1.txt`
echo "The difference (must be empty): "$difference


N=10000
echo "[3]Acceleration test for N ="$N"..."
(time bin/first $N 0) &> time_buf
time_ref=`grep -E user time_buf | grep -E -oh "[0-9]*\.[0-9]*"`

pow=2
while [ $pow -lt 8 ]
do
  (time bin/parallel-first $pow $N 0) &> time_buf
  time=`grep -E user time_buf | grep -E -oh "[0-9]*\.[0-9]*"`
  acc=`bc <<< 'scale=3; '$time'/'$time_ref`
  echo $pow" "$acc >> time_data
  pow=$((pow*2))
done
echo "n, accel"
cat time_data
rm time_buf
mv time_data data/
