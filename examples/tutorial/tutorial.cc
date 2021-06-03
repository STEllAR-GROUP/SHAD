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

#include "shad/core/array.h"
#include "shad/core/algorithm.h"
//#include "shad/core/numeric.h"
//#include "shad/core/execution.h"


namespace shad {


int main(int argc, char *argv[]) {

  constexpr static size_t kArraySize = 10000;
  shad::array<uint64_t, kArraySize> array_;

  // time how long it takes to fill them using shad::fill
  {
    const auto startTime = std::chrono::high_resolution_clock::now();
    shad::fill(
        shad::distributed_parallel_tag{},
        array_.begin(), array_.end(), 42);
    const auto endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = endTime - startTime;
    std::cout << "shad::fill time: " << diff.count() << std::endl;
  }
  
  // time how long it takes to fill them using shad::array::fill
  {
    const auto startTime = std::chrono::high_resolution_clock::now();
    array_.fill(16);
    const auto endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = endTime - startTime;
    std::cout << "shad::array::fill time: " << diff.count() << std::endl;
  }

  shad::array<uint64_t, kArraySize> arrayTrans; 
  // time how long it takes to transform them using shad::transform
  {
    const auto startTime = std::chrono::high_resolution_clock::now();
    shad::transform(
        shad::distributed_parallel_tag{},
        array_.begin(), array_.end(), arrayTrans.begin(), 
        [=](int i){ return i*2; });
    const auto endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = endTime - startTime;
    std::cout << "shad::transform time: " << diff.count() << std::endl;
  }

  //for(auto v:arrayTrans){
  //  std::cout << v << " ";
  //}
  //std::cout << " " << std::endl;


  return 0;
}

} //namespace shad
