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

#ifndef INCLUDE_SHAD_RUNTIME_MAPPINGS_HPX_HPX_TASK_GROUP_H_
#define INCLUDE_SHAD_RUNTIME_MAPPINGS_HPX_HPX_TASK_GROUP_H_

#include <exception>
#include <memory>
#include <mutex>
#include <type_traits>
#include <utility>
#include <vector>

#include "hpx/config.hpp"
#include "hpx/async_combinators/when_all.hpp"
#include "hpx/async_local/dataflow.hpp"
#include "hpx/functional/bind.hpp"
#include "hpx/functional/bind_back.hpp"
#include "hpx/futures/future.hpp"
#include "hpx/futures/traits/is_future.hpp"
#include "hpx/modules/errors.hpp"
#include "hpx/synchronization/spinlock.hpp"
#include "hpx/execution/executors/execution.hpp"
#include "hpx/executors/exception_list.hpp"
#include "hpx/executors/execution_policy.hpp"
#include "hpx/parallel/util/detail/algorithm_result.hpp"

namespace shad {
namespace rt {

//class Handle;

namespace impl {

template <typename ExPolicy = hpx::execution::parallel_policy>
class task_group
{
private:
    /// \cond NOINTERNAL
    typedef hpx::lcos::local::spinlock mutex_type;

    void wait_for_completion(std::false_type)
    {
        when();
    }

    void wait_for_completion(std::true_type)
    {
        when().wait();
    }

    void wait_for_completion()
    {
        typedef typename hpx::parallel::util::detail::algorithm_result
            <ExPolicy>::type result_type;
        typedef hpx::traits::is_future<result_type> is_fut;
        wait_for_completion(is_fut());
    }

    task_group(task_group const&) = delete;
    task_group& operator=(task_group const&) = delete;
    task_group* operator&() const = delete;

    static void on_ready(std::vector<hpx::future<void>>&& results,
        hpx::parallel::exception_list&& errors)
    {
        for (hpx::future<void>& f : results)
        {
            if (f.has_exception())
                errors.add(f.get_exception_ptr());
        }
        if (errors.size() != 0)
            throw std::forward<hpx::parallel::exception_list>(errors);
    }

    // return future representing the execution of all tasks
    typename hpx::parallel::util::detail::algorithm_result<ExPolicy>::type
        when(bool throw_on_error = false)
    {
        std::vector<hpx::future<void>> tasks;
        hpx::parallel::exception_list errors;

       {
            std::lock_guard<mutex_type> l(mtx_);
            std::swap(tasks_, tasks);
            std::swap(errors_, errors);
       }

        typedef hpx::parallel::util::detail::algorithm_result<ExPolicy> result;

        if (tasks.empty() && errors.size() == 0)
            return result::get();

        if (!throw_on_error)
            return result::get(hpx::when_all(tasks));

        return result::get(
            hpx::dataflow(hpx::util::one_shot(hpx::util::bind_back(
                              &task_group::on_ready, std::move(errors))),
                std::move(tasks)));
    }
    /// \endcond

public:
    explicit task_group(ExPolicy const& policy = ExPolicy())
    //    : id_(hpx::threads::get_self_id())
    {
    }

    ~task_group()
    {
        wait_for_completion();
    }

    // local case
    template <typename F, typename... Ts>
    void run(F&& f, Ts&&... ts)
    {
        hpx::parallel::execution::parallel_executor exec;
        hpx::future<void> result = exec.async_execute(std::forward<F>(f),
                std::forward<Ts>(ts)...);

        std::lock_guard<mutex_type> l(mtx_);
        tasks_.push_back(std::move(result));
    }

    //// remote case
    //template <typename F, typename... Ts>
    //void run(shad::rt::Handle& handle, F&& f, Ts&&... ts)
    //{
    //    hpx::parallel::execution::parallel_executor exec;
    //    hpx::future<void> result = exec.async_execute(std::forward<F>(f),
    //            std::forward<Ts>(ts)...);
//
    //    std::lock_guard<mutex_type> l(mtx_);
    //    tasks_.push_back(std::move(result));
    //}

    void wait()
    {
        wait_for_completion();
    }


private:
    mutable mutex_type mtx_;
    std::vector<hpx::future<void>> tasks_;
    hpx::parallel::exception_list errors_;
    //hpx::threads::thread_id_type id_;
};

}  // namespace impl
}  // namespace rt
}  // namespace shad

/// \cond NOINTERNAL
namespace std {

template <typename ExPolicy>
shad::rt::impl::task_group<ExPolicy>* addressof(
    shad::rt::impl::task_group<ExPolicy>&) = delete;

} // namespace std
/// \endcond

#endif  // INCLUDE_SHAD_RUNTIME_MAPPINGS_HPX_HPX_TASK_GROUP_H_

