#! /bin/bash
for i in $(seq 1 $1)
do
    ./test.sh $((10*$i)) 2>/dev/null > res.txt
    ./check_res.py res.txt
done 
