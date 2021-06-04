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

#include <iostream>
#include <random>
#include <stdlib.h>

#include "shad/core/array.h"
#include "shad/core/algorithm.h"
#include "shad/util/measure.h"
#include "shad/core/unordered_set.h"

constexpr static size_t kArraySize = 4;

void shad_generate_algirthm(shad::array<int, kArraySize> &in){
  shad::generate(
    shad::distributed_parallel_tag{},
    in.begin(), in.end(), 
    [=]() {
        std::random_device rd;
        std::default_random_engine G(rd());
        std::uniform_int_distribution<int> dist(1, 10);
        return dist(G);
    }
  );
}

void shad_fill_algirthm(shad::array<int, kArraySize> &in){
  shad::fill(
    shad::distributed_parallel_tag{},
    in.begin(), in.end(), 42);
}

namespace shad {

int main(int argc, char *argv[]) {

  shad::array<int, kArraySize> in;

  // shad fill algorithm
  auto execute_time = shad::measure<std::chrono::nanoseconds>::duration(
    [&](){shad_fill_algirthm(in);});
  std::cout << "Using " << shad::rt::numLocalities() 
            << " localities, shad fill of array took " 
            << execute_time.count() << "nanoseconds" << std::endl;

  // shad generate algorithm
  execute_time = shad::measure<std::chrono::nanoseconds>::duration(
    [&](){shad_generate_algirthm(in);});
  std::cout << "Using " << shad::rt::numLocalities() 
            << " localities, shad generate of array took " 
            << execute_time.count() << "nanoseconds" << std::endl;

  
  return 0;
}

} //namespace shad
