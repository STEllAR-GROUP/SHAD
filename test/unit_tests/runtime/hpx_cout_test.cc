#include <hpx/config.hpp>
#include <hpx/hpx.hpp>
#include <hpx/hpx_init.hpp>

#include <hpx/iostream.hpp>

int hpx_main()
{

    hpx::cout << "Hello HPX World!" << std::endl;

    return hpx::finalize();
}
int main(int argc, char* argv[])
{
    std::cout << "running HPX test " << std::endl;
    return hpx::init(argc, argv);       // Initialize and run HPX.
}