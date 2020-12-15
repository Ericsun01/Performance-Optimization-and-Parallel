#include <iostream>

#include "utils.hpp"

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
  // int P = std::stoi(argv[1]);
  int M = std::stoi(argv[2]);
  double A = std::stod(argv[3]);
  int N = std::stoi(argv[4]);
  char * filename = argv[5];
  // initialize the landscape
  std::vector<std::vector<Node *> > landscape(N, std::vector<Node *>(N, nullptr));
  // initialize
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      landscape[i][j] = new Node();
    }
  }
  // read the evaluation value from file
  readLandscape(filename, N, landscape);
  // calculate the water movement direction
  calNeigh(landscape, N);

  bool isFinished = false;
  int timesteps = 0;
  double tmp;
  Node * node;

  clock_gettime(CLOCK_MONOTONIC, &start_time);
  while (!isFinished) {
    isFinished = true;
    timesteps++;
    // first iteration, do
    // 1) add 1 rain drop to each point if still raining
    // 2) absorb A rain drop if there is still rain left
    // 3) calculate the trickle amount & direction
    // TODO: probably can use Loop Unswitching here
    for (int i = 0; i < N; i++) {
      for (int j = 0; j < N; j++) {
        node = landscape[i][j];
        if (timesteps <= M) {
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
      }
    }
    // second iteration, do
    // 1) use the calculated result from 1, update all lowest neighbors
    for (int i = 0; i < N; i++) {
      for (int j = 0; j < N; j++) {
        node = landscape[i][j];
        // tickle to each lowest neighbor
        for (Node * n : node->neigh) {
          n->rainLeft += node->trickleAmount;
        }
      }
    }

    
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
