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

constexpr int repetitions = 2;
constexpr static size_t kArraySize = 1000000;
using array_t = shad::impl::array<int, kArraySize>;
using iterator = array_t::iterator;

void SetUp(shad::array<int, kArraySize> &in) {
    for (size_t i = 0; i < kArraySize; i++) {
      in[i] = i + 1;
    }
}

template <typename ExecutionPolicy>
void shad_fill_algorithm(ExecutionPolicy &&policy, 
                         shad::array<int, kArraySize> &in) {
  for(int i = 0; i < repetitions; ++i)
  {
    shad::fill(std::forward<ExecutionPolicy>(policy), in.begin(), in.end(), 42);
  }
}

template <typename ExecutionPolicy>
void shad_generate_algorithm(ExecutionPolicy &&policy,
                             shad::array<int, kArraySize> &in) {
  for(int i = 0; i < repetitions; ++i)
  {
    shad::generate(std::forward<ExecutionPolicy>(policy), in.begin(), in.end(), 
                   [=]() {
                     std::random_device rd;
                     std::default_random_engine G(rd());
                     std::uniform_int_distribution<int> dist(1, 10);
                     return dist(G);
                   });
  }
}

template <typename ExecutionPolicy>
void shad_count_algorithm(ExecutionPolicy &&policy,
                          shad::array<int, kArraySize> &in) {
  for(int i = 0; i < repetitions; ++i)
  {
    shad::count(std::forward<ExecutionPolicy>(policy), in.begin(), in.end(), 6);
  }
}


template <typename ExecutionPolicy>
void shad_for_each_algorithm(ExecutionPolicy &&policy,
                            shad::array<int, kArraySize> &in) {
  for(int i = 0; i < repetitions; ++i)
  {
    shad::for_each(std::forward<ExecutionPolicy>(policy), in.begin(), in.end(),
                   [](int &i) { i++; });
  }
}

template <typename ExecutionPolicy>
void shad_minmax_algorithm(ExecutionPolicy &&policy,
                            shad::array<int, kArraySize> &in) {
  for(int i = 0; i < repetitions; ++i)
  {
    shad::minmax_element(std::forward<ExecutionPolicy>(policy), in.begin(),
                         in.end());
  }
}

template <typename ExecutionPolicy>
void shad_transform_algorithm(ExecutionPolicy &&policy,
                            shad::array<int, kArraySize> &in) {
  for(int i = 0; i < repetitions; ++i)
  {
    shad::transform(std::forward<ExecutionPolicy>(policy), in.begin(), in.end(),
                    in.begin(), [](int i) { return i + 2; });
  }
}

namespace shad {

int main(int argc, char *argv[]) {

  shad::array<int, kArraySize> in;

  std::cout << "shad::array, size of " << kArraySize 
            << ", using " << shad::rt::numLocalities() 
            << " localities, running each shad STL algorithm for " 
            << repetitions << " times: \n";

  // shad fill algorithm 
  // using distributed_sequential_tag
  {
    SetUp(in);
    auto execute_time = shad::measure<std::chrono::seconds>::duration(
      [&]() {shad_fill_algorithm(shad::distributed_sequential_tag{}, in);});
    std::cout << "shad::fill with sequential policy takes " 
              << (execute_time.count()/repetitions) << " seconds \n";
  }

  // using distributed_parallel_tag
  {
    SetUp(in);
    auto execute_time = shad::measure<std::chrono::seconds>::duration(
      [&]() {shad_fill_algorithm(shad::distributed_parallel_tag{}, in);});
    std::cout << "shad::fill with parallel policy takes " 
              << (execute_time.count()/repetitions) << " seconds \n"; 
  }


  // shad generate algorithm 
  // using distributed_sequential_tag
  {
    SetUp(in);
    auto execute_time = shad::measure<std::chrono::seconds>::duration(
      [&]() {shad_generate_algorithm(shad::distributed_sequential_tag{}, in);});
    std::cout << "shad::generate with sequential policy takes "
              << (execute_time.count()/repetitions) << " seconds \n"; 
  }

  // using distributed_parallel_tag
  {
    SetUp(in);
    auto execute_time = shad::measure<std::chrono::seconds>::duration(
      [&]() {shad_generate_algorithm(shad::distributed_parallel_tag{}, in);});
    std::cout << "shad::generate with parallel policy takes "
             << (execute_time.count()/repetitions) << " seconds \n"; 
  }


  // shad count algorithm 
  // using distributed_sequential_tag
  {
    SetUp(in);
    auto execute_time = shad::measure<std::chrono::seconds>::duration(
      [&]() {shad_count_algorithm(shad::distributed_sequential_tag{}, in);});
    std::cout << "shad::count with sequential policy takes " 
              << (execute_time.count()/repetitions) << " seconds \n"; 
  }

  // using distributed_parallel_tag
  {
    SetUp(in);
    auto execute_time = shad::measure<std::chrono::seconds>::duration(
      [&]() {shad_count_algorithm(shad::distributed_parallel_tag{}, in);});
    std::cout << "shad::count with parallel policy takes " 
              << (execute_time.count()/repetitions) << " seconds \n"; 
  }

  // shad for_each algorithm 
  // using distributed_sequential_tag
  {
    SetUp(in);
    auto execute_time = shad::measure<std::chrono::seconds>::duration(
      [&]() {shad_for_each_algorithm(shad::distributed_sequential_tag{}, in);});
    std::cout << "shad::for_each with sequential policy takes " 
              << (execute_time.count()/repetitions) << " seconds \n"; 
  }

  // using distributed_parallel_tag
  {
    SetUp(in);
    auto execute_time = shad::measure<std::chrono::seconds>::duration(
      [&]() {shad_for_each_algorithm(shad::distributed_parallel_tag{}, in);});
    std::cout << "shad::for_each with parallel policy takes " 
              << (execute_time.count()/repetitions) << " seconds \n"; 
  }

  // shad minmax algorithm 
  // using distributed_sequential_tag
  {
    SetUp(in);
    auto execute_time = shad::measure<std::chrono::seconds>::duration(
      [&]() {shad_minmax_algorithm(shad::distributed_sequential_tag{}, in);});
    std::cout << "shad::transform with sequential policy takes " 
              << (execute_time.count()/repetitions) << " seconds \n"; 
  }

  // using distributed_parallel_tag
  {
    SetUp(in);
    auto execute_time = shad::measure<std::chrono::seconds>::duration(
      [&]() {shad_minmax_algorithm(shad::distributed_parallel_tag{}, in);});
    std::cout << "shad::transform with parallel policy takes " 
              << (execute_time.count()/repetitions) << " seconds \n"; 
  }

  // shad transform algorithm 
  // using distributed_sequential_tag
  {
    SetUp(in);
    auto execute_time = shad::measure<std::chrono::seconds>::duration(
      [&]() {shad_transform_algorithm(shad::distributed_sequential_tag{},in);});
    std::cout << "shad::transform with sequential policy takes " 
              << (execute_time.count()/repetitions) << " seconds \n"; 
  }

  // using distributed_parallel_tag
  {
    SetUp(in);
    auto execute_time = shad::measure<std::chrono::seconds>::duration(
      [&]() {shad_transform_algorithm(shad::distributed_parallel_tag{}, in);});
    std::cout << "shad::transform with parallel policy takes " 
              << (execute_time.count()/repetitions) << " seconds \n"; 
  }


  return 0;
}

}  // namespace shad