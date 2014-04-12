#!/bin/bash

for i in $(seq 1 $1)
do
    python myTest.py $i > py_res
    procs=$(($i+1))
    bash test.sh $procs > temp_res
    python parseOut.py temp_res > my_res
    res=`diff my_res py_res`
    if [[ $res == "" ]]
        then
            echo "TEST $i [OK]"
        else
            echo "TEST $i [FAIL]"
            cat operands
            echo ""
            echo "RESULTS:"
            cat py_res
            echo "----"
            cat my_res
            echo "---"
    fi
done
rm py_res
rm my_res
rm temp_res
rm operands
