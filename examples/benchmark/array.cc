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

constexpr int repetitions = 10;
constexpr static size_t kArraySize = 1000000;
using array_t = shad::impl::array<int, kArraySize>;
using iterator = array_t::iterator;

namespace shad {

int main(int argc, char *argv[]) {

  std::cout << "shad::array, size of " << kArraySize 
            << ", using " << shad::rt::numLocalities() 
            << " localities, running each shad STL algorithm for " 
            << repetitions << " times, and take average: \n";

  // set up
  shad::array<int, kArraySize> in;
  for (size_t i = 0; i < kArraySize; i++) {
      in[i] = i + 1;
  }

  //////////////////////////////////////////////////////////////////////
  // shad count_if algorithm 
  // using distributed_sequential_tag
  {
    // warm up loop
    for (int i = 0; i < 10; i ++){
      shad::count_if(shad::distributed_sequential_tag{}, in.begin(), in.end(),
                  [](int &i) { return i % 4 == 0; });
    }

    // timing loop
    auto start = std::chrono::steady_clock::now();
    for(int i = 0; i < repetitions; ++i)
    {
      shad::count_if(shad::distributed_sequential_tag{}, in.begin(), in.end(),
                  [](int &i) { return i % 4 == 0; });
    }
    std::chrono::duration<double, std::chrono::seconds::period> duration = 
      std::chrono::steady_clock::now() - start;

    std::cout << "shad::count_if with sequential policy takes " 
              << (duration.count()/repetitions) << " seconds \n"; 
  }

  // using distributed_parallel_tag
  {
    // warm up loop
    for (int i = 0; i < 10; i ++){
      shad::count_if(shad::distributed_parallel_tag{}, in.begin(), in.end(),
                  [](int &i) { return i % 4 == 0; });
    }

    // timing loop
    auto start = std::chrono::steady_clock::now();
    for(int i = 0; i < repetitions; ++i)
    {
      shad::count_if(shad::distributed_parallel_tag{}, in.begin(), in.end(),
                  [](int &i) { return i % 4 == 0; });
    }
    std::chrono::duration<double, std::chrono::seconds::period> duration = 
      std::chrono::steady_clock::now() - start;

    std::cout << "shad::count_if with parallel policy takes " 
              << (duration.count()/repetitions) << " seconds \n"; 
  }

  //////////////////////////////////////////////////////////////////////

  // shad minmax algorithm 
  // using distributed_sequential_tag
  {
    // warm up loop
    for (int i = 0; i < 10; i ++){
      shad::minmax_element(shad::distributed_sequential_tag{}, in.begin(),
                           in.end());
    }

    // timing loop
    auto start = std::chrono::steady_clock::now();
    for(int i = 0; i < repetitions; ++i)
    {
      shad::minmax_element(shad::distributed_sequential_tag{}, in.begin(), 
                           in.end());
    }
    std::chrono::duration<double, std::chrono::seconds::period> duration = 
      std::chrono::steady_clock::now() - start;

    std::cout << "shad::minmax_element with sequential policy takes " 
              << (duration.count()/repetitions) << " seconds \n"; 
  }

  // using distributed_parallel_tag
  {
    // warm up loop
    for (int i = 0; i < 10; i ++){
      shad::minmax_element(shad::distributed_parallel_tag{}, in.begin(),
                           in.end());
    }

    // timing loop
    auto start = std::chrono::steady_clock::now();
    for(int i = 0; i < repetitions; ++i)
    {
      shad::minmax_element(shad::distributed_parallel_tag{}, in.begin(), 
                           in.end());
    }
    std::chrono::duration<double, std::chrono::seconds::period> duration = 
      std::chrono::steady_clock::now() - start;

    std::cout << "shad::minmax_element with parallel policy takes " 
              << (duration.count()/repetitions) << " seconds \n"; 
  }

  return 0;
}

}  // namespace shad