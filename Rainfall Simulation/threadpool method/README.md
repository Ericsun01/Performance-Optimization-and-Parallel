[toc]

# ECE 565 Homework 5

* Kewei Xia (kx30)
* Junqi Sun (js895)

## Parallelize strategy

### Sequential version

In the sequential version, I follow exactly the procedure described in pdf. The basic structure is as followed, an outer most while loop (which simulates the timesteps), loop until there are no rain left on the surface of any points. In each iteration of the while loop, there are two for loop, one to rain & absorb, one for tickle.

```cpp
while (!isFinished) {
  isFinished = true;
  timesteps++;
  // first iteration, do
  // 1) add 1 rain drop to each point if still raining
  // 2) absorb A rain drop if there is still rain left
  // 3) calculate the trickle amount & direction
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
        // do something
    }
  }
  // second iteration, do
  // 1) use the calculated result from 1, update all lowest neighbors
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
        // do something
    }
  }
}
```

To facilitate the coding, I create a struct called Node, which represent a point on the 2D landscape, as followed:

```cpp
struct Node {
  int elevation;
  // accumulate absorbed rain (the value we interested)
  double rainAbsorbed;
  // the rain left on this point
  double rainLeft;
  // the amount of water will flow to neighbor(s)
  double trickleAmount;
  // can have multiple lowest neighbors
  std::vector<Node *> neigh;
  Node() : elevation(0), rainAbsorbed(0), rainLeft(0), trickleAmount(0) {}
};
```

### Parallel version

To parallelize the code, I choose to parallelize the two for loop. For several reasons: 

* 1) the program spend at least 90% time on those two for loop, so it's worthy to parallelize; 
* 2) we don't know the iteration times of the while loop, it's difficult to load balance statically, on the other side, we know exactly the iteration times of two for loop, so we can load balance each thread easily;
* 3) the while loop simulates the timesteps, which inherently is sequential (timestep 1 must happen after timestep 0), so it's not necessary to parallelize this while loop.

Note that, in sequential version, we store the landscape in an 2D array, one naive parallel strategy is passing this array between different threads, this will make the performance of parallel version even worse than the sequential one, even worse, the performance will degrade with the thread number increase (since more data movement).

#### (1) change data structure

To avoid this unnecessary data movement, I make a small change to our data structure. 1) add a `next` pointer; 2) add a `mutex` which used to protect current cell. So for every thread, I only need to specify the starting node, it can follow the next pointer to handle all nodes assign to it, no need to access the 2D array itself.

```cpp
struct NodeP : Node {
  // next node (either the right one, or the first one next line)
  NodeP * next;
  // assign each node a mutex
  std::mutex mtx;
  NodeP() : next(nullptr) {}
};
```

#### (2) thread management

There are two strategies of thread management:

* 1) create P thread whenever we need to parallelize some code;
* 2) use a threadpool to avoid the overhead of creation;

more detail sees the analysis section

#### (3) synchronization

Note that, for the first iteration, all operations happen in the same node, so no need to synchronize. The only place we might have race condition is in the second iteration when we need to update the neighbour(s). To minimize the critical section, I add a mutex for each node, so that we will only acquire a lock of a specific node before we want to modify it.

Since we are simulating the time, so we must add some kind of "barrier" before each iteration; logically, we must finish all first for loop before we proceed to the second one, we also need to finish one while loop iteration before we proceed to the next one.

```cpp
while (!isFinished) {
    // first for loop
    for (int i = 0; i < P; i++) { }
    // insert barrier
    // second for loop
    for (int i = 0; i < P; i++) { }
    // insert barrier
}
```

## Result analysis

`./<program> <#thread> 50 0.5 4096 measurement_4096x4096.in`

Here is the result of measurement:

| # threads | 1         | 2         | 4         | 8         |
| --------- | --------- | --------- | --------- | --------- |
| seq       | 353.359 s | 353.359 s | 353.359 s | 353.359 s |
| pt        | 785.415 s | 439.014 s | 257.19 s  | 176.453 s |
| th        | 797.581 s | 446.379 s | 266.868 s | 180.334 s |

* seq --- sequential version
* pt --- parallel version with threadpool
* th --- parallel version, create P threads whenever needed

My results match my expectation, there are several observations here:

* 1) when thread number is small (1 or 2), the parallel version is actually worse than the sequential version, because to parallelize the code, we introduce several overhead, e.g. acquire & release lock, barrier between different iterations;
* 2) when thread number gets bigger (4 or 8), the parallel version is better than the sequential version, i.e. the performance of parallel version is getting better when the number of threads increase, this is as we expected;
* 3) the performence of using threadpool is better than creating threads as needed; this also match our expectation, by using threadpool, we only need to create P threads once, and the time for creation is not in the critical path of this program (compared to creating threads as needed), so it will keep constant even the problem size grows;
* 4) the difference between "pt" and "th" version is not that big, I guess this is because of the overhead of joining thread is more expensive than creating thread (note that in both two versions, we need barrier between iterations);