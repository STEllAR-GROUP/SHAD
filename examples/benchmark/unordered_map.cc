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

#include "shad/core/algorithm.h"
#include "shad/core/unordered_map.h"
#include "shad/util/measure.h"

constexpr int repetitions = 2;
constexpr static size_t kSize = 1000000;

using hashmap_t =
    shad::Hashmap<int, int, shad::MemCmp<int>, shad::Updater<int>>;
using iterator = hashmap_t::iterator;
using value_type = hashmap_t::value_type;
using shad_inserter_t = shad::insert_iterator<shad::unordered_map<int, int>>;
using shad_buffered_inserter_t =
    shad::buffered_insert_iterator<shad::unordered_map<int, int>>;

void SetUp(shad::unordered_map<int, int> &in) {
    shad_buffered_inserter_t ins(in, in.begin());

    for (size_t i = 0; i < kSize; ++i) {
      ins = std::make_pair(i, 3 * (i + 1));
    }
    ins.wait();
    ins.flush();
}


template <typename ExecutionPolicy>
void shad_count_algorithm(ExecutionPolicy &&policy,
                          shad::unordered_map<int, int> &in) {
  for(int i = 0; i < repetitions; ++i)
  {
    shad::count(std::forward<ExecutionPolicy>(policy), in.begin(), in.end(),
                std::make_pair(0, 1));
  }
}

template <typename ExecutionPolicy>
void shad_count_if_algorithm(ExecutionPolicy &&policy,
                             shad::unordered_map<int, int> &in) {
  for(int i = 0; i < repetitions; ++i)
  {
    shad::count_if(std::forward<ExecutionPolicy>(policy), in.begin(), in.end(),
                   [](const value_type &i) { return i.second % 4 == 0; });
  }
}


template <typename ExecutionPolicy>
void shad_minmax_algorithm(ExecutionPolicy &&policy,
                           shad::unordered_map<int, int> &in) {
  for(int i = 0; i < repetitions; ++i)
  {
    shad::minmax_element(std::forward<ExecutionPolicy>(policy), in.begin(),
                         in.end());
  }
}

template <typename ExecutionPolicy, typename shad_inserter>
void shad_transform_algorithm(ExecutionPolicy &&policy,
                              shad::unordered_map<int, int> &in) {
  for(int i = 0; i < repetitions; ++i)
  {
    shad::transform(std::forward<ExecutionPolicy>(policy), in.begin(), in.end(),
                    shad_inserter(in, in.begin()),
                    [](const value_type &i) { return i; });
  }
}

namespace shad {

int main(int argc, char *argv[]) {

  shad::unordered_map<int, int> map_;

  std::cout << "shad::unordered_map, size of " << kSize 
            << ", using " << shad::rt::numLocalities() 
            << " localities, running each shad STL algorithm for " 
            << repetitions << " times: \n";


  // shad count algorithm 
  // using distributed_sequential_tag
  {
    SetUp(map_);
    auto execute_time = shad::measure<std::chrono::seconds>::duration(
      [&]() {shad_count_algorithm(shad::distributed_sequential_tag{}, map_);});
    std::cout << "shad::count with sequential policy takes " 
              << (execute_time.count()/repetitions) << " seconds \n"; 
  }

  // using distributed_parallel_tag
  {
    SetUp(map_);
    auto execute_time = shad::measure<std::chrono::seconds>::duration(
      [&]() {shad_count_algorithm(shad::distributed_parallel_tag{}, map_);});
    std::cout << "shad::count with parallel policy takes " 
              << (execute_time.count()/repetitions) << " seconds \n"; 
  }


    // shad count_if algorithm 
  // using distributed_sequential_tag
  {
    SetUp(map_);
    auto execute_time = shad::measure<std::chrono::seconds>::duration(
      [&]() {shad_count_if_algorithm(shad::distributed_sequential_tag{}, map_);});
    std::cout << "shad::count_if with sequential policy takes " 
              << (execute_time.count()/repetitions) << " seconds \n"; 
  }

  // using distributed_parallel_tag
  {
    SetUp(map_);
    auto execute_time = shad::measure<std::chrono::seconds>::duration(
      [&]() {shad_count_if_algorithm(shad::distributed_parallel_tag{}, map_);});
    std::cout << "shad::count_if with parallel policy takes " 
              << (execute_time.count()/repetitions) << " seconds \n"; 
  }

    // shad minmax algorithm 
  // using distributed_sequential_tag
  {
    SetUp(map_);
    auto execute_time = shad::measure<std::chrono::seconds>::duration(
      [&]() {shad_minmax_algorithm(shad::distributed_sequential_tag{}, map_);});
    std::cout << "shad::minmax_element with sequential policy takes " 
              << (execute_time.count()/repetitions) << " seconds \n"; 
  }

  // using distributed_parallel_tag
  {
    SetUp(map_);
    auto execute_time = shad::measure<std::chrono::seconds>::duration(
      [&]() {shad_minmax_algorithm(shad::distributed_parallel_tag{}, map_);});
    std::cout << "shad::minmax_element with parallel policy takes " 
              << (execute_time.count()/repetitions) << " seconds \n"; 
  }


  // shad transform algorithm 
  // using insert iterator with distributed_sequetest_with_policy
  {
    SetUp(map_);
    auto execute_time = shad::measure<std::chrono::seconds>::duration(
      [&]() {shad_transform_algorithm<shad::distributed_sequential_tag, 
             shad_inserter_t>(
                shad::distributed_sequential_tag{}, map_);});
    std::cout << "shad::transform using insert iterator with sequential policy "
              << "takes " <<(execute_time.count()/repetitions) << " seconds \n"; 
  }

  // using insert iterator with distributed_parallel_tag
  {
    SetUp(map_);
    auto execute_time = shad::measure<std::chrono::seconds>::duration(
      [&]() {shad_transform_algorithm<shad::distributed_parallel_tag, 
             shad_inserter_t>( shad::distributed_parallel_tag{}, map_);});
    std::cout << "shad::transform using insert iterator with parallel policy "
              << "takes " <<(execute_time.count()/repetitions) << " seconds \n"; 
  }


  return 0;
}

}  // namespace shad