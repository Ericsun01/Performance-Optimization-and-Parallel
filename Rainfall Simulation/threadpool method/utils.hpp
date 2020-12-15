//
// Created by xkw on 2020/10/26.
//

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
  Node() : elevation(0), rainAbsorbed(0), rainLeft(0), trickleAmount(0) {}
};

struct NodeP : Node {
  // next node(either the right one, or the first one next line)
  NodeP * next;
  // assign each node a mutex
  std::mutex mtx;
  NodeP() : next(nullptr) {}
};

// Read the landscape/evaluation from file.
void readLandscape(char * filename, int N, std::vector<std::vector<Node *> > & result) {
  std::ifstream infile(filename);
  std::string line;
  for (int i = 0; i < N; i++) {
    getline(infile, line, '\n');
    std::stringstream ss(line);
    for (int j = 0; j < N; j++) {
      ss >> result[i][j]->elevation;
    }
  }
}

// Calculate the lowest neighbor(s) of each point.
void calNeigh(std::vector<std::vector<Node *> > & landscape, int N) {
  int r[4] = {-1, 1, 0, 0};
  int c[4] = {0, 0, -1, 1};
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      int minValue = INT_MAX;
      // traversal 4 neighbors
      for (int k = 0; k < 4; k++) {
        int row = i + r[k];
        int col = j + c[k];
        if (row >= 0 && row < N && col >= 0 && col < N) {
          minValue = std::min(minValue, landscape[row][col]->elevation);
        }
      }
      if (landscape[i][j]->elevation <= minValue) {
        // current point is the lowest point among its neighbours, no water movement
        continue;
      }
      for (int k = 0; k < 4; k++) {
        int row = i + r[k];
        int col = j + c[k];
        if (row >= 0 && row < N && col >= 0 && col < N) {
          if (landscape[row][col]->elevation == minValue) {
            // push the lowest neighbor
            landscape[i][j]->neigh.push_back(landscape[row][col]);
          }
        }
      }
    }
  }
}

void updateNext(std::vector<std::vector<Node *> > & landscape, int N) {
  // update the next field
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      // if not last column, then next = right;
      ((NodeP *)landscape[i][j])->next =
          (NodeP *)(j == N - 1 ? (i == N - 1 ? nullptr : landscape[i + 1][0])
                               : landscape[i][j + 1]);
    }
  }
}
/*void readLandscape(char * filename, int N, std::vector<std::vector<NodeP *> > & result) {
  std::ifstream infile(filename);
  std::string line;
  for (int i = 0; i < N; i++) {
    getline(infile, line, '\n');
    std::stringstream ss(line);
    for (int j = 0; j < N; j++) {
      ss >> result[i][j]->evaluation;
    }
  }
}

// Calculate the lowest neighbor(s) of each point.
void calNeigh(std::vector<std::vector<NodeP *> > & landscape, int N) {
  int r[4] = {-1, 1, 0, 0};
  int c[4] = {0, 0, -1, 1};
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      int minValue = INT_MAX;
      // traversal 4 neighbors
      for (int k = 0; k < 4; k++) {
        int row = i + r[k];
        int col = j + c[k];
        if (row >= 0 && row < N && col >= 0 && col < N) {
          minValue = std::min(minValue, landscape[row][col]->evaluation);
        }
      }
      if (landscape[i][j]->evaluation <= minValue) {
        // current point is the lowest point among its neighbours, no water movement
        continue;
      }
      for (int k = 0; k < 4; k++) {
        int row = i + r[k];
        int col = j + c[k];
        if (row >= 0 && row < N && col >= 0 && col < N) {
          if (landscape[row][col]->evaluation == minValue) {
            // push the lowest neighbor
            landscape[i][j]->neigh.push_back(landscape[row][col]);
          }
        }
      }
    }
  }
  // update the next field
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      // if not last column, then next = right;
      landscape[i][j]->next = (j == N - 1 ? (i == N - 1 ? nullptr : landscape[i + 1][0])
                                          : landscape[i][j + 1]);
    }
  }
  }*/

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
