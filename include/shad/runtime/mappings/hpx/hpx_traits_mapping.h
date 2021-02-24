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
//===----------------------------------------------------------------------===//

#ifndef INCLUDE_SHAD_RUNTIME_MAPPINGS_HPX_HPX_TRAITS_MAPPING_H_
#define INCLUDE_SHAD_RUNTIME_MAPPINGS_HPX_HPX_TRAITS_MAPPING_H_

#include <cstdint>
#include <limits>
#include <memory>
#include <mutex>
#include <string>

#include "hpx/hpx.hpp"
#include "shad/runtime/mapping_traits.h"

namespace shad {

namespace rt {
namespace impl {

struct hpx_tag {};


struct HpxHandle {};

template <>
struct HandleTrait<hpx_tag> {
  using HandleTy = hpx::shared_future<void>;
  using ParameterTy = hpx::shared_future<void> &;
  using ConstParameterTy = const hpx::shared_future<void> &;

  static void Init(ParameterTy H, ConstParameterTy V) {
    H = V.then([](HandleTy&&){});
  }

  static HandleTy NullValue() { return hpx::shared_future<void>(); }

  static bool Equal(ConstParameterTy lhs, ConstParameterTy rhs) {
    return toUnsignedInt(lhs) == toUnsignedInt(rhs);
  }

  static std::string toString(ConstParameterTy H) {
    return std::to_string(toUnsignedInt(H));
    }

  static uint64_t toUnsignedInt(ConstParameterTy H) {
    return reinterpret_cast<uint64_t>(hpx::traits::detail::get_shared_state(H)
                                      .get());
  }

  static HandleTy CreateNewHandle() {
    return hpx::shared_future<void>();
  }

  // alternative implementation:
  //static HandleTy CreateNewHandle() {
  //  return hpx::make_ready_future();
  //}

  static void WaitFor(ParameterTy H) { if (H.valid()) H.get(); }

};


template <>
struct LockTrait<hpx_tag> {
  using LockTy = hpx::lcos::local::spinlock;

  static void lock(LockTy &L) { L.lock(); }
  static void unlock(LockTy &L) { L.unlock(); }
};

template <>
struct RuntimeInternalsTrait<hpx_tag> {
  static void Initialize(int argc, char *argv[]) {}

  static void Finalize() {}

  static size_t Concurrency() { return hpx::get_num_worker_threads(); }
  static void Yield() { hpx::this_thread::yield(); }

  static uint32_t ThisLocality() { return hpx::get_locality_id(); }
  static uint32_t NullLocality() { return hpx::naming::invalid_locality_id; }
  static uint32_t NumLocalities() { 
    return hpx::get_num_localities(hpx::launch::sync); }
};

}  // namespace impl

using TargetSystemTag = impl::hpx_tag;

}  // namespace rt
}  // namespace shad

#endif  // INCLUDE_SHAD_RUNTIME_MAPPINGS_HPX_HPX_TRAITS_MAPPING_H_
