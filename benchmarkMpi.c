#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <mpi.h>

#define MAX_SIZE 9000
#define RUNS 10

int main(int argc, char *argv[]) {
    int commRank, commSize;

    // Initialize MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &commRank);
    MPI_Comm_size(MPI_COMM_WORLD, &commSize);

    if (commRank == 0) {
        printf("Running MPI Matrix-Vector Multiplication Benchmark\n");
        printf("Number of processes: %d\n", commSize);
        printf("--------------------------------------------\n");
        printf("Size\tMFLOPS\tTime(s)\n");

        // Create gnuplot script file
        FILE *plotFile = fopen("plot_mpi_benchmark.gp", "w");
        if (plotFile == NULL) {
            printf("Error creating plot file!\n");
        } else {
            fprintf(plotFile, "set terminal png size 800,600\n");
            fprintf(plotFile, "set output 'mpi_benchmark_results.png'\n");
            fprintf(plotFile, "set title 'MPI Matrix-Vector Multiplication Performance'\n");
            fprintf(plotFile, "set xlabel 'Matrix Size (N)'\n");
            fprintf(plotFile, "set ylabel 'Performance (MFLOPS)'\n");
            fprintf(plotFile, "set grid\n");
            fprintf(plotFile, "set key top left\n");
            fprintf(plotFile, "plot 'mpi_results.txt' using 1:2 with linespoints title 'MFLOPS (%d processes)', \\\n", commSize);
            fprintf(plotFile, "     'mpi_results.txt' using 1:2 smooth bezier with lines title 'Trend' lw 2\n");
            fclose(plotFile);
        }
    }

    // Open results file (only root process will write to it)
    FILE *resultsFile = NULL;
    if (commRank == 0) {
        resultsFile = fopen("mpi_results.txt", "w");
        if (resultsFile == NULL) {
            printf("Error opening results file!\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        fprintf(resultsFile, "# Size MFLOPS Time(s)\n");
    }

    // Step size based on number of processes
    int step = commSize * 10;

    // Test different matrix sizes
    for (int N = commSize * 10; N <= MAX_SIZE; N += step) {
        // Increase step size for larger matrices to reduce benchmark time
        if (N >= 1000) step = commSize * 50;
        if (N >= 3000) step = commSize * 100;
        if (N >= 6000) step = commSize * 200;

        // Ensure N is divisible by commSize for even distribution
        if (N % commSize != 0) {
            continue;
        }

        // Calculate chunk size for each process
        int rowsPerProcess = N / commSize;
        int chunkSize = rowsPerProcess * N;

        // Allocate memory for matrix parts and vectors
        double *matrixPart = (double *)malloc(chunkSize * sizeof(double));
        double *vector = (double *)malloc(N * sizeof(double));
        double *resultPart = (double *)malloc(rowsPerProcess * sizeof(double));

        // Root process allocates additional memory
        double *matrix = NULL;
        double *gatheredResult = NULL;
        if (commRank == 0) {
            matrix = (double *)malloc(N * N * sizeof(double));
            gatheredResult = (double *)malloc(N * sizeof(double));

            // Initialize matrix (identity matrix for testing)
            for (int i = 0; i < N; i++) {
                for (int j = 0; j < N; j++) {
                    matrix[i * N + j] = (i == j) ? 1.0 : 0.0;
                }
            }

            // Initialize vector
            for (int i = 0; i < N; i++) {
                vector[i] = i;
            }
        }

        // Scatter matrix parts to all processes
        MPI_Scatter(matrix, chunkSize, MPI_DOUBLE,
                    matrixPart, chunkSize, MPI_DOUBLE,
                    0, MPI_COMM_WORLD);

        // Broadcast vector to all processes
        MPI_Bcast(vector, N, MPI_DOUBLE, 0, MPI_COMM_WORLD);

        // Synchronize before starting timing
        MPI_Barrier(MPI_COMM_WORLD);

        // Start timing
        double startTime, endTime;
        startTime = MPI_Wtime();

        // Run the benchmark
        for (int run = 0; run < RUNS; run++) {
            // Calculate matrix_part * vector
            for (int i = 0; i < rowsPerProcess; i++) {
                double sum = 0.0;
                for (int j = 0; j < N; j++) {
                    sum += matrixPart[i * N + j] * vector[j];
                }
                resultPart[i] = sum;
            }
        }

        // End timing
        endTime = MPI_Wtime();
        double elapsedTime = endTime - startTime;

        // Gather results from all processes
        MPI_Gather(resultPart, rowsPerProcess, MPI_DOUBLE,
                   gatheredResult, rowsPerProcess, MPI_DOUBLE,
                   0, MPI_COMM_WORLD);

        // Calculate and report performance on root process
        if (commRank == 0) {
            // Calculate MFLOPS (2*N*N operations per matrix-vector mult)
            double flops = 2.0 * N * N * RUNS;
            double mflops = (flops / elapsedTime) / 1.0e6;

            printf("%d\t%.2f\t%.4f\n", N, mflops, elapsedTime);
            fprintf(resultsFile, "%d %.2f %.4f\n", N, mflops, elapsedTime);
        }

        // Free memory
        free(matrixPart);
        free(vector);
        free(resultPart);

        if (commRank == 0) {
            free(matrix);
            free(gatheredResult);
        }
    }

    // Close results file and generate plot
    if (commRank == 0) {
        fclose(resultsFile);
        printf("\nGenerating performance plot...\n");
        system("gnuplot plot_mpi_benchmark.gp");
        printf("Plot saved as 'mpi_benchmark_results.png'\n");
    }

    MPI_Finalize();
    return 0;
}