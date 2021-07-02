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

constexpr int RUN_TIMES = 2;
constexpr static size_t kArraySize = 2;
using array_t = shad::impl::array<int, kArraySize>;
using iterator = array_t::iterator;

template <typename ExecutionPolicy>
void shad_fill_algorithm(ExecutionPolicy &&policy, shad::array<int, kArraySize> &in) {
  for(int i = 0; i < RUN_TIMES; ++i)
  {
    shad::fill(std::forward<ExecutionPolicy>(policy), in.begin(), in.end(), 42);
  }
}

namespace shad {

int main(int argc, char *argv[]) {
 // array
  shad::array<int, kArraySize> in;

  std::cout << "shad::arrray, using " << shad::rt::numLocalities() 
              << " localities, running shad::fill algorithm for " 
              << RUN_TIMES << " times \n";

  // shad fill algorithm using distributed_sequential_tag
  {
    auto execute_time = shad::measure<std::chrono::seconds>::duration(
      [&]() { shad_fill_algorithm(shad::distributed_sequential_tag{},in); });
    std::cout << "Using sequential policy takes " << execute_time.count() 
              << " seconds \n";
  }

    // shad fill algorithm using distributed_parallel_tag
  {
    auto execute_time = shad::measure<std::chrono::seconds>::duration(
      [&]() { shad_fill_algorithm(shad::distributed_parallel_tag{},in); });
    std::cout << "Using parallel policy takes " << execute_time.count() 
              << " seconds \n"; 
  }


  return 0;
}

}  // namespace shad