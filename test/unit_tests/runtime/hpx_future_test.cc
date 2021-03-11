//  Copyright (c) 2015 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <hpx/hpx.hpp>
#include <hpx/hpx_init.hpp>

#include <hpx/include/task_group.hpp>
#include <hpx/iostream.hpp>
#include <hpx/modules/testing.hpp>
#include <hpx/runtime_local/custom_exception_info.hpp>

#include <string>
#include <vector>
#include <memory>

using hpx::execution::par;
using hpx::execution::parallel_task_policy;
using hpx::execution::task;
using hpx::task_group;

///////////////////////////////////////////////////////////////////////////////
void task_group_test1()
{
    std::string s("test");

    bool parent_flag = false;
    bool task1_flag = false;
    bool task2_flag = false;
    bool task21_flag = false;
    bool task3_flag = false;


    task_group g;
    parent_flag = true;
    g.run([&](){
        task1_flag = true;
        hpx::cout << "task1: " << s << hpx::endl;
    });

    g.run([&] {
        task2_flag = true;
        hpx::cout << "task2: " << hpx::endl;
        task_group h;
        h.run([&]{
            task21_flag = true;
            hpx::cout << "task2.1: " << s << hpx::endl;
        });
    });

    int i = 0, j = 10, k = 20;
    g.run([=, &task3_flag]() {
        task3_flag = true;
        hpx::cout << "task3: " << i << " " << j << " " << k << hpx::endl;
    });

    hpx::cout << "parent" << hpx::endl;

    g.wait();


    HPX_TEST(parent_flag);
    HPX_TEST(task1_flag);
    HPX_TEST(task2_flag);
    HPX_TEST(task21_flag);
    HPX_TEST(task3_flag);

}

///////////////////////////////////////////////////////////////////////////////
void task_group_test3()
{
    std::string s("test");

    bool parent_flag = false;
    bool task1_flag = false;
    bool task2_flag = false;
    bool task21_flag = false;
    bool task3_flag = false;


    task_group g;
    using a = std::shared_ptr<task_group<>>;
    parent_flag = true;
    g.run([&](){
        task1_flag = true;
        hpx::cout << "task1: " << s << hpx::endl;
    });

    g.run([&] {
        task2_flag = true;
        hpx::cout << "task2: " << hpx::endl;
        task_group h;
        h.run([&]{
            task21_flag = true;
            hpx::cout << "task2.1: " << s << hpx::endl;
        });
    });

    int i = 0, j = 10, k = 20;
    g.run([=, &task3_flag]() {
        task3_flag = true;
        hpx::cout << "task3: " << i << " " << j << " " << k << hpx::endl;
    });

    hpx::cout << "parent" << hpx::endl;

    g.wait();


    HPX_TEST(parent_flag);
    HPX_TEST(task1_flag);
    HPX_TEST(task2_flag);
    HPX_TEST(task21_flag);
    HPX_TEST(task3_flag);

}
///////////////////////////////////////////////////////////////////////////////
int hpx_main()
{
    //task_group_test1();
    //task_group_test2();
    task_group_test3();

    return hpx::finalize();
}

int main(int argc, char* argv[])
{
    // By default this test should run on all available cores
    std::vector<std::string> const cfg = {"hpx.os_threads=all"};

    // Initialize and run HPX
    hpx::init_params init_args;
    init_args.cfg = cfg;
    HPX_TEST_EQ_MSG(hpx::init(argc, argv, init_args), 0,
        "HPX main exited with non-zero status");

    return hpx::util::report_errors();
}