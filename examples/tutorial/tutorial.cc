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

namespace shad {

int main(int argc, char *argv[]) {
  //constexpr static size_t kArraySize = 4;
  //shad::array<int, kArraySize> in;

  //// shad generate algorithm
  //auto execute_time = shad::measure<std::chrono::nanoseconds>::duration(
  //  [&](){shad::detail::shad_generate(in);});
  //std::cout << "shad generate took " << execute_time.count() << std::endl;

  // shad generate algorithm

  ///////////////////////////////////////////////////////////////////////////
  // array
  
  // create array
  constexpr static size_t kArraySize = 4;

  using T = shad::array<int, kArraySize>;
  using iterator = shad::impl::array<int, kArraySize>::array_iterator<int>;

  auto arrayPtr = std::make_shared<T>();

  // array using shad::generate
  shad::generate(
    shad::distributed_parallel_tag{},
    arrayPtr->begin(), arrayPtr->end(), 
    [=]() {
        std::random_device rd;
        std::default_random_engine G(rd());
        std::uniform_int_distribution<int> dist(1, 10);
        return dist(G);
    }
  );

  for (iterator it = arrayPtr->begin(); it != arrayPtr->end(); ++it)
  {
    std::cout << *it << std::endl;
  }

  // array using shad::fill
  shad::fill(
    shad::distributed_parallel_tag{},
    arrayPtr->begin(), arrayPtr->end(), 42
  );

  for (iterator it = arrayPtr->begin(); it != arrayPtr->end(); ++it)
  {
    std::cout << *it << std::endl;
  }

  

  


  //for(uint32_t i = 0; i < rt::numLocalities(); ++i)
  //{
  //  auto t = tile(i, kArraySize, rt::numLocalities());
  //}
  //
  //std::cout << "loc: " << rt::thisLocality() << std::get<0>(t) << " " << std::get<1>(t) << std::endl;
  //
  //auto iter_begin = array_.begin() + std::get<0>(t);
  //auto iter_end = iter_begin + std::get<1>(t);

/***
  //// array using shad::generate
  //
  const auto startTime = std::chrono::high_resolution_clock::now();
  shad::generate(
      shad::distributed_parallel_tag{},
      iter_begin, iter_end,
      [=]() {
        //std::random_device rd;
        //std::default_random_engine G(rd());
        std::default_random_engine G(0);
        std::uniform_int_distribution<int> dist(1, 10);
        return dist(G);
      });
  const auto endTime = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> diff = endTime - startTime;
  std::cout << "array shad::generate time: " << diff.count() << std::endl;
 ***/ 
  //shad::unordered_set<int> set_;
  //set_.insert(42);
  //set_.insert(45);
/***
  // array using shad::fill
  {
    const auto startTime = std::chrono::high_resolution_clock::now();
    shad::fill(
        shad::distributed_parallel_tag{},
        array_.begin(), array_.end(), 42);
    const auto endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = endTime - startTime;
    std::cout << "array shad::fill time: " << diff.count() << std::endl;
  }
***/
  //for(auto v:array_){
  //
  //  std::cout << v << std::endl;
  //}

  //for(iterator it = array_.begin(); it != array_.end(); ++it){
  //  std::cout << *it << std::endl;
  //}


  //for(auto v:set_){
  //  std::cout << v << std::endl;
  //}
  

  /***
  // array using shad::transform
  shad::array<uint64_t, kArraySize> arrayTrans; 
  {
    const auto startTime = std::chrono::high_resolution_clock::now();
    shad::transform(
        shad::distributed_parallel_tag{},
        array_.begin(), array_.end(), arrayTrans.begin(), 
        [=](int i){ return i*2; });
    const auto endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = endTime - startTime;
    std::cout << "array shad::transform time: " << diff.count() << std::endl;
  }
  ***/

  


  return 0;
}

} //namespace shad
