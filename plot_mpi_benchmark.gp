set terminal png size 800,600
set output 'mpi_benchmark_results.png'
set title 'MPI Matrix-Vector Multiplication Performance'
set xlabel 'Matrix Size (N)'
set ylabel 'Performance (MFLOPS)'
set grid
set key top left
plot 'mpi_results.txt' using 1:2 with linespoints title 'MFLOPS (4 processes)', \
     'mpi_results.txt' using 1:2 smooth bezier with lines title 'Trend' lw 2
