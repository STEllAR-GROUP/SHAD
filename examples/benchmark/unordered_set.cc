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
#include "shad/core/unordered_set.h"
#include "shad/util/measure.h"

constexpr int repetitions = 2;
constexpr static size_t kSize = 2;
using set_t = shad::Set<int>;
using iterator = set_t::iterator;
using value_type = set_t::value_type;
using shad_inserter_t = shad::insert_iterator<shad::unordered_set<int>>;
using shad_buffered_inserter_t =
    shad::buffered_insert_iterator<shad::unordered_set<int>>;

void SetUp(shad::unordered_set<int> &in) {
    shad_buffered_inserter_t ins(in, in.begin());
    for (size_t i = 0; i < kSize; ++i) {
      ins = 2 * (i + 1);
    }
    ins.wait();
    ins.flush();
}


template <typename ExecutionPolicy>
void shad_count_algorithm(ExecutionPolicy &&policy,
                          shad::unordered_set<int> &in) {
  for(int i = 0; i < repetitions; ++i)
  {
    shad::count(std::forward<ExecutionPolicy>(policy), in.begin(), in.end(), 6);
  }
}


template <typename ExecutionPolicy>
void shad_minmax_algorithm(ExecutionPolicy &&policy,
                            shad::unordered_set<int> &in) {
  for(int i = 0; i < repetitions; ++i)
  {
    shad::minmax_element(std::forward<ExecutionPolicy>(policy), in.begin(),
                         in.end());
  }
}

template <typename ExecutionPolicy, typename shad_inserter>
void shad_transform_algorithm(ExecutionPolicy &&policy,
                            shad::unordered_set<int> &in) {
  for(int i = 0; i < repetitions; ++i)
  {
    shad::transform(std::forward<ExecutionPolicy>(policy), in.begin(), in.end(),
                    shad_inserter(in, in.begin()),
                    [](const value_type &i) { return i + 2; });
  }
}

namespace shad {

int main(int argc, char *argv[]) {

  shad::unordered_set<int> set_;

  std::cout << "shad::unordered_set, size of " << kSize 
            << ", using " << shad::rt::numLocalities() 
            << " localities, running each shad STL algorithm for " 
            << repetitions << " times: \n";


  // shad count algorithm 
  // using distributed_sequential_tag
  {
    SetUp(set_);
    auto execute_time = shad::measure<std::chrono::seconds>::duration(
      [&]() {shad_count_algorithm(shad::distributed_sequential_tag{}, set_);});
    std::cout << "shad::count with sequential policy takes " 
              << (execute_time.count()/repetitions) << " seconds \n"; 
  }

  // using distributed_parallel_tag
  {
    SetUp(set_);
    auto execute_time = shad::measure<std::chrono::seconds>::duration(
      [&]() {shad_count_algorithm(shad::distributed_parallel_tag{}, set_);});
    std::cout << "shad::count with parallel policy takes " 
              << (execute_time.count()/repetitions) << " seconds \n"; 
  }

    // shad minmax algorithm 
  // using distributed_sequential_tag
  {
    SetUp(set_);
    auto execute_time = shad::measure<std::chrono::seconds>::duration(
      [&]() {shad_minmax_algorithm(shad::distributed_sequential_tag{}, set_);});
    std::cout << "shad::minmax_element with sequential policy takes " 
              << (execute_time.count()/repetitions) << " seconds \n"; 
  }

  // using distributed_parallel_tag
  {
    SetUp(set_);
    auto execute_time = shad::measure<std::chrono::seconds>::duration(
      [&]() {shad_minmax_algorithm(shad::distributed_parallel_tag{}, set_);});
    std::cout << "shad::minmax_element with parallel policy takes " 
              << (execute_time.count()/repetitions) << " seconds \n"; 
  }


  // shad transform algorithm 
  // using insert iterator with distributed_sequential_tag
  {
    SetUp(set_);
    auto execute_time = shad::measure<std::chrono::seconds>::duration(
      [&]() {shad_transform_algorithm<shad::distributed_sequential_tag, 
             shad_inserter_t>(
                shad::distributed_sequential_tag{}, set_);});
    std::cout << "shad::transform using insert iterator with sequential policy"
              << "takes " <<(execute_time.count()/repetitions) << " seconds \n"; 
  }

  // using insert iterator with distributed_parallel_tag
  {
    SetUp(set_);
    auto execute_time = shad::measure<std::chrono::seconds>::duration(
      [&]() {shad_transform_algorithm<shad::distributed_parallel_tag, 
             shad_inserter_t>(
                shad::distributed_parallel_tag{}, set_);});
    std::cout << "shad::transform using insert iterator with parallel policy"
              << "takes " <<(execute_time.count()/repetitions) << " seconds \n"; 
  }


  return 0;
}

}  // namespace shad