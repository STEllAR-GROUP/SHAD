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

#include <atomic>
#include <chrono>
#include <cstring>
#include <iostream>
#include <vector>


#include "shad/runtime/runtime.h"

constexpr int repetitions = 1;

// set up
static std::atomic<int> Counter(0);

namespace shad {

int main(int argc, char *argv[]) {

  std::cout << "execute on all task measurement, " 
            << "using " << shad::rt::numLocalities() 
            << " localities, running each test for " 
            << repetitions << " times, and take average: \n";


  //////////////////////////////////////////////////////////////////////
  
  // warm up loop
  for(int i = 0; i < 20; ++i)
  {    
    shad::rt::executeOnAll([](const bool &) { Counter++; }, false);
  }

    /////////////////////////////////////////////////////////////////////
  // executeOnAll
  {   
    for (auto &loc : shad::rt::allLocalities()) {
      shad::rt::executeAt(loc, [](const bool &) { Counter = 0; }, false);
    }

    auto start = std::chrono::steady_clock::now();
    
    shad::rt::executeOnAll([](const bool &) { Counter++; }, false);
    
    std::chrono::duration<double, std::chrono::seconds::period> duration = 
      std::chrono::steady_clock::now() - start;

    std::cout << "executeOnAll takes " << (duration.count()/repetitions) 
            << " seconds \n"; 
  }

  
  /////////////////////////////////////////////////////////////////////
  // asyncExecuteOnAll
  {   
    for (auto &loc : shad::rt::allLocalities()) {
      shad::rt::executeAt(loc, [](const bool &) { Counter = 0; }, false);
    }

    auto start = std::chrono::steady_clock::now();
    
    shad::rt::Handle handle;
    shad::rt::asyncExecuteOnAll(
        handle, [](shad::rt::Handle &, const bool &) { Counter++; }, false);

    shad::rt::waitForCompletion(handle);
    
    std::chrono::duration<double, std::chrono::seconds::period> duration = 
      std::chrono::steady_clock::now() - start;

    std::cout << "asyncExecuteOnAll takes " << (duration.count()/repetitions) 
            << " seconds \n"; 
  }
  
            
  //////////////////////////////////////////////////////////////////////
  // executeOnAll_buffer

  {
    for (auto &loc : shad::rt::allLocalities()) {
      shad::rt::executeAt(loc, [](const bool &) { Counter = 0; }, false);
    }

    auto start = std::chrono::steady_clock::now(); 
    
    std::shared_ptr<uint8_t> buffer(new uint8_t[2]{5, 5},
                                  std::default_delete<uint8_t[]>());
    
    shad::rt::executeOnAll(
      [](const uint8_t *B, const uint32_t S) {
        Counter = B[0] + B[1];
      },
      buffer, sizeof(uint8_t) << 1);

    std::chrono::duration<double, std::chrono::seconds::period> duration = 
      std::chrono::steady_clock::now() - start;

    std::cout << "executeOnAll_buffer takes " << (duration.count()/repetitions) 
              << " seconds \n"; 
  }
  

  //////////////////////////////////////////////////////////////////////
  // asyncExecuteOnAll_buffer
  
  {
    for (auto &loc : shad::rt::allLocalities()) {
      shad::rt::executeAt(loc, [](const bool &) { Counter = 0; }, false);
    }

    auto start = std::chrono::steady_clock::now();

    shad::rt::Handle handle;

    std::shared_ptr<uint8_t> buffer(new uint8_t[2]{5, 5},
                                    std::default_delete<uint8_t[]>());

    shad::rt::asyncExecuteOnAll(
        handle,
        [](shad::rt::Handle &, const uint8_t *B, const uint32_t S) {
          Counter = B[0] + B[1];
        },
        buffer, 2);

    shad::rt::waitForCompletion(handle);

    std::chrono::duration<double, std::chrono::seconds::period> duration = 
      std::chrono::steady_clock::now() - start;

    std::cout << "asyncExecuteOnAll with buffer takes " 
              << (duration.count()/repetitions) 
              << " seconds \n";    
  }
  
  
  return 0;
}

}  // namespace shad