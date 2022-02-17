#include <MarkupReader.hpp>
#include <memory>
#include <memory>

using namespace sgx::Markup;

int
main(int argc, char** argv)
{
    sgxMarkupReader reader;
    try {
        std::unique_ptr<MarkupNode> conf = reader.read(argc > 1 ? argv[1] : "./config.sgx");
    }
    catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
