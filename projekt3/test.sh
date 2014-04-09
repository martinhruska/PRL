#!/bin/bash
if [ $# -lt 1 ];then
    numbers=10;
else
    numbers=$1;
fi;
mpic++ --prefix /usr/local/share/OpenMPI -o mm mm.cpp
mpirun --prefix /usr/local/share/OpenMPI -np `expr $numbers + 1` mm
rm -f mm
