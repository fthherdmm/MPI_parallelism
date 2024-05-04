# MPI_parallelism

The parallel matrix-vector multiplication algorithm is implemented using MPI, where each process is responsible for computing a portion of the output vector. The algorithm involves distributing the matrix rows among the processes, distributing the vector elements as needed, and performing local computations to compute partial results. Finally, the partial results are combined to obtain the final output vector. 

## To execute these files write the codes above: <br />
**for the sequential codes:** <br />
`gcc -o sequential sequential.c -lm`<br />
`./sequential testX.txt testA.txt`

**for the point-to-point communication codes:** <br />
`mpicc -o point-to-point point-to-point.c -lm`<br />
`mpirun -n <number_of_processes> ./point-to-point testA.txt testX.txt`<br />

**for the collective communication codes:** <br />
`mpicc -o collective collective.c -lm`<br />
`mpirun -n <number_of_processes> ./collective testA.txt testX.txt`<br />




