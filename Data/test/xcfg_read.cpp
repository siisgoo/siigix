#include <siigix/Data/Markup.hpp>

#include <memory>
#include <iostream>

using namespace sgx::Markup;

int
main(int argc, char** argv)
{
    IMarkupParser * sgx_parser = new sgxMarkupParser;
    MarkupImpex reader(sgx_parser);
    try {
        std::unique_ptr<MarkupNode> conf = reader.Import(argc > 1 ? argv[1] : "./config.sgx");
    }
    catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

    delete sgx_parser;

    return 0;
}
