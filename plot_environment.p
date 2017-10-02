# Gnuplot script for visualization of environment

reset

unset label
set title "food distribution in environment"

set key off
set view map
set xrange [0:]
set yrange [0:]
set palette defined (0 "white", 1 "blue")

count = 0

set term x11 0 size 900, 800
plot "foodReserve0.txt" matrix with image
#     "population.txt" with points linecolor rgb "red"

set term x11 1 size 900, 800
plot "population.txt" using 1:2 with lines linecolor rgb "red", \
     "population.txt" using 1:2:(column(0)*5) every 5 with labels  offset 1,1

set term x11 2 size 900, 800
plot "foodReserve1.txt" matrix with image, \
     "sensations.txt" with points linecolor rgb "green"

