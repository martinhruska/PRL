#!/bin/bash
if [ $# -lt 1 ];then
    numbers=10;
else
    numbers=$1;
fi;
mpic++ --prefix /usr/local/share/OpenMPI -o es es.cpp
dd if=/dev/random bs=1 count=$numbers of=numbers
mpirun --prefix /usr/local/share/OpenMPI -np `expr $numbers + 1` es
#rm -f es numbers
