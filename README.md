This repo contains some classical problems in Operating Systems.

# Restaurant Problem
A restaurant requires careful coordination of resources,
which are the tables in the restaurant, the cooks available to cook the order, and the machines
available to cook the food (we assume that there is no contention on other possible resources,
like servers to take orders and serve the orders).
<p align="center">
  <img width="789" alt="Restaurant" src="https://github.com/iwinterknight/Operating-Systems/blob/master/Restaurant.gif">
</p>

Eaters in the restaurant place their orders when they get seated. These orders are then handled
by available cooks. Each cook handles one order at a time. A cook handles an order by using
machines to cook the food items. To simplify the simulation, we make the following
assumptions. There are only three types of food served by the restaurant - a Buckeye Burger,
Brutus Fries, and Coke. Each person entering the restaurant occupies one table and orders one
or more burgers, zero or more orders of fries, and zero or one glass of coke. The cook needs to
use the burger machine for 5 minutes to prepare each burger, fries machine for 3 minutes for
one order of fries, and the soda machine for 1 minute to fill a glass with coke. The cook can use
at most one of these three machines at any given time. There is only one machine of each type
in the restaurant (so, for example, over a 5-minute duration, only one burger can be prepared).
Once the food (all items at the same time) are brought to a diner's table, they take exactly 30
minutes to finish eating them, and then leave the restaurant, making the table available for
another diner (immediately).

he simulation has four sets of
parameters - the number of eaters (diners) that wish to enter the restaurant, the times they
approach the restaurant and what they order, the number of tables in the restaurant (there
can only be as many eaters as there are tables at a time; other eaters must wait for a free
table before placing their order), and the number of cooks in the kitchen that process orders.

# ---------------------------------------------------------------------------------------------------------------------------------
# Thread Level Parallelism
Designing programming solution for matrix multiplication that exploit multiple threads executing in parallel.

Let A be an n × m matrix and B be an m × p matrix. Then C = AB yields C, an n × p matrix. The code below illustrates an algorithm for matrix multiplication C = AB.
```
for (i = 0; i < n; i++) {
    for (j = 0; j < p; j++) {
         c[i][j] = 0;
         for (k = 0; k < m; k++) {
             c[i][j] += a[i][k] * b[k][j];
         }
    }
}
```
In this program, matrix A is 1200 × 1000 and matrix B is 1000 × 500 (i.e., n = 1200, m = 1000, and p = 500). Initializing elements of matrix A as follows: a[i][j] = i + j for i  = 0, 1, 2, …, n − 1 and j  = 0, 1, 2, …, m − 1. Initializing elements of matrix B as follows: b[i][j] = j for i  = 0, 1, 2, …, m − 1 and j  = 0, 1, 2, …, p − 1.

Steps:

1. Multiply these two matrices according to the algorithm given above. Let matrix C1 store the result of this multiplication. Display the time needed to perform the calculation (excluding the time required for initializing matrices A and B).

2. Develop an algorithm for matrix multiplication that can exploit n (where n = 2, 3, 4, …) threads executing in parallel.

3. Perform matrix multiplication using thread level parallelism. Main thread should create n threads that will multiply the matrices; each thread terminates when it completes its task. Matrix C stores the result of this calculation. Display the time needed to perform the calculation. If the system has at least two processors, time improvements should be observable. Compare matrix C with matrix C1 to verify if the parallelized matrix multiplication result concurs with the single-thread matrix multiplication execution.
