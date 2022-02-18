#ifndef CONFIGREADER_HPP_RTPUGKYJ
#define CONFIGREADER_HPP_RTPUGKYJ

#include "siigix/General/Formatter.hpp"
#include "siigix/General/thread_pool.hpp"
#include "siigix/General/Types.hpp"
#include "siigix/Data/Markup/MarkupParser.hpp"
#include "siigix/Data/Markup/MarkupObject.hpp"
#include "siigix/Data/Markup/MarkupSignatures.hpp"

#include <algorithm>
#include <iostream>
#include <string>

/*
 * sgxMarkup file standart:
 *
 * All must be in blocks defined inside {}, blacks may be nested
 * Block optionaly may have a name defined inside [], name defines inside block, only one accepted
 * Setting aka MarkupUnit must have name(filed) and value separeted by ":" or "="
 * Multiline values must be ecraned with "\" or if its a string all insize quotes will be readed in one Field(white spaces will be counted)
 *
 * Data types:
 *  bool - contain true or false in any case without quotes
 *  decimal - decimal value without quotes float point must be "." not ","
 *  string - string inside quotes
 *  array - any sequence with any type inside []
 *
 * Variable names:
 *  can be an any case contain all characters exclude ":", "=", ":=", "[", "]", "{", "}", "\", "/", ".", ","
 */


// TODO!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// !!!!!!!!!!_---------------------------!!!!!!!!!!!!!!!
// ADD config reader manager, using singleto signatureManager scuko :(
//
// Add check for double defenition of [name] in block

namespace sgx::Markup {

    class MarkupImpex {
        public:
            MarkupImpex(IMarkupParser *);

            void setParser(IMarkupParser *);

            std::unique_ptr<MarkupNode> Import(const std::string& path);
            void Export(const MarkupNode&, const std::string& path);

            virtual ~MarkupImpex();

        private:
            signatureManager * _signatureManager;
            IMarkupParser * parser;
    };

    //TODO add json, xml ...

} /* sgx  */ 


#endif /* end of include guard: CONFIGREADER_HPP_RTPUGKYJ */
