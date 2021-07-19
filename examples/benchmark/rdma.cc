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
static const size_t _n_elements = 10000000; 
struct myelement_t {
  uint8_t first;
  uint16_t second;
  uint32_t third;
};
std::vector<myelement_t> remoteData(_n_elements, {0, 0, 0});

namespace shad {

int main(int argc, char *argv[]) {

  std::cout << "rdma measurement, " 
            << "number of element is: " << _n_elements 
            << ", using " << shad::rt::numLocalities() 
            << " localities, running each test for " 
            << repetitions << " times, and take average: \n";


  //////////////////////////////////////////////////////////////////////
  
  // warm up loop
  for(int i = 0; i < 5; ++i)
  {    
    for (auto loc : shad::rt::allLocalities()) {
      myelement_t* raddress;
      shad::rt::executeAtWithRet(
          loc,
          [](const size_t &, myelement_t**addr) {
             *addr = remoteData.data();
          },
          size_t(0), &raddress);
    }
  }

    /////////////////////////////////////////////////////////////////////
  // dma
  {   
    auto start = std::chrono::steady_clock::now();
    
    myelement_t val{8, 24, 42};
    std::vector<myelement_t>localData(_n_elements, val);
    for (auto loc : shad::rt::allLocalities()) {
      myelement_t* raddress;
      shad::rt::executeAtWithRet(
          loc,
          [](const size_t &, myelement_t**addr) {
             *addr = remoteData.data();
          },
          size_t(0), &raddress);
      shad::rt::dma(loc, raddress, localData.data(), _n_elements);
      std::tuple<size_t, size_t, size_t> acc(0, 0, 0);
      shad::rt::executeAtWithRet(
          loc,
          [](const size_t &, std::tuple<size_t, size_t, size_t>*addr) {
            size_t acc1(0), acc2(0), acc3(0);
            for (auto el : remoteData){
              acc1 += el.first;
              acc2 += el.second;
              acc3 += el.third;
            }
            std::tuple<size_t, size_t, size_t> acc(acc1, acc2, acc3);
            *addr = acc;
          },
          size_t(0), &acc);
      localData = std::vector<myelement_t>(_n_elements, {0, 0, 0});
      shad::rt::dma(localData.data(), loc, raddress, _n_elements);
      size_t acc1(0), acc2(0), acc3(0);
      for (auto el : localData){
        acc1 += el.first;
        acc2 += el.second;
        acc3 += el.third;
      }
    }
    
    std::chrono::duration<double, std::chrono::seconds::period> duration = 
      std::chrono::steady_clock::now() - start;

    std::cout << "dma takes " << (duration.count()/repetitions) 
            << " seconds \n"; 
  }

  
  /////////////////////////////////////////////////////////////////////
  // async_put_sync_get
  {   

    auto start = std::chrono::steady_clock::now();
    
    myelement_t val{8, 24, 42};
    std::vector<myelement_t>localData(_n_elements, val);
    std::vector<myelement_t*> raddresses(shad::rt::numLocalities());
    shad::rt::Handle handle;
    for (auto loc : shad::rt::allLocalities()) {
      shad::rt::executeAtWithRet(
          loc,
          [](const size_t &, myelement_t**addr) {
             *addr = remoteData.data();
          },
          size_t(0), &(raddresses[static_cast<uint32_t>(loc)]));
      
      shad::rt::asyncDma(handle, loc, raddresses[static_cast<uint32_t>(loc)],
                         localData.data(), _n_elements);
    }
    shad::rt::waitForCompletion(handle);
    for (auto loc : shad::rt::allLocalities()) {
      std::tuple<size_t, size_t, size_t> acc(0, 0, 0);
      shad::rt::executeAtWithRet(
          loc,
          [](const size_t &, std::tuple<size_t, size_t, size_t>*addr) {
            size_t acc1(0), acc2(0), acc3(0);
            for (auto el : remoteData){
              acc1 += el.first;
              acc2 += el.second;
              acc3 += el.third;
            }
            std::tuple<size_t, size_t, size_t> acc(acc1, acc2, acc3);
            *addr = acc;
          },
          size_t(0), &acc);
      
      localData = std::vector<myelement_t>(_n_elements, {0, 0, 0});
      shad::rt::dma(localData.data(), loc,
                    raddresses[static_cast<uint32_t>(loc)], _n_elements);
      size_t acc1(0), acc2(0), acc3(0);
      for (auto el : localData) {
        acc1 += el.first;
        acc2 += el.second;
        acc3 += el.third;
      }
    }
    
    std::chrono::duration<double, std::chrono::seconds::period> duration = 
      std::chrono::steady_clock::now() - start;

    std::cout << "async_put_sync_get takes " << (duration.count()/repetitions) 
            << " seconds \n"; 
  }
  
            
  //////////////////////////////////////////////////////////////////////
  // async_put_async_get

  {
    auto start = std::chrono::steady_clock::now(); 
    
    myelement_t val{8, 24, 42};
    std::vector<myelement_t>localData(_n_elements, val);
    std::vector<myelement_t*> raddresses(shad::rt::numLocalities());
    shad::rt::Handle handle;
    for (auto loc : shad::rt::allLocalities()) {
      shad::rt::executeAtWithRet(
          loc,
          [](const size_t &, myelement_t**addr) {
             *addr = remoteData.data();
          },
          size_t(0), &(raddresses[static_cast<uint32_t>(loc)]));
      
      shad::rt::asyncDma(handle, loc, raddresses[static_cast<uint32_t>(loc)],
                         localData.data(), _n_elements);
    }
    shad::rt::waitForCompletion(handle);
    std::vector<std::vector<myelement_t>>getData(shad::rt::numLocalities(),
                                                 std::vector<myelement_t>(
                                                     _n_elements, val));
    for (auto loc : shad::rt::allLocalities()) {
      const myelement_t* laddr = getData[static_cast<uint32_t>(loc)].data();
      shad::rt::asyncDma(handle, laddr, loc,
                         raddresses[static_cast<uint32_t>(loc)], _n_elements);
    }
    shad::rt::waitForCompletion(handle);
    for (auto loc : shad::rt::allLocalities()) {
      size_t acc1(0), acc2(0), acc3(0);
      for (auto el : getData[static_cast<uint32_t>(loc)]) {
        acc1 += el.first;
        acc2 += el.second;
        acc3 += el.third;
      }
    }

    std::chrono::duration<double, std::chrono::seconds::period> duration = 
      std::chrono::steady_clock::now() - start;

    std::cout << "async_put_async_get takes " << (duration.count()/repetitions) 
              << " seconds \n"; 
  }
  
  return 0;
}

}  // namespace shad