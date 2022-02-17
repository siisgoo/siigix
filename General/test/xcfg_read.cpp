#include <ConfigReader.hpp>
#include <memory>
#include <memory>

using namespace sgx;

int
main(int argc, char** argv)
{
    sgxConfigReader reader;
    try {
        std::unique_ptr<ConfigNode> conf = reader.read(argc > 1 ? argv[1] : "./config.sgx");
    }
    catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
