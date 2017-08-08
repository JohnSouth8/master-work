# Gnuplot script for visualization of environment

unset label
set title "food distribution in environment"


set view map
set palette defined (0 "white", 1 "blue")
plot "foodReserve.txt" matrix with image
