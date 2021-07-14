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

#include <chrono>
#include <iostream>

#include "shad/core/algorithm.h"
#include "shad/core/unordered_set.h"


constexpr int repetitions = 10;
constexpr static size_t kSize = 1000000;
using set_t = shad::Set<int>;
using iterator = set_t::iterator;
using value_type = set_t::value_type;
using shad_inserter_t = shad::insert_iterator<shad::unordered_set<int>>;
using shad_buffered_inserter_t =
    shad::buffered_insert_iterator<shad::unordered_set<int>>;

namespace shad {

int main(int argc, char *argv[]) {

  std::cout << "shad::unordered_set, size of " << kSize 
            << ", using " << shad::rt::numLocalities() 
            << " localities, running each shad STL algorithm for " 
            << repetitions << " times, and take average: \n";
  //////////////////////////////////////////////////////////////////////
  // set up
  shad::unordered_set<int> in;  
  shad_buffered_inserter_t ins(in, in.begin());
  for (size_t i = 0; i < kSize; ++i) {
    ins = 2 * (i + 1);
  }
  ins.wait();
  ins.flush();

    std::cout << "Done set up \n";

  //////////////////////////////////////////////////////////////////////
  // shad count_if algorithm 
  // using distributed_sequential_tag
  {
    // warm up loop
    for (int i = 0; i < 10; i ++){
      shad::count_if(shad::distributed_sequential_tag{}, in.begin(), in.end(),
                     [](const value_type &i) { return i % 4 == 0; });
    }

    // timing loop
    auto start = std::chrono::steady_clock::now();
    for(int i = 0; i < repetitions; ++i)
    {
      shad::count_if(shad::distributed_sequential_tag{}, in.begin(), in.end(),
                     [](const value_type &i) { return i % 4 == 0; });
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
                     [](const value_type &i) { return i % 4 == 0; });
    }

    // timing loop
    auto start = std::chrono::steady_clock::now();
    for(int i = 0; i < repetitions; ++i)
    {
      shad::count_if(shad::distributed_parallel_tag{}, in.begin(), in.end(),
                     [](const value_type &i) { return i % 4 == 0; });
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