#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>

#define MAX_SIZE 1000 // Maximum size of matrix/vector
#define RUNS 50000    // Number of iterations for each size

// Function to perform matrix-vector multiplication
void matrix_vector_multiply(double **A, double *v, double *result, int size) {
    // Reset result vector to zeros
    for (int i = 0; i < size; i++) {
        result[i] = 0.0;
    }

    // Perform the multiplication
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            result[i] += A[i][j] * v[j];
        }
    }
}

// Generate a vector with predictable values
double* generate_vector(int size) {
    double *vector = (double *)malloc(size * sizeof(double));

    for (int i = 0; i < size; i++) {
        vector[i] = i;
    }

    return vector;
}

// Generate a matrix for testing
double** generate_matrix(int size) {
    double **A = (double **)malloc(size * sizeof(double *));
    for (int i = 0; i < size; i++) {
        A[i] = (double *)malloc(size * sizeof(double));
        for (int j = 0; j < size; j++) {
            // Using identity matrix for consistent performance testing
            A[i][j] = (i == j) ? 1.0 : 0.0;
        }
    }
    return A;
}

// Free the memory allocated for the matrix
void free_matrix(double **A, int size) {
    for (int i = 0; i < size; i++) {
        free(A[i]);
    }
    free(A);
}

// Print a vector (for debugging)
void print_vector(double *v, int size) {
    printf("Vector: ");
    for (int i = 0; i < size; i++) {
        printf("%f ", v[i]);
    }
    printf("\n");
}

int main() {
    FILE *data_file = fopen("benchmark_results.txt", "w");
    if (data_file == NULL) {
        printf("Error opening data file!\n");
        return 1;
    }

    fprintf(data_file, "# Size MegaFLOPS Time(s)\n");

    // Create a script file for gnuplot
    FILE *plot_file = fopen("plot_benchmark.gp", "w");
    if (plot_file == NULL) {
        printf("Error opening plot file!\n");
        fclose(data_file);
        return 1;
    }

    // Write gnuplot commands
    fprintf(plot_file, "set terminal png size 800,600\n");
    fprintf(plot_file, "set output 'benchmark_results.png'\n");
    fprintf(plot_file, "set title 'Matrix-Vector Multiplication Performance'\n");
    fprintf(plot_file, "set xlabel 'Matrix Size (N)'\n");
    fprintf(plot_file, "set ylabel 'Performance (MFLOPS)'\n");
    fprintf(plot_file, "set grid\n");
    fprintf(plot_file, "set key top left\n");
    fprintf(plot_file, "plot 'benchmark_results.txt' using 1:2 with linespoints title 'MFLOPS', \\\n");
    fprintf(plot_file, "     'benchmark_results.txt' using 1:2 smooth bezier with lines title 'Trend' lw 2\n");

    fclose(plot_file);

    // Testing different sizes
    int sizes[] = {10, 20, 50, 100, 150, 200, 250, 300, 400, 500};
    int num_sizes = sizeof(sizes) / sizeof(sizes[0]);

    printf("Matrix-Vector Multiplication Benchmark\n");
    printf("--------------------------------------\n");
    printf("Size\tMFLOPS\tTime(s)\n");

    for (int idx = 0; idx < num_sizes; idx++) {
        int N = sizes[idx];

        // Generate test data
        double *vector = generate_vector(N);
        double **matrix = generate_matrix(N);

        // Allocate result vector
        double *result = (double *)malloc(N * sizeof(double));
        double *temp = (double *)malloc(N * sizeof(double));

        // Copy initial vector to temp
        for (int i = 0; i < N; i++) {
            temp[i] = vector[i];
        }

        struct timespec start_time, end_time;
        clock_gettime(CLOCK_MONOTONIC, &start_time);

        // Run the benchmark
        for (int run = 0; run < RUNS; run++) {
            matrix_vector_multiply(matrix, temp, result, N);

            // Swap pointers for next iteration
            double *swap = temp;
            temp = result;
            result = swap;
        }

        clock_gettime(CLOCK_MONOTONIC, &end_time);

        // Calculate elapsed time
        double elapsed_seconds = (end_time.tv_sec - start_time.tv_sec) +
                                 (end_time.tv_nsec - start_time.tv_nsec) / 1.0e9;

        // Calculate MFLOPS (2*N*N operations per matrix-vector mult)
        double flops = 2.0 * N * N * RUNS;
        double mflops = (flops / elapsed_seconds) / 1.0e6;

        printf("%d\t%.2f\t%.4f\n", N, mflops, elapsed_seconds);
        fprintf(data_file, "%d %.2f %.4f\n", N, mflops, elapsed_seconds);

        // Free memory
        free(vector);
        free_matrix(matrix, N);
        free(result);
        free(temp);
    }

    fclose(data_file);

    // Generate the plot
    printf("\nGenerating performance plot...\n");
    system("gnuplot plot_benchmark.gp");
    printf("Plot saved as 'benchmark_results.png'\n");

    return 0;
}