#include <atomic>
#include <functional>
#include <iostream>
#include <mutex>
#include <queue>

#include "threadpool.hpp"
#include "utils.hpp"

void doAbsorb(Node * start,
              int cnt,
              double A,
              bool isRaining,
              std::atomic<bool> & isFinished) {
  Node * node = start;
  double tmp;
  int c = 0;
  while (c < cnt) {
    if (isRaining) {
      // still raining, add 1 raindrop
      node->rainLeft++;
    }
    // absorb A amount of water if there are still raindrop left
    tmp = std::min(node->rainLeft, A);
    node->rainLeft -= tmp;
    node->rainAbsorbed += tmp;
    // calculate the tickle amount (evenly distribute)
    if (!node->neigh.empty()) {
      tmp = std::min(node->rainLeft, 1.0);
      node->trickleAmount = tmp / node->neigh.size();
      node->rainLeft -= tmp;
    }
    if (tmp > 0 || node->rainLeft > 0) {
      isFinished = false;
    }
    c++;
    node = ((NodeP *)node)->next;
  }
}

void doTickle(Node * start, int cnt) {
  NodeP * node = (NodeP *)start;
  int c = 0;
  while (c < cnt) {
    // tickle to each lowest neighbor
    for (Node * n : node->neigh) {
      std::unique_lock<std::mutex> lck(((NodeP *)n)->mtx);
      n->rainLeft += node->trickleAmount;
      lck.unlock();
    }
    c++;
    node = node->next;
  }
}

int main(int argc, char * argv[]) {
  if (argc < 6) {
    std::cout << "usage: ./rainfall <P> <M> <A> <N> <elevation_file>";
    return 1;
  }
  struct timespec start_time, end_time;
  // P = # of parallel threads to use.
  // M = # of simulation time steps during which a rain drop will fall on each landscape point.
  // In other words, 1 rain drop falls on each point during the first M steps of the simulation.
  // A = absorption rate (specified as a floating point number). The amount of raindrops that
  // are absorbed into the ground at a point during a timestep.
  // N = dimension of the landscape (NxN)
  // elevation_file = name of input file that specifies the elevation of each point.

  // read all arguments
  int P = std::stoi(argv[1]);
  int M = std::stoi(argv[2]);
  double A = std::stod(argv[3]);
  int N = std::stoi(argv[4]);
  char * filename = argv[5];
  // initialize the landscape
  std::vector<std::vector<Node *> > landscape(N, std::vector<Node *>(N, nullptr));
  // initialize
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      landscape[i][j] = new NodeP();
    }
  }
  // read the evaluation value from file
  readLandscape(filename, N, landscape);
  // calculate the water movement direction
  calNeigh(landscape, N);
  // update the next pointer of NodeP
  updateNext(landscape, N);

  // multiple thread will modify this, make it atomic
  std::atomic<bool> isFinished(false);
  int timesteps = 0;

  ECE565::ThreadPool tp(P);
  // task per thread
  std::vector<int> cntList;
  // starting index of each thread
  std::vector<int> indexList;

  int cnt = N * N / P;
  for (int i = 0; i < P; i++) {
    int index = i * cnt;
    cnt = (i == P - 1 ? N * N - index : cnt);
    indexList.push_back(index);
    cntList.push_back(cnt);
  }

  Node * node;
  bool isRaining;

  clock_gettime(CLOCK_MONOTONIC, &start_time);
  while (!isFinished) {
    isFinished = true;
    timesteps++;
    // first iteration, do
    // 1) add 1 rain drop to each point if still raining
    // 2) absorb A rain drop if there is still rain left
    // 3) calculate the trickle amount & direction
    // TODO: probably can use Loop Unswitching here
    for (int i = 0; i < P; i++) {
      int index = indexList[i];
      int row = index / N;
      int col = index % N;
      // get the start node
      node = landscape[row][col];
      isRaining = timesteps <= M;
      tp.addTask([node, cntList, i, A, isRaining, &isFinished] {
        doAbsorb(node, cntList[i], A, isRaining, isFinished);
      });
    }

    tp.waitAll();

    // second iteration, do
    // 1) use the calculated result from 1, update all lowest neighbors
    for (int i = 0; i < P; i++) {
      int index = indexList[i];
      int row = index / N;
      int col = index % N;
      node = landscape[row][col];
      tp.addTask([node, cntList, i] { doTickle(node, cntList[i]); });
    }

    tp.waitAll();
    
  }

  clock_gettime(CLOCK_MONOTONIC, &end_time);

  double elapsed_s = calc_time(start_time, end_time) / 1000000000.0;

  std::cout << "Rainfall simulation took " << timesteps << " time steps to complete.\n";
  std::cout << "Runtime = " << elapsed_s << " seconds\n\n";
  std::cout
      << "The following grid shows the number of raindrops absorbed at each point:\n";
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      std::cout.width(8);
      std::cout << landscape[i][j]->rainAbsorbed;
    }
    std::cout << '\n';
  }

  // release all resource
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      delete landscape[i][j];
    }
  }
}
