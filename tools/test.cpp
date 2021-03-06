
#include "gflags/gflags.h"
#include "glog/logging.h"
#include "ditree.hpp"
#include <fstream>
#include <cmath>

using namespace std;
using namespace ditree;

int main(int argc, char** argv) {

  cout << digamma(0) << endl;
  cout << digamma(-0.01) << endl;
  cout << digamma(0.01) << endl;
  cout << digamma(200) << endl;

  cout << digamma(-266.391) << endl;
  cout << digamma(67152) << endl;
  cout << digamma(66865.6) << endl;
  cout << digamma(16545.7) << endl;

  return 0;
} 
