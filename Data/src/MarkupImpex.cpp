#include "siigix/Data/Markup/MarkupImpex.hpp"
#include "siigix/Data/Markup/MarkupSignatures.hpp"

#include <algorithm>
#include <iostream>
#include <fstream>
#include <memory>
#include <system_error>

namespace sgx::Markup {

    MarkupImpex::MarkupImpex(IMarkupParser * p)
        : _signatureManager(new signatureManager),
        parser(p)
    {
    }

    MarkupImpex::~MarkupImpex()
    {
        delete _signatureManager;
    }

    void
    MarkupImpex::setParser(IMarkupParser * pp)
    {
        parser = pp;
    }

    std::unique_ptr<MarkupNode>
    MarkupImpex::Import(const std::string& path)
    {
        return std::move(parser->parseFile(path));;
    }

    void
    MarkupImpex::Export(const MarkupNode& conf, const std::string& path)
    {
    }

} /* sgx  */ 
