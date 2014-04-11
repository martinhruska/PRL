for i in $(seq 1 $1)
do
    python myTest.py $i > py_res
    bash test.sh $i > temp_res
    python parseOut.py temp_res > my_res
    res=`diff my_res py_res`
    if [ $res == ""]
        then
            echo "TEST $i [OK]"
        else
            echo "TEST $i [FAIL]"
            cat operands
            echo ""
            echo "RESULTS:"
            cat py_res
            cat my_res
            echo "---"
    fi
done
rm py_res
rm my_res
rm temp_res
rm operands
