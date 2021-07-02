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
void shad_fill_algorithm(ExecutionPolicy &&policy, 
                         shad::array<int, kArraySize> &in) {
  for(int i = 0; i < RUN_TIMES; ++i)
  {
    shad::fill(std::forward<ExecutionPolicy>(policy), in.begin(), in.end(), 42);
  }
}

template <typename ExecutionPolicy>
void shad_generate_algorithm(ExecutionPolicy &&policy,
                             shad::array<int, kArraySize> &in) {
  for(int i = 0; i < RUN_TIMES; ++i)
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
  for(int i = 0; i < RUN_TIMES; ++i)
  {
    shad::count(std::forward<ExecutionPolicy>(policy), in.begin(), in.end(), 6);
  }
}

template <typename ExecutionPolicy>
void shad_find_if_algorithm(ExecutionPolicy &&policy,
                            shad::array<int, kArraySize> &in) {
  for(int i = 0; i < RUN_TIMES; ++i)
  {
    shad::find_if(std::forward<ExecutionPolicy>(policy), in.begin(),
                  in.end(), [](int i) { return i % 2 == 0; });
  }
}

template <typename ExecutionPolicy>
void shad_for_each_algorithm(ExecutionPolicy &&policy,
                            shad::array<int, kArraySize> &in) {
  for(int i = 0; i < RUN_TIMES; ++i)
  {
    shad::for_each(std::forward<ExecutionPolicy>(policy), in.begin(), in.end(),
                   [](int &i) { i++; });
  }
}

template <typename ExecutionPolicy>
void shad_minmax_algorithm(ExecutionPolicy &&policy,
                            shad::array<int, kArraySize> &in) {
  for(int i = 0; i < RUN_TIMES; ++i)
  {
    shad::minmax_element(std::forward<ExecutionPolicy>(policy), in.begin(),
                         in.end());
  }
}

template <typename ExecutionPolicy>
void shad_transform_algorithm(ExecutionPolicy &&policy,
                            shad::array<int, kArraySize> &in) {
  for(int i = 0; i < RUN_TIMES; ++i)
  {
    shad::transform(std::forward<ExecutionPolicy>(policy), in.begin(), in.end(),
                    in.begin(), [](int i) { return i + 2; });
  }
}

namespace shad {

int main(int argc, char *argv[]) {
 // array
  shad::array<int, kArraySize> in;

  std::cout << "shad::array, using " << shad::rt::numLocalities() 
            << " localities, running each shad STL algorithm for " 
            << RUN_TIMES << " times: \n";

  // shad fill algorithm 
  // using distributed_sequential_tag
  {
    auto execute_time = shad::measure<std::chrono::seconds>::duration(
      [&]() {shad_fill_algorithm(shad::distributed_sequential_tag{},in);});
    std::cout << "shad::fill with sequential policy takes " 
              << execute_time.count() << " seconds \n";
  }

    // using distributed_parallel_tag
  {
    auto execute_time = shad::measure<std::chrono::seconds>::duration(
      [&]() {shad_fill_algorithm(shad::distributed_parallel_tag{},in);});
    std::cout << "shad::fill with parallel policy takes " 
              << execute_time.count() << " seconds \n"; 
  }


  // shad generate algorithm 
  // using distributed_sequential_tag
  {
    auto execute_time = shad::measure<std::chrono::seconds>::duration(
      [&]() {shad_generate_algorithm(shad::distributed_sequential_tag{},in);});
    std::cout << "shad::generate with sequential policy takes "
              << execute_time.count() << " seconds \n"; 
  }

  // using distributed_parallel_tag
  {
    auto execute_time = shad::measure<std::chrono::seconds>::duration(
      [&]() {shad_generate_algorithm(shad::distributed_parallel_tag{}, in);});
    std::cout << "shad::generate with parallel policy takes "
             << execute_time.count() << " seconds \n"; 
  }


  // shad count algorithm 
  // using distributed_sequential_tag
  {
    auto execute_time = shad::measure<std::chrono::seconds>::duration(
      [&]() {shad_count_algorithm(shad::distributed_sequential_tag{},in);});
    std::cout << "shad::count with sequential policy takes " 
              << execute_time.count() << " seconds \n"; 
  }

  // using distributed_parallel_tag
  {
    auto execute_time = shad::measure<std::chrono::seconds>::duration(
      [&]() {shad_count_algorithm(shad::distributed_parallel_tag{}, in);});
    std::cout << "shad::count with parallel policy takes " 
              << execute_time.count() << " seconds \n"; 
  }


  // shad find_if algorithm 
  // using distributed_sequential_tag
  {
    auto execute_time = shad::measure<std::chrono::seconds>::duration(
      [&]() {shad_find_if_algorithm(shad::distributed_sequential_tag{},in);});
    std::cout << "shad::find_if with sequential policy takes " 
              << execute_time.count() << " seconds \n"; 
  }

  // using distributed_parallel_tag
  {
    auto execute_time = shad::measure<std::chrono::seconds>::duration(
      [&]() {shad_find_if_algorithm(shad::distributed_parallel_tag{}, in);});
    std::cout << "shad::find_if with parallel policy takes " 
              << execute_time.count() << " seconds \n"; 
  }

  // shad for_each algorithm 
  // using distributed_sequential_tag
  {
    auto execute_time = shad::measure<std::chrono::seconds>::duration(
      [&]() {shad_for_each_algorithm(shad::distributed_sequential_tag{},in);});
    std::cout << "shad::for_each with sequential policy takes " 
              << execute_time.count() << " seconds \n"; 
  }

  // using distributed_parallel_tag
  {
    auto execute_time = shad::measure<std::chrono::seconds>::duration(
      [&]() {shad_for_each_algorithm(shad::distributed_parallel_tag{}, in);});
    std::cout << "shad::for_each with parallel policy takes " 
              << execute_time.count() << " seconds \n"; 
  }

  // shad transform algorithm 
  // using distributed_sequential_tag
  {
    auto execute_time = shad::measure<std::chrono::seconds>::duration(
      [&]() {shad_transform_algorithm(shad::distributed_sequential_tag{},in);});
    std::cout << "shad::transform with sequential policy takes " 
              << execute_time.count() << " seconds \n"; 
  }

  // using distributed_parallel_tag
  {
    auto execute_time = shad::measure<std::chrono::seconds>::duration(
      [&]() {shad_transform_algorithm(shad::distributed_parallel_tag{}, in);});
    std::cout << "shad::transform with parallel policy takes " 
              << execute_time.count() << " seconds \n"; 
  }



  return 0;
}

}  // namespace shad