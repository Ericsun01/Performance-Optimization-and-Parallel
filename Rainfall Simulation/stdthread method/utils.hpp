#ifndef HW5_UTILS_H
#define HW5_UTILS_H

#include <atomic>
#include <climits>
#include <fstream>
#include <mutex>
#include <sstream>
#include <vector>

// A Node object represent each point on the landscape.
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
  // assign each node a mutex
  std::mutex mtx;
  Node() : elevation(0), rainAbsorbed(0), rainLeft(0), trickleAmount(0) {}
};


void preProcess(char* filename, int N, std::vector<std::vector<Node *> > & landscape) {
  // read input file
  std::ifstream infile(filename);
  std::string line;
  for (int i = 0; i < N; i++) {
    getline(infile, line, '\n');
    std::stringstream ss(line);
    for (int j = 0; j < N; j++) {
      ss >> landscape[i][j]->elevation;
    }
  }

  // select the choosen neigh of each point
  for(int i = 0; i < N; i++) {
    for(int j = 0; j < N; j++) {
      int minValue = INT_MAX;
      int value[5] = {INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX};
      // traversal itself and 4 neighbors
      // up
      if (i-1 >= 0 ) {
        value[0] = landscape[i-1][j]->elevation;
      }
      // left
      if (j-1 >= 0 ) {
        value[1] = landscape[i][j-1]->elevation;
      } 
      // down
      if (i+1 < N) {
        value[2] = landscape[i+1][j]->elevation;
      }
      // right
      if (j+1 < N) {
        value[3] = landscape[i][j+1]->elevation;
      }
      // center
      value[4] = landscape[i][j]->elevation;
      for (int k=0; k<4; k++) {
        minValue = std::min(minValue, value[k]);
      }

      // if center point is the lowest
      if (value[4] <= minValue) {
        continue;
      }
      
      // generate neigh array
      for (int m=0; m<4; m++) {
        if(value[m]==minValue) {
          switch (m)
          {
          case 0:
            landscape[i][j]->neigh.push_back(landscape[i-1][j]);
            break;
          case 1:
            landscape[i][j]->neigh.push_back(landscape[i][j-1]);
            break;
          case 2:
            landscape[i][j]->neigh.push_back(landscape[i+1][j]);
            break;
          case 3:
            landscape[i][j]->neigh.push_back(landscape[i][j+1]);
            break;
          }
        }
      }
      // debugging print
      // for (int m=0; m<4; m++) {
      //   std::cout<<"value ["<<m<<"] is "<<value[m]<<std::endl;
      // }
      // std::cout<<"current node ["<<i<<", "<<j<<"] has neighbors: "<<std::endl;
      // for(Node *node : landscape[i][j]->neigh) {       
      //     std::cout<<node->elevation<<std::endl;
      // }
    }
  }
}

double calc_time(struct timespec start, struct timespec end) {
  double start_sec = (double)start.tv_sec * 1000000000.0 + (double)start.tv_nsec;
  double end_sec = (double)end.tv_sec * 1000000000.0 + (double)end.tv_nsec;

  if (end_sec < start_sec) {
    return 0;
  }
  else {
    return end_sec - start_sec;
  }
}

#endif  //HW5_UTILS_H