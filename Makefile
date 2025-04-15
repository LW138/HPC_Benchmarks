CC=mpicc
CFLAGS=-O3
LDLIBS = -lm
TARGET=Benchmark


all: $(EXEC)

run_bench: Makefile
	./benchmarkNormal


run_mpi_bench: Makefile
	mpirun -np 4 ./benchmarkMpi -4

benchmarkMpi: benchmarkMpi.c
	$(CC) $(CFLAGS) -o benchmarkMpi benchmarkMpi.c $(LDLIBS)


benchmarkNormal: benchmarkNormal.c
	$(CC) $(CFLAGS) -o benchmarkNormal benchmarkNormal.c $(LDLIBS)

clean:
	rm -f $(TARGET)