#include <stdlib.h>
#include <iostream>
#include <random>

#include "shad/core/algorithm.h"
#include "shad/core/array.h"
#include "shad/util/measure.h"

namespace shad {

int main(int argc, char *argv[]) {
  
  /***
  Question:
  1. create an array containing 1, 3, 5, 7
  2. use shad::replace to replace 5 to 42
***/


// solution:

  // create array
  shad::array<int, 4> array_;

  for (size_t i = 0; i < 4; ++i) {
    array_[i] = 2 * (i+1) - 1;
  }

  for(auto v: array_) std::cout << v << std::endl;

  // use shad::replace
  shad::replace(shad::distributed_parallel_tag{}, array_.begin(), array_.end(), 5, 42);
   for(auto v: array_) std::cout << v << std::endl;


  return 0;
}

}  // namespace shad




