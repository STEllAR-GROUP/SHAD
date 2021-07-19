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
#include <cstring>
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
static const unsigned kNumIters = 100;
static const size_t kValue = 3;

static void executeAtFun(const exData &data) {
  globalData.counter += data.counter;
  globalData.locality = data.locality;
};

static void asyncExecuteAtFun(shad::rt::Handle & /*unused*/, const exData &data) {
  __sync_fetch_and_add(&globalData.counter, data.counter);
  globalData.locality = data.locality;
};

static void executeAtBuffFun(const uint8_t *argsBuffer,
                            const uint32_t /*bufferSize*/) {
  const exData data = *reinterpret_cast<const exData *>(argsBuffer);
  globalData.counter += data.counter;
  globalData.locality = data.locality;
};

static void asyncExecuteAtBuffFun(shad::rt::Handle & /*unused*/,
                                 const uint8_t *argsBuffer,
                                 const uint32_t /*bufferSize*/) {
  const exData data = *reinterpret_cast<const exData *>(argsBuffer);
  __sync_fetch_and_add(&globalData.counter, data.counter);

  globalData.locality = data.locality;
}

static void incrFunWithRetBuffExplicit(const uint8_t *argsBuffer,
                                       const uint32_t /*bufferSize*/,
                                       uint8_t *result, uint32_t *resSize) {
  const exData data = *reinterpret_cast<const exData *>(argsBuffer);
  globalData.counter += data.counter;
  globalData.locality = data.locality;
  *resSize = sizeof(globalData);
  memcpy(result, &globalData, *resSize);
};

static void asynIncrFunWithRetBuffExplicit(shad::rt::Handle & /*unused*/,
                                           const uint8_t *argsBuffer,
                                           const uint32_t /*bufferSize*/,
                                           uint8_t *result, uint32_t *resSize) {
  const exData &data = *reinterpret_cast<const exData *>(argsBuffer);
  __sync_fetch_and_add(&globalData.counter, data.counter);
  globalData.locality = data.locality;
  exData res{data.counter + 1, data.locality};

  *resSize = sizeof(res);
  *reinterpret_cast<exData *>(result) = res;
};

static void incrFunWithRet(const exData &data, exData *result) {
  globalData.counter += data.counter;
  globalData.locality = data.locality;
  *result = globalData;
};

namespace shad {

int main(int argc, char *argv[]) {

  std::cout << "execute at task measurement, number of iterations " << kNumIters 
            <<", using " << shad::rt::numLocalities() 
            << " localities, running each test for " 
            << repetitions << " times, and take average: \n";



  //////////////////////////////////////////////////////////////////////
  
  // warm up loop
  for(int i = 0; i < 5; ++i)
  {    
    for (auto loc : shad::rt::allLocalities()) {
      size_t value = kValue + static_cast<uint32_t>(loc);
      exData data = {value, loc};
      for (size_t i = 0; i < kNumIters; i++) {
        shad::rt::executeAt(loc, executeAtFun, data);
      }
    }
  }
  
  /////////////////////////////////////////////////////////////////////
  // executeAt
  {
    auto start = std::chrono::steady_clock::now();
    for (auto loc : shad::rt::allLocalities()) {
      size_t value = kValue + static_cast<uint32_t>(loc);
      exData data = {value, loc};
      for (size_t i = 0; i < kNumIters; i++) {
        shad::rt::executeAt(loc, executeAtFun, data);
      }
    }
    std::chrono::duration<double, std::chrono::seconds::period> duration = 
      std::chrono::steady_clock::now() - start;

    std::cout << "executeAt takes " << (duration.count()/repetitions) 
            << " seconds \n"; 
  }
  
            
  //////////////////////////////////////////////////////////////////////
  // AsyncExecuteAt

  {
    auto start = std::chrono::steady_clock::now(); 
    shad::rt::Handle handle;
    std::vector<exData> argv(shad::rt::numLocalities());
    for (auto loc : shad::rt::allLocalities()) {
      size_t value = kValue + static_cast<uint32_t>(loc);
      exData data = {value, loc};
      argv[static_cast<uint32_t>(loc)] = data;
      for (size_t i = 0; i < kNumIters; i++) {
        shad::rt::asyncExecuteAt(handle, loc, asyncExecuteAtFun,
                                 argv[static_cast<uint32_t>(loc)]);
      }
    }
    shad::rt::waitForCompletion(handle);
    std::chrono::duration<double, std::chrono::seconds::period> duration = 
      std::chrono::steady_clock::now() - start;

    std::cout << "AsyncExecuteAt takes " << (duration.count()/repetitions) 
              << " seconds \n"; 
  }
  

  //////////////////////////////////////////////////////////////////////
  // executeAt_buffer
  
  {
    auto start = std::chrono::steady_clock::now();
    for (auto loc : shad::rt::allLocalities()) {
      size_t value = kValue + static_cast<uint32_t>(loc);
      exData data = {value, loc};
      std::shared_ptr<uint8_t> argsBuffer(new uint8_t[sizeof(exData)],
                                          std::default_delete<uint8_t[]>());
  
      std::memcpy(argsBuffer.get(), &data, sizeof(exData));
  
      for (size_t i = 0; i < kNumIters; i++) {
        shad::rt::executeAt(loc, executeAtBuffFun, argsBuffer, sizeof(data));
      }
    }
    std::chrono::duration<double, std::chrono::seconds::period> duration = 
      std::chrono::steady_clock::now() - start;

    std::cout << "executeAt with buffer takes " 
              << (duration.count()/repetitions) 
              << " seconds \n";    
  }
  
  //////////////////////////////////////////////////////////////////////
  // asyncExecuteAt_buffer
  
  {
    auto start = std::chrono::steady_clock::now();
    shad::rt::Handle handle;
    for (auto &loc : shad::rt::allLocalities()) {
      size_t value = kValue + static_cast<uint32_t>(loc);
      exData data = {value, loc};
      std::shared_ptr<uint8_t> args(new uint8_t[sizeof(exData)],
                                    std::default_delete<uint8_t[]>());
  
      std::memcpy(args.get(), &data, sizeof(exData));
      for (size_t i = 0; i < kNumIters; i++) {
        shad::rt::asyncExecuteAt(handle, loc, asyncExecuteAtBuffFun, args,
                                 sizeof(exData));
      }
    }

    shad::rt::waitForCompletion(handle);
    std::chrono::duration<double, std::chrono::seconds::period> duration = 
      std::chrono::steady_clock::now() - start;

    std::cout << "asyncExecuteAt with buffer takes " 
              << (duration.count()/repetitions) 
              << " seconds \n"; 
  }
  
  //////////////////////////////////////////////////////////////////////
  // ExecuteAtWithRetBuff
  {
    auto start = std::chrono::steady_clock::now();
    for (auto loc : shad::rt::allLocalities()) {
      size_t value = kValue + static_cast<uint32_t>(loc);
      exData data = {value, loc};
  
      std::shared_ptr<uint8_t> argsBuffer(new uint8_t[sizeof(exData)],
                                          std::default_delete<uint8_t[]>());
      std::memcpy(argsBuffer.get(), &data, sizeof(exData));
  
      exData retData;
      uint32_t retSize;
      for (size_t i = 0; i < kNumIters; i++) {
        shad::rt::executeAtWithRetBuff(
            loc, incrFunWithRetBuffExplicit, argsBuffer, sizeof(data),
            reinterpret_cast<uint8_t *>(&retData), &retSize);
      }
    }
    std::chrono::duration<double, std::chrono::seconds::period> duration = 
        std::chrono::steady_clock::now() - start;
  
    std::cout << "executeAtWithRetBuff with buffer takes " 
              << (duration.count()/repetitions) 
              << " seconds \n"; 
  }
  //////////////////////////////////////////////////////////////////////
  // asyncExecuteAtWithRetBuff
  
  {
    auto start = std::chrono::steady_clock::now();
    std::vector<exData> retData(shad::rt::numLocalities() * kNumIters);
    std::vector<uint32_t> retSizes(shad::rt::numLocalities() * kNumIters, 0);
  
    shad::rt::Handle handle;
  
    for (auto &locality : shad::rt::allLocalities()) {
      uint32_t localityNumber = static_cast<uint32_t>(locality);
      size_t value = kValue + localityNumber;
      exData data = {value, locality};
      std::shared_ptr<uint8_t> argsBuffer(new uint8_t[sizeof(exData)],
                                          std::default_delete<uint8_t[]>());
      std::memcpy(argsBuffer.get(), &data, sizeof(exData));
  
      for (size_t i = 0; i < kNumIters; ++i) {
        size_t idx = i * shad::rt::numLocalities() + localityNumber;
        shad::rt::asyncExecuteAtWithRetBuff(
            handle, locality, asynIncrFunWithRetBuffExplicit, argsBuffer,
            sizeof(exData), reinterpret_cast<uint8_t *>(&retData[idx]),
            &retSizes[idx]);
      }
    }
  
    shad::rt::waitForCompletion(handle);
    std::chrono::duration<double, std::chrono::seconds::period> duration = 
      std::chrono::steady_clock::now() - start;

    std::cout << "asyncExecuteAtWithRetBuff with buffer takes " 
              << (duration.count()/repetitions) 
              << " seconds \n"; 
  }
  
  //////////////////////////////////////////////////////////////////////
  // executeAtWithRet
  
  {
    auto start = std::chrono::steady_clock::now();
    for (auto loc : shad::rt::allLocalities()) {
      size_t value = kValue + static_cast<uint32_t>(loc);
      exData data = {value, loc};
      exData retData;
      for (size_t i = 0; i < kNumIters; i++) {
        shad::rt::executeAtWithRet(loc, incrFunWithRet, data, &retData);
      }
    }
    std::chrono::duration<double, std::chrono::seconds::period> duration = 
      std::chrono::steady_clock::now() - start;

    std::cout << "executeAtWithRet takes " << (duration.count()/repetitions) 
              << " seconds \n"; 
  }
  
  //////////////////////////////////////////////////////////////////////
  // asyncExecuteAtWithRet
  
  {
    auto start = std::chrono::steady_clock::now();
    shad::rt::Handle handle;
    std::vector<exData> argv(shad::rt::numLocalities());
    for (auto &locality : shad::rt::allLocalities()) {
      std::vector<exData> retData(kNumIters);
      uint32_t localityNumber = static_cast<uint32_t>(locality);
      size_t value = kValue + localityNumber;
      exData data = {value, locality};
      argv[localityNumber] = data;
  
      for (size_t i = 0; i < kNumIters; ++i) {
        shad::rt::asyncExecuteAtWithRet(
            handle, locality,
            [](shad::rt::Handle &handle, const exData &data, exData *result) {
              globalData.locality = data.locality;
              __sync_fetch_and_add(&globalData.counter, data.counter);
              *result = data;
            },
            argv[localityNumber], &retData[i]);
      }
      shad::rt::waitForCompletion(handle);
    }
    std::chrono::duration<double, std::chrono::seconds::period> duration = 
      std::chrono::steady_clock::now() - start;

    std::cout << "asyncExecuteAtWithRet takes " 
              << (duration.count()/repetitions) 
              << " seconds \n"; 
  }
  
  return 0;
}

}  // namespace shad