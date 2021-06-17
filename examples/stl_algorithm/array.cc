//===------------------------------------------------------------*- C++ -*-===//
//
//                                     SHAD
//
//      The Scalable High-performance Algorithms and Data Structure Library
//
//===----------------------------------------------------------------------===//
//
// Copyright 2018 Battelle Memorial Institute
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not
// use this file except in compliance with the License. You may obtain a copy
// of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
// License for the specific language governing permissions and limitations
// under the License.
//
//===----------------------------------------------------------------------===/

#include <stdlib.h>
#include <iostream>
#include <random>

#include "shad/core/algorithm.h"
#include "shad/core/array.h"
#include "shad/util/measure.h"

namespace shad {

int main(int argc, char *argv[]) {
  // array
  shad::array<int, 4> array_;

  // shad fill algorithm
  shad::fill(shad::distributed_parallel_tag{}, array_.begin(), array_.end(), 42);
  std::cout << "==> After using shad::fill, array is " << std::endl;
  for (auto v: array_) std::cout << v << "\n";


  // shad generate algorithm
  shad::generate(shad::distributed_parallel_tag{}, array_.begin(), array_.end(), [=]() {
    std::random_device rd;
    std::default_random_engine G(rd());
    std::uniform_int_distribution<int> dist(1, 10);
    return dist(G);
  });
  std::cout << "==> After using shad::generate, array is " << std::endl;
  for (auto v: array_) std::cout << v << "\n";


  // shad count algorithm
  size_t counter =
      shad::count(shad::distributed_parallel_tag{}, array_.begin(), array_.end(), 5);
  std::cout << "==> After using shad::count, the counter of 5 is: " << counter << std::endl;


  // shad find_if algorithm
  using iterator = shad::impl::array<int, 4>::iterator;
  iterator iter = shad::find_if(shad::distributed_parallel_tag{}, array_.begin(),
                            array_.end(), [](int i) { return i % 2 == 0; });
  std::cout << "==> After using shad::find_if, ";
  (iter != array_.end())
      ? std::cout << "array contains an even number" << std::endl
      : std::cout << "array does not contain even numbers" << std::endl;


  // shad minmax algorithm
  std::pair<iterator, iterator> min_max = shad::minmax_element(
      shad::distributed_parallel_tag{}, array_.begin(), array_.end());
  std::cout << "==> After using shad::minmax, min = " << *min_max.first 
            << ", max= " << *min_max.second << std::endl;
  

  // shad transform algorithm
  shad::transform(shad::distributed_parallel_tag{}, array_.begin(), array_.end(),
                  array_.begin(), [](int i) { return i + 100; });
  std::cout << "==> After using shad::transform, array is " << std::endl;
  for (auto v: array_) std::cout << v << "\n";

  return 0;
}

}  // namespace shad
