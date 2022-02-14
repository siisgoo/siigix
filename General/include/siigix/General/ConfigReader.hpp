#ifndef CONFIGREADER_HPP_RTPUGKYJ
#define CONFIGREADER_HPP_RTPUGKYJ

#include <algorithm>
#include <filesystem>
#include <iostream>

#include "BitFlag.hpp"
#include "Config.hpp"
#include "ConfigSignatures.hpp"

/*
 * May be replace it in new module?
 */

/*
 * sgxConfig file standart:
 *
 * All must be in blocks defined inside {}, blacks may be nested
 * Block optionaly may have a name defined inside [], name defines inside block, only one accepted
 * Setting aka ConfigUnit must have name(filed) and value separeted by ":" or "="
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


//
//
//
// TODO!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//
// ADD config reader manager, using singleto signatureManager scuko :(
//
//
//

namespace sgx {
    //Add factory?

    class ConfigReader {
        public:
            ConfigReader();

            virtual std::unique_ptr<ConfigRoot> read(const std::string& path) = 0;
            virtual bool write(const ConfigRoot&, const std::string& path) = 0;

            virtual ~ConfigReader();
        private:
        protected:
            signatureManager * _signatureManager;
    };

    class ReadingState : public BitFlag<bitflag32_t> {
        public:
        enum {
            in_block,
            in_quotes,
            in_word,

            field_readed,

            after_assigment,

            error,
            reading,
            end_reading,
        };
    };

    /* my own format :) */
    class sgxConfigReader : public ConfigReader {
        public:
            virtual std::unique_ptr<ConfigRoot> read(const std::string& path) override;
            virtual bool write(const ConfigRoot&, const std::string& path) override;

        private:
            void printErrorLine(const std::string& file, const std::string& str, int line);

            int skipSpaces(const std::string& line); /* return pos when end spaces or -1 if line contain only spaces */
            int skipBeforeSignature(const std::string& line, const ISignature* sign);

            void readBool(IConfigNode* res, const std::string& cur_line, std::ifstream& file);
            void readDecimalType(IConfigNode* res, const std::string& cur_line, std::ifstream& file);
            void readString(IConfigNode* res, const std::string& cur_line, std::ifstream& file);
            void readArray(IConfigNode* res, const std::string& cur_line, std::ifstream& file);

            std::string _buffer;
            ReadingState rState;
    };

    //TODO add json, xml ...

} /* sgx  */ 


#endif /* end of include guard: CONFIGREADER_HPP_RTPUGKYJ */
