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
#include <hpx/hpx_init.hpp>
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

        //template <typename R, typename T>
        //struct invoke_function_ptr<R (*)(T)>
        //{
        //    static R call(std::size_t f,
        //        hpx::serialization::serialize_buffer<std::uint8_t> args)
        //    {
        //        return reinterpret_cast<R (*)(T)>(f)(
        //            std::move(*reinterpret_cast<std::decay_t<T>*>(args.data())));
        //    }
        //};

        template <typename R, typename... Ts>
        struct invoke_function_ptr<R (*)(Ts...)>
        {
            static R call(std::size_t f,
                hpx::serialization::serialize_buffer<std::uint8_t> ts)
            {
                return reinterpret_cast<R (*)(Ts...)>(f)(
                    std::move(*reinterpret_cast<std::decay_t<Ts>*>(ts.data()))...);
            }
        };
    }    // namespace detail

    // action definition exposing invoke_function_ptr<> that binds a global
    // function (Note: this assumes global function addresses are the same on
    // all localities. This also assumes that all argument types are bitwise
    // copyable
    template <typename F>
    struct invoke_function_action;

    //template <typename R, typename T>
    //struct invoke_function_action<R (*)(T)>
    //  : ::hpx::actions::action<
    //        R (*)(std::size_t,
    //            hpx::serialization::serialize_buffer<std::uint8_t>),
    //        &detail::invoke_function_ptr<R (*)(T)>::call,
    //        invoke_function_action<R (*)(T)>>
    //{
    //};

    template <typename R, typename... Ts>
    struct invoke_function_action<R (*)(Ts...)>
      : ::hpx::actions::action<
            R (*)(std::size_t,
                hpx::serialization::serialize_buffer<std::uint8_t>),
            &detail::invoke_function_ptr<R (*)(Ts...)>::call,
            invoke_function_action<R (*)(Ts...)>>
    {
    };
}    // namespace example

int call_me(int arg)
{
    return arg;
}

void void_call_me(const int &arg) {std::cout << "Yah!" << '\n';}

void void_test(const int &arg1, const int &arg2){std::cout << "test!" << '\n';}

int hpx_main()
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

    {
        using action_type = example::invoke_function_action<decltype(&void_call_me)>;
        using buffer_type = hpx::serialization::serialize_buffer<std::uint8_t>;

        const int &arg = 42;

        hpx::future<void> result = hpx::async<action_type>(hpx::find_here(),
            reinterpret_cast<std::size_t>(&void_call_me),
            buffer_type(const_cast<std::uint8_t*>(reinterpret_cast<const std::uint8_t*>(&arg)), sizeof(arg),
                buffer_type::reference));

        result.get();
    }

    {
        using action_type = example::invoke_function_action<decltype(&void_test)>;
        using buffer_type = hpx::serialization::serialize_buffer<std::uint8_t>;

        struct wrapperArgs {
          const int & arg1;
          const int & arg2;
        } arg = {.arg1=42, .arg2= 6};

        //wrapperArgs arg;
        //const int &arg1 = 42;
        //const int &arg2 = 12;

        //new_size = sizeof(arg.arg1) + sizeof(arg.arg2);

        hpx::future<void> result = hpx::async<action_type>(hpx::find_here(),
            reinterpret_cast<std::size_t>(&void_test),
            buffer_type(const_cast<std::uint8_t*>(reinterpret_cast<const std::uint8_t*>(&arg)), (sizeof(arg.arg1) + sizeof(arg.arg2)),
                buffer_type::reference));

        result.get();
    }

    return hpx::finalize();
}

int main(int argc, char* argv[])
{
    // Configure application-specific options
    hpx::program_options::options_description
       desc_commandline("Usage: " HPX_APPLICATION_STRING " [options]");

    // Initialize and run HPX
    hpx::init_params init_args;
    init_args.desc_cmdline = desc_commandline;

    return hpx::init(argc, argv, init_args);
}