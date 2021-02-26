#include "gtest/gtest.h"

#include "shad/runtime/runtime.h"

#include <hpx/iostream.hpp>
#include <iostream>

namespace shad{

int main(int argc, char* argv[])
{
    std::cout << "Welcome: \n";

    hpx::cout << "Hello HPX World! \n";
    return 0;
}

} //namespace shad
