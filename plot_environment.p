# Gnuplot script for visualization of environment

unset label
set title "food distribution in environment"

set key off
set view map
set xrange [0:]
set yrange [0:]
set palette defined (0 "white", 1 "blue")
plot "foodReserve.txt" matrix with image, \
     "sensations.txt" with points linecolor rgb "green", \
     "population.txt" with points linecolor rgb "red"

