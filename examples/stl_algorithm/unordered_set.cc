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
#include "shad/core/unordered_set.h"

constexpr static size_t kSize = 1;
using set_t = shad::Set<int>;
using iterator = set_t::iterator;
using value_type = set_t::value_type;
using shad_inserter_t = shad::insert_iterator<shad::unordered_set<int>>;
using shad_buffered_inserter_t =
    shad::buffered_insert_iterator<shad::unordered_set<int>>;


namespace shad {

int main(int argc, char *argv[]) {
  // unordered_set
  shad::unordered_set<int> set_;

  // create set
  for (size_t i = 0; i < kSize; ++i) {
    set_.insert(2 * (i + 1));
  }


  shad::unordered_set<int> out;
  shad::transform(
      shad::distributed_parallel_tag{}, set_.begin(), set_.end(),
      shad_buffered_inserter_t(out, out.begin()), [](const value_type &i) { return i; });
      //shad_inserter_t(out, out.begin()), [](const value_type &i) { return i; });
  
  std::cout << "output set: \n";
  for (auto v: out) std::cout << v << std::endl;

  return 0;
}

}  // namespace shad
