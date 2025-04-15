# Matrix-Vector Multiplication Benchmarks

## About the Project

This project implements and compares two approaches to matrix-vector multiplication:

* **Serial Benchmark**: Single-processor implementation in C using POSIX timers
* **MPI Benchmark**: Distributed implementation using Message Passing Interface

**Key Differences:**
- Serial version runs on one processor; MPI distributes work across multiple processes
- MPI version scales with available nodes but introduces communication overhead
- Both measure performance in MFLOPS across different matrix sizes
- Both use GNUPlot for performance visualization

## Key Learnings in HPC

- **Parallelization Trade-offs**: Communication overhead vs. computational benefits
- **Scaling Behavior**: Performance varies with matrix size and process count
- **Memory Access Patterns**: Impact on cache utilization and overall performance
- **Resource Utilization**: MFLOPS measurements reveal efficiency of implementations
- **Optimization Techniques**: Loop structure, memory management, workload balancing
- **Benchmarking Practices**: Proper timing, visualization, and performance metrics

These benchmarks demonstrate fundamental HPC concepts through practical implementation and performance measurement.