#include <iostream>
#include <atomic>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>

#include "utils.hpp"
using namespace std;

int P;
int M;
double A;
int N;
char * filename;
std::atomic<bool> isFinished(false);

void doAbsorb(int I, int timesteps, std::vector<std::vector<Node *> > landscape) {
  int startRow = I * N/P;
  int endRow = (I+1) * (N/P);
  double tmp=0.0;
  double trickle=0.0;
    for(int i=startRow; i<endRow; i++) {
      for(int j=0; j<N; j++) {
          if(timesteps < M) {
            landscape[i][j]->rainLeft++;
          } 
          // 2) if there are raindrops on a point, absorb into point
          tmp = std::min(landscape[i][j]->rainLeft, (double)A);
          landscape[i][j]->rainAbsorbed += tmp;
          landscape[i][j]->rainLeft -= tmp;

          // 3a) get number of raindrops that will next trickle to the lowest neighbors
          if(!landscape[i][j]->neigh.empty()) {
            trickle = min(landscape[i][j]->rainLeft, 1.0);
            landscape[i][j]->trickleAmount = trickle/(landscape[i][j]->neigh.size());
            landscape[i][j]->rainLeft -= trickle;
          }
          if(trickle > 0 || landscape[i][j]->rainLeft > 0) {
            isFinished = false;
          }
      }
  }    
}     


void doTrickle(int I, std::vector<std::vector<Node *> > landscape) {
  int startRow = I * N/P;
  int endRow = (I+1) * (N/P);

    for(int i=startRow; i<endRow; i++) {
      for(int j=0; j<N; j++) {
        // 3b) use calculated number of raindrops to update raindrops at each lowest neighbors
        for(Node *node : landscape[i][j]->neigh) {
          std::unique_lock<std::mutex> lck(((Node *)node)->mtx);
          node->rainLeft += landscape[i][j]->trickleAmount;
          lck.unlock();
        }
      }
    }  
}

void waitAll(std::vector<std::thread> & threads) {
  for (auto & th : threads) {
    if (th.joinable()) {
      th.join();
    }
  }
  threads.clear();
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
  P = std::stoi(argv[1]);
  M = std::stoi(argv[2]);
  A = std::stod(argv[3]);
  N = std::stoi(argv[4]);
  filename = argv[5];
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
  std::vector<std::thread> threads;
  
  int timesteps = 0;
 
  // execution flow
  clock_gettime(CLOCK_MONOTONIC, &start_time);
  while (!isFinished)
  {
    isFinished=true;  
    for (int i = 0; i < P; i++) {
      // thread t(doAbsorb, i, timesteps, landscape);
      // threads.push_back(t);
      threads.push_back(std::thread([i, timesteps ,landscape] { doAbsorb(i, timesteps,landscape); }));
    }
    waitAll(threads); 

    for (int i = 0; i < P; i++) {
      // thread t(doTrickle, i, landscape);
      // threads.push_back(t);
      threads.push_back(std::thread([i, landscape] { doTrickle(i, landscape); }));
    }
    waitAll(threads);

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