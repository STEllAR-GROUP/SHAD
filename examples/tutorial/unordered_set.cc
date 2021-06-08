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

constexpr static size_t kSize = 4;
using iterator = shad::Set<int>::iterator;
using shad_inserter_t = shad::insert_iterator<shad::unordered_set<int>>;
using shad_buffered_inserter_t = shad::buffered_insert_iterator<shad::unordered_set<int>>;

/***
void shad_generate_algorithm(shad::array<int, kSize> &in){
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

void shad_fill_algorithm(shad::array<int, kSize> &in){
  shad::fill(
    shad::distributed_parallel_tag{},
    in.begin(), in.end(), 42);
}

size_t shad_count_algorithm(shad::array<int, kSize> &in){
  auto counter = shad::count(
                    shad::distributed_parallel_tag{},
                    in.begin(), in.end(), 1);
  return counter;
}

iterator shad_find_if_algorithm(shad::array<int, kSize> &in){
  auto iter = shad::find_if(
                    shad::distributed_parallel_tag{},
                    in.begin(), in.end(),
                    [](int i){ return i%2 == 0;});
  return iter;
}

void shad_for_each_algorithm(shad::array<int, kSize> &in){
  shad::for_each(
    shad::distributed_parallel_tag{},
    in.begin(), in.end(),
    [](int& i){ i++; });
}
***/

std::pair<iterator, iterator> shad_minmax_algorithm(shad::unordered_set<int> &in){
  auto [min, max] = shad::minmax_element(
                    shad::distributed_parallel_tag{},
                    in.begin(), in.end());
  return {min, max};
}


void shad_transform_algorithm(shad::unordered_set<int> &in){
  shad::unordered_set<int> out;
  shad::transform(
                    shad::distributed_parallel_tag{},
                    in.begin(), in.end(), shad_inserter_t(out, out.begin()),
                    [](const std::unordered_set<int>::value_type &i){ return i;});
  //for(auto v : out) std::cout<< v << std::endl;
}

namespace shad {

int main(int argc, char *argv[]) {

  // unordered_set
  shad::unordered_set<int> set_;

  // create set
  shad_inserter_t ins(set_, set_.begin());
  for (size_t i = 0; i < kSize; ++i){
    ins = 2 * i;
  } 

  // shad minmax algorithm
  std::pair<iterator, iterator> min_max;
  auto execute_time = shad::measure<std::chrono::nanoseconds>::duration(
    [&](){ min_max = shad_minmax_algorithm(set_); });
  std::cout << "Unordered set, using " << shad::rt::numLocalities() 
            << " localities, shad::count took " 
            << execute_time.count() << " nanoseconds (min = "
            << *min_max.first<< ", max = " << *min_max.second << " )" << std::endl;

  // shad transform algorithm

  execute_time = shad::measure<std::chrono::nanoseconds>::duration(
    [&](){shad_transform_algorithm(set_);});
  std::cout << "Unordered set, using " << shad::rt::numLocalities() 
            << " localities, shad::transform took " 
            << execute_time.count() << " nanoseconds" << std::endl;

  return 0;
}

} //namespace shad
