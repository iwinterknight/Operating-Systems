This repo contains some classical problems in Operating Systems.

# Restaurant Problem
A restaurant requires careful coordination of resources,
which are the tables in the restaurant, the cooks available to cook the order, and the machines
available to cook the food (we assume that there is no contention on other possible resources,
like servers to take orders and serve the orders).

<p align="center">
  ![Restaurant](http://github.com/iwinterknight/Operating-Systems/Restaurant.gif)
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
