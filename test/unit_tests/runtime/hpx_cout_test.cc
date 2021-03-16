//  Copyright (c) 2021 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// This example demonstrates the use of the facility invoke_function_action that
// allows to wrap arbitrary global functions into an action. Please note: this
// facility will work as expected only if the function address of the wrapped
// function is the same on all localities.

#include <hpx/hpx.hpp>
#include <hpx/hpx_main.hpp>
#include <hpx/serialization/serialize_buffer.hpp>

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <utility>

namespace example {

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
                    std::move(*reinterpret_cast<T*>(args.data())));
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
}    // namespace example

int call_me(int arg)
{
    return arg;
}

int main(int, char*[])
{
    // The function pointer is casted to a std::size_t to avoid compilation
    // problems complaining about raw pointers being used as action parameters.
    // The invoke_function facilities will cast this back to the correct
    // function pointer on the receiving end.
    //
    // This also assumes that all argument types are bitwise copyable
    {
        using action_type = example::invoke_function_action<decltype(&call_me)>;
        using buffer_type = hpx::serialization::serialize_buffer<std::uint8_t>;

        int arg = 42;
        hpx::future<int> result = hpx::async<action_type>(hpx::find_here(),
            reinterpret_cast<std::size_t>(&call_me),
            buffer_type(reinterpret_cast<std::uint8_t*>(&arg), sizeof(arg),
                buffer_type::reference));

        std::cout << "the action invocation returned: " << result.get() << "\n";
    }

    return 0;
}