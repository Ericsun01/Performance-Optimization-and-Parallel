#include <iostream>

#include "utils.hpp"

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

  // get the neighbors for trickling 
  preProcess(filename, N, landscape);
  
  bool isFinished = false;
  int timesteps = 0;
  double tmp = 0.0;
  double trickle = 0.0;
 
  // execution flow
  clock_gettime(CLOCK_MONOTONIC, &start_time);
  while (!isFinished)
  {
    isFinished=true;
    
    // first traversal
    for(int i=0; i<N; i++) {
        for(int j=0; j<N; j++) {
            // 1) receive a new raindrop (if it is still raining) for each point
            if(timesteps < M) {
                landscape[i][j]->rainLeft++;
            }
            // 2) if there are raindrops on a point, absorb into point
            tmp = std::min(landscape[i][j]->rainLeft, A);
            landscape[i][j]->rainAbsorbed += tmp;
            landscape[i][j]->rainLeft -= tmp;

            // 3a) get number of raindrops that will next trickle to the lowest neighbors
            if(!landscape[i][j]->neigh.empty()) {
              trickle = std::min(landscape[i][j]->rainLeft, 1.0);
              landscape[i][j]->trickleAmount = trickle/(landscape[i][j]->neigh.size());
              landscape[i][j]->rainLeft -= trickle;
            }
            if(trickle > 0 || landscape[i][j]->rainLeft > 0) {
              isFinished = false;
          }
        } 
    }

    // second traversal
    for(int i=0; i<N; i++) {
      for(int j=0; j<N; j++) {
        // 3b) use calculated number of raindrops to update raindrops at each lowest neighbors
        for(Node *node : landscape[i][j]->neigh) {
          node->rainLeft += landscape[i][j]->trickleAmount;
        }
        
      }
    }  

    timesteps++;

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