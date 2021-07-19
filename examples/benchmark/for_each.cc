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
#include <iostream>

#include "shad/runtime/runtime.h"

constexpr int repetitions = 1;

// set up
static std::atomic<int> Counter(0);

struct TestStruct {
  int valueA;
  int valueB;
};

namespace shad {

int main(int argc, char *argv[]) {

  std::cout << "rdma measurement, " 
            << "using " << shad::rt::numLocalities() 
            << " localities, running each test for " 
            << repetitions << " times, and take average: \n";


  //////////////////////////////////////////////////////////////////////
  
  // warm up loop
  for(int i = 0; i < 5; ++i)
  {    
    shad::rt::forEachOnAll(
    [](const TestStruct &args, size_t i) {
      Counter++;
    },
    TestStruct{5, 5},
    shad::rt::numLocalities() * shad::rt::impl::getConcurrency());
  }

    /////////////////////////////////////////////////////////////////////
  // ForEachOnAll
  { 
    for (auto &loc : shad::rt::allLocalities()) {
      shad::rt::executeAt(loc, [](const bool &) { Counter = 0; }, false);
    }
    auto start = std::chrono::steady_clock::now();

    shad::rt::forEachOnAll(
      [](const TestStruct &args, size_t i) {
        Counter++;
      },
      TestStruct{5, 5},
      shad::rt::numLocalities() * shad::rt::impl::getConcurrency());
    
    std::chrono::duration<double, std::chrono::seconds::period> duration = 
      std::chrono::steady_clock::now() - start;

    std::cout << "ForEachOnAll takes " << (duration.count()/repetitions) 
            << " seconds \n"; 
  }

  
  /////////////////////////////////////////////////////////////////////
  // AsyncForEachOnAll
  {   
    for (auto &loc : shad::rt::allLocalities()) {
      shad::rt::executeAt(loc, [](const bool &) { Counter = 0; }, false);
    }

    auto start = std::chrono::steady_clock::now();
    
    shad::rt::Handle handle;
    shad::rt::asyncForEachOnAll(
        handle,
        [](shad::rt::Handle &handle, const TestStruct &args, size_t i) {
          Counter++;
        },
        TestStruct{5, 5},
        shad::rt::numLocalities() * shad::rt::impl::getConcurrency());

    shad::rt::waitForCompletion(handle);
    
    std::chrono::duration<double, std::chrono::seconds::period> duration = 
      std::chrono::steady_clock::now() - start;

    std::cout << "AsyncForEachOnAll takes " << (duration.count()/repetitions) 
            << " seconds \n"; 
  }
  
            
  //////////////////////////////////////////////////////////////////////
  // ForEachOnAllWithBuffer

  {
    for (auto &loc : shad::rt::allLocalities()) {
      shad::rt::executeAt(loc, [](const bool &) { Counter = 0; }, false);
    }

    auto start = std::chrono::steady_clock::now(); 
    
    std::shared_ptr<uint8_t> buffer(new uint8_t[2]{5, 5},
                                  std::default_delete<uint8_t[]>());

    shad::rt::forEachOnAll(
      [](const uint8_t *input, const uint32_t size, size_t i) {
        Counter++;
      },
      buffer, 2, shad::rt::numLocalities() * shad::rt::impl::getConcurrency());
    

    std::chrono::duration<double, std::chrono::seconds::period> duration = 
      std::chrono::steady_clock::now() - start;

    std::cout << "ForEachOnAllWithBuffer takes " 
              << (duration.count()/repetitions) 
              << " seconds \n"; 
  }

  //////////////////////////////////////////////////////////////////////
  // AsyncForEachOnAllWithBuffer

  {
    for (auto &loc : shad::rt::allLocalities()) {
      shad::rt::executeAt(loc, [](const bool &) { Counter = 0; }, false);
    }
    auto start = std::chrono::steady_clock::now(); 

    std::shared_ptr<uint8_t> buffer(new uint8_t[2]{5, 5},
                                  std::default_delete<uint8_t[]>());
    {
      shad::rt::Handle handle;
      shad::rt::asyncForEachOnAll(
          handle,
          [](shad::rt::Handle &handle, const uint8_t *input, const uint32_t size,
             size_t i) {
            Counter++;
          },
          buffer, 2,
          shad::rt::numLocalities() * shad::rt::impl::getConcurrency());
  
      shad::rt::waitForCompletion(handle);
    }
    

    std::chrono::duration<double, std::chrono::seconds::period> duration = 
      std::chrono::steady_clock::now() - start;

    std::cout << "AsyncForEachOnAllWithBuffer takes " 
              << (duration.count()/repetitions) 
              << " seconds \n"; 
  }

  //////////////////////////////////////////////////////////////////////
  // ForEachAt

  {
    for (auto &loc : shad::rt::allLocalities()) {
      shad::rt::executeAt(loc, [](const bool &) { Counter = 0; }, false);
    }
    auto start = std::chrono::steady_clock::now(); 
    
    for (auto &locality : shad::rt::allLocalities()) {
    shad::rt::forEachAt(locality,
                        [](const TestStruct &args, size_t i) {
                          Counter++;
                        },
                        TestStruct{5, 5}, shad::rt::impl::getConcurrency());
    }

    std::chrono::duration<double, std::chrono::seconds::period> duration = 
      std::chrono::steady_clock::now() - start;

    std::cout << "ForEachAt takes " << (duration.count()/repetitions) 
              << " seconds \n"; 
  }

  //////////////////////////////////////////////////////////////////////
  // AsyncForEachAt

  {
    for (auto &loc : shad::rt::allLocalities()) {
      shad::rt::executeAt(loc, [](const bool &) { Counter = 0; }, false);
    }
    auto start = std::chrono::steady_clock::now(); 
    
    {
      shad::rt::Handle handle;
      for (auto &locality : shad::rt::allLocalities()) {
        shad::rt::asyncForEachAt(
            handle, locality,
            [](shad::rt::Handle &handle, const TestStruct &args, size_t i) {
              Counter++;
            },
            TestStruct{5, 5}, shad::rt::impl::getConcurrency());
      }
      shad::rt::waitForCompletion(handle);
    }

    std::chrono::duration<double, std::chrono::seconds::period> duration = 
      std::chrono::steady_clock::now() - start;

    std::cout << "AsyncForEachAt takes " << (duration.count()/repetitions) 
              << " seconds \n"; 
  }

  //////////////////////////////////////////////////////////////////////
  // ForEachAtWithBuffer

  {
    for (auto &loc : shad::rt::allLocalities()) {
      shad::rt::executeAt(loc, [](const bool &) { Counter = 0; }, false);
    }
    auto start = std::chrono::steady_clock::now(); 
    
    std::shared_ptr<uint8_t> buffer(new uint8_t[2]{5, 5},
                                  std::default_delete<uint8_t[]>());

    for (auto &locality : shad::rt::allLocalities()) {
      shad::rt::forEachAt(
          locality,
          [](const uint8_t *input, const uint32_t size, size_t i) {
            Counter++;
          },
          buffer, 2, shad::rt::impl::getConcurrency());
    }

    std::chrono::duration<double, std::chrono::seconds::period> duration = 
      std::chrono::steady_clock::now() - start;

    std::cout << "ForEachAtWithBuffer takes " << (duration.count()/repetitions) 
              << " seconds \n"; 
  }

  //////////////////////////////////////////////////////////////////////
  // AsyncForEachAtWithBuffer

  {
    for (auto &loc : shad::rt::allLocalities()) {
      shad::rt::executeAt(loc, [](const bool &) { Counter = 0; }, false);
    }
    auto start = std::chrono::steady_clock::now(); 
    
    std::shared_ptr<uint8_t> buffer(new uint8_t[2]{5, 5},
                                  std::default_delete<uint8_t[]>());

    {
      shad::rt::Handle handle;
  
      for (auto &locality : shad::rt::allLocalities()) {
        shad::rt::asyncForEachAt(
            handle, locality,
            [](shad::rt::Handle &handle, const uint8_t *input,
               const uint32_t size, size_t i) {
              Counter++;
            },
            buffer, 2, shad::rt::impl::getConcurrency());
      }
  
      shad::rt::waitForCompletion(handle);
    }

    std::chrono::duration<double, std::chrono::seconds::period> duration = 
      std::chrono::steady_clock::now() - start;

    std::cout << "AsyncForEachAtWithBuffer takes " << (duration.count()/repetitions) 
              << " seconds \n"; 
  }

  return 0;
}

}  // namespace shad