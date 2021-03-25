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

#ifndef INCLUDE_SHAD_RUNTIME_MAPPINGS_HPX_HPX_UTILITY_H_
#define INCLUDE_SHAD_RUNTIME_MAPPINGS_HPX_HPX_UTILITY_H_

#include <cstddef>
#include <cstdint>
#include <sstream>
#include <system_error>
#include <utility>

# include <iostream>

#include "hpx/hpx.hpp"
#include <hpx/hpx_init.hpp>
#include "hpx/serialization/serialize_buffer.hpp"

#include "shad/runtime/locality.h"

namespace shad {
namespace rt {

namespace impl {

inline uint32_t getLocalityId(const Locality &loc) {
  return static_cast<uint32_t>(loc);
}

inline void checkLocality(const Locality& loc) {
  uint32_t localityID = getLocalityId(loc);
  if (localityID >= hpx::get_num_localities(hpx::launch::sync)) {
    std::stringstream ss;
    ss << "The system does not include " << loc;
    throw std::system_error(0xdeadc0de, std::generic_category(), ss.str());
  }
}

namespace detail {
    ///////////////////////////////////////////////////////////////////////
    // simple utility action which invoke an arbitrary global function
    template <typename F>
    struct invoke_function_ptr;
    template <typename R, typename T>
    struct invoke_function_ptr<R (*)(T)>
    {
        static R call(std::size_t f,
            hpx::serialization::serialize_buffer<std::uint8_t> args)
        {
            return reinterpret_cast<R (*)(T)>(f)(
                std::move(*reinterpret_cast<std::decay_t<T>*>(args.data())));
        }
    };

    struct invoke_function_buffer
    {
        static void call(std::size_t f,
            hpx::serialization::serialize_buffer<std::uint8_t> args)
        {
            reinterpret_cast<void (*)(const uint8_t *, const uint32_t)>(f)(
                args.data(), args.size());
        }
    };
    template <typename F>
    struct invoke_function_with_ret;
    template <typename T>
    struct invoke_function_with_ret<void (*)(T, std::uint8_t*, std::uint32_t*)>
    {
        static hpx::serialization::serialize_buffer<std::uint8_t> call(
          std::size_t f,
          hpx::serialization::serialize_buffer<std::uint8_t> args,
          std::uint32_t size)
        {
            hpx::serialization::serialize_buffer<std::uint8_t> result(size);

            reinterpret_cast<void (*)(T, std::uint8_t*, std::uint32_t*)>(f)(
                std::move(*reinterpret_cast<std::decay_t<T>*>(args.data())),
                result.data(), &size);

            return result;
        }
    };

    struct invoke_function_with_ret_buff
    {
        static hpx::serialization::serialize_buffer<std::uint8_t> call(
          std::size_t f,
          hpx::serialization::serialize_buffer<std::uint8_t> args,
          std::uint32_t size)
        {
            hpx::serialization::serialize_buffer<std::uint8_t> result(size);

            reinterpret_cast<void (*)(const std::uint8_t *, const std::uint32_t,
                                      std::uint8_t*, std::uint32_t*)>(f)(
                args.data(), args.size(), result.data(), &size);

            return result;
        }
    };

}    // namespace detail
// action definition exposing invoke_function_ptr<> that binds a global
// function (Note: this assumes global function addresses are the same on
// all localities. This also assumes that all argument types are bitwise
// copyable
template <typename F>
struct invoke_function_action;
template <typename R, typename T>
struct invoke_function_action<R (*)(T)>
  : ::hpx::actions::action<
        R (*)(std::size_t,
            hpx::serialization::serialize_buffer<std::uint8_t>),
        &detail::invoke_function_ptr<R (*)(T)>::call,
        invoke_function_action<R (*)(T)>>
{
};

struct invoke_function_buffer_action
  : ::hpx::actions::action<
        void (*)(std::size_t,
            hpx::serialization::serialize_buffer<std::uint8_t>),
        &detail::invoke_function_buffer::call,
        invoke_function_buffer_action>
{
};

template <typename F>
struct invoke_function_with_ret_action;
template <typename T>
struct invoke_function_with_ret_action<void (*)(T, std::uint8_t*,
                                                std::uint32_t*)>
  : ::hpx::actions::action<
        hpx::serialization::serialize_buffer<std::uint8_t> (*)(std::size_t,
            hpx::serialization::serialize_buffer<std::uint8_t>, std::uint32_t),
        &detail::invoke_function_with_ret<
            void (*)(T, std::uint8_t*, std::uint32_t*)>::call,
        invoke_function_with_ret_action<void (*)(T, std::uint8_t*,
                                                 std::uint32_t*)>>
{
};


struct invoke_function_with_ret_buff_action
  : ::hpx::actions::action<
        hpx::serialization::serialize_buffer<std::uint8_t> (*)(std::size_t,
            hpx::serialization::serialize_buffer<std::uint8_t>, std::uint32_t),
        &detail::invoke_function_with_ret_buff::call,
        invoke_function_with_ret_buff_action>
{
};


}  // namespace impl

}  // namespace rt
}  // namespace shad

#endif  // INCLUDE_SHAD_RUNTIME_MAPPINGS_HPX_HPX_UTILITY_H_
