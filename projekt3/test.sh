#!/bin/bash
if [ $# -lt 1 ];then
    numbers=10;
else
    numbers=$1;
fi;
bases=`echo "l($numbers)/l(2)" | bc -l`
basesInt=${bases%.*}
procs=`echo "2^$basesInt" | bc -l`
if [[ $procs -ne $numbers ]]
then
    procs=`echo "2^(1+$basesInt)" | bc -l`
fi
mpic++ --prefix /usr/local/share/OpenMPI -o mm mm.cpp
mpirun --prefix /usr/local/share/OpenMPI -np `expr $procs + 1` mm
rm -f mm
