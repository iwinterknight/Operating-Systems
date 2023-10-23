Name : Sunit Singh
Date : May 13, 2023

Instructions :

Compiling: run `make`

Running: The compilation output of `make` will create an executable `matmul`. To run the executable run `./matmul [NUM_THREADS]` where [NUM_THREADS] is the number of threads between 1 & 5 threads. This will execute the matrix multiplication serially and then with the number of threads specified. The program will compare the results of the multithreaded executions against the serial execution to check for errors in multiplication, and will create timestamps.

The output is in the following format :

Threads Seconds Errors
1       2.23000 No Errors
2       1.4053  No Errors
3       0.62    No Errors
4       0.46    No Errors
5       0.52    No Errors

NOTE : With 5 threads the code can take upto 20 sec to execute (Matrix multiplication done 6 times, once serial & 5 times for multithreaded cases).
