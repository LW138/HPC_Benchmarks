set terminal png size 800,600
set output 'benchmark_results.png'
set title 'Matrix-Vector Multiplication Performance'
set xlabel 'Matrix Size (N)'
set ylabel 'Performance (MFLOPS)'
set grid
set key top left
plot 'benchmark_results.txt' using 1:2 with linespoints title 'MFLOPS', \
     'benchmark_results.txt' using 1:2 smooth bezier with lines title 'Trend' lw 2
