#include <hpx/algorithm.hpp>
#include <hpx/hpx.hpp>
#include <hpx/hpx_init.hpp>
#include <hpx/modules/testing.hpp>

#include <algorithm>
#include <cstddef>
#include <iostream>
#include <numeric>
#include <random>
#include <string>
#include <utility>
#include <vector>

int seed = std::random_device{}();
std::mt19937 gen(seed);

int hpx_main(int, char**)
{
    typedef std::vector<std::size_t>::iterator base_iterator;
    //typedef test::test_iterator<base_iterator, int> iterator;

    std::vector<std::size_t> c(10007);
    std::iota(std::begin(c), std::end(c), gen());
    std::size_t sum2 =
        std::accumulate(std::begin(c), std::end(c), std::size_t(0));
    std::cout << "sum2: " << sum2 << '\n';

    std::size_t sum = 0;
    hpx::for_loop(hpx::execution::par, 0, 10007,
        [&](const size_t &it) { sum += it; });
    std::cout << "sum: " << sum << '\n';

    return hpx::finalize();
}

int main(int argc, char* argv[])
{
    return hpx::init(argc, argv);
}