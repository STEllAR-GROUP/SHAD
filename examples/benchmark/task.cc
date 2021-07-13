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

#include <assert.h>
#include <chrono>
#include <iostream>
#include <vector>

#include "shad/runtime/runtime.h"

constexpr int repetitions = 1;

// set up
struct exData {
 size_t counter;
 shad::rt::Locality locality;
 size_t extra;
 void reset() {
   counter = 0;
   locality = shad::rt::Locality();
 }
};

static exData globalData = {0, shad::rt::Locality(), 0ul};
static const unsigned kNumIters = 1000000;
static const size_t kValue = 3;

static void incrFun(const exData &data) {
  globalData.counter += data.counter;
  globalData.locality = data.locality;
};

static void asyncIncrFun(shad::rt::Handle & /*unused*/, const exData &data) {
  __sync_fetch_and_add(&globalData.counter, data.counter);
  globalData.locality = data.locality;
};

namespace shad {

int main(int argc, char *argv[]) {

  std::cout << "task measurement, number of iterations " << kNumIters 
            <<", using " << shad::rt::numLocalities() 
            << " localities, running each test for " 
            << repetitions << " times, and take average: \n";



  //////////////////////////////////////////////////////////////////////
  // AsyncExecuteAt
  // warm up loop
  for(int i = 0; i < 5; ++i)
  {    
    for (auto loc : shad::rt::allLocalities()) {
      size_t value = kValue + static_cast<uint32_t>(loc);
      exData data = {value, loc};
      for (size_t i = 0; i < kNumIters; i++) {
        shad::rt::executeAt(loc, incrFun, data);
      }
    }
  }

  // timing loop
  auto start = std::chrono::steady_clock::now();
  { 
    shad::rt::Handle handle;
    std::vector<exData> argv(shad::rt::numLocalities());
    for (auto loc : shad::rt::allLocalities()) {
      size_t value = kValue + static_cast<uint32_t>(loc);
      exData data = {value, loc};
      argv[static_cast<uint32_t>(loc)] = data;
      for (size_t i = 0; i < kNumIters; i++) {
        shad::rt::asyncExecuteAt(handle, loc, asyncIncrFun,
                                 argv[static_cast<uint32_t>(loc)]);
      }
    }
    shad::rt::waitForCompletion(handle);
  }
  std::chrono::duration<double, std::chrono::seconds::period> duration = 
      std::chrono::steady_clock::now() - start;

  std::cout << "AsyncExecuteAt takes " << (duration.count()/repetitions) 
            << " seconds \n"; 

  

  return 0;
}

}  // namespace shad