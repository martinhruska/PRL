set xlabel "Pocet vstupu"
set ylabel "Cas [s]"
set terminal png
set output "perf.png"
set autoscale
set grid
plot "perf.data" using 1:2 with linespoints title "Realny cas" , \
"perf.data" using 1:(4*$1/10000 - 0.001) with linespoints title "0.0004*n-0.001"
