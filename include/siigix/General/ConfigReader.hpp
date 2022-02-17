#ifndef CONFIGREADER_HPP_RTPUGKYJ
#define CONFIGREADER_HPP_RTPUGKYJ

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <mutex>

#include "thread_pool.hpp"
#include "BitFlag.hpp"
#include "Config.hpp"
#include "ConfigSignatures.hpp"
#include "Types.hpp"

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


// TODO!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// !!!!!!!!!!_---------------------------!!!!!!!!!!!!!!!
// ADD config reader manager, using singleto signatureManager scuko :(

#define SGX_PARSER_ERROR_MAP(XX) \
    XX(OK,                 "success")                     \
                                                          \
    XX(EMPTY,              "file empty")                  \
    XX(NO_BLOCK_NAME,      "block have no name")          \
    XX(DATA_OUTSIDE_BLOCK, "data outside of block")       \
    XX(NO_END_SIGNATURE,   "data have not end signature") \


namespace sgx {

    #define SGX_PARSER_ERRNO_GEN(num, str) SGXP_##num,
    enum sgx_parser_errno {
        SGX_PARSER_ERROR_MAP(SGX_PARSER_ERRNO_GEN)
    };
    #undef SGX_PARSER_ERRNO_GEN

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

    //RENAME TO ParseBuffer? */
    class BlockInfo {
        public:
            using Lines = std::vector<std::string>; //try to use charVectorBuffer
            class ParseStepInfo;

            class ParseStepInfo {
                friend class BlockInfo;
                linePosition _position;
                bool _jumped_to_new_line = false;
                bool _end = false;
                char _ch;
                public:
                    operator bool() const              { return !_end; }
                    const linePosition& getpos() const { return _position; }
                    char ch() const                    { return _ch; }
                    bool jumped_to_new_line() const    { return _jumped_to_new_line; }
                    bool isEnd() const                 { return _end; }
                    ParseStepInfo snupshot() const     { return ParseStepInfo(*this); }

                    ParseStepInfo()
                        : _position(0, 0),
                        _jumped_to_new_line(false),
                        _end(false),
                        _ch(EOF) {  }
                    ParseStepInfo(const ParseStepInfo& other)
                        : _position(other._position),
                        _jumped_to_new_line(other._jumped_to_new_line),
                        _end(other._end),
                        _ch(other._ch) {  }
            };

        private:
            Lines         _lines;
            ParseStepInfo _parsing_info;
            std::vector<ParseStepInfo> _parsing_info_snaps; /* save */
            int _max_snaps = 100; /* -1 no limitation */
        public:
            //iterate with Lines
            std::string& operator[] (unsigned i)             { return _lines.at(i); }
            const std::string& operator[] (unsigned i) const { return _lines.at(i); }
            void add_line(const std::string line) { _lines.push_back(line); }
            void remove_line(const unsigned i)    { _lines.erase(_lines.begin() + i); } /*its ok??*/

            const int lines() const { return _lines.size(); }
            const ParseStepInfo& parsing_info() const { return _parsing_info; }

            /* Increment parsing pos col, if its end on cur line in _block, increment row; */
            const ParseStepInfo& parser_step() {
                _parsing_info._jumped_to_new_line = false;

                if (_parsing_info_snaps.size() >= _max_snaps) {
                    _parsing_info_snaps.erase(_parsing_info_snaps.begin());
                }
                _parsing_info_snaps.push_back(_parsing_info.snupshot());

                int &l_col = _parsing_info._position.col;
                int &l_row = _parsing_info._position.row;

                /* not the whole line has been read yet */
                if (l_col < _lines.at(l_row).length()) {
                    _parsing_info._jumped_to_new_line = false;
                    l_col++;
                    _parsing_info._ch = _lines.at(l_row).at(l_col);
                }
                /* not last line in block */
                else if (l_row < _lines.size()) {
                    /* reset to next line */
                    _parsing_info._jumped_to_new_line = true;
                    l_row++;
                    l_col = 0;
                    _parsing_info._ch = _lines.at(l_row).at(l_col);
                } else {
                    _parsing_info._end = true;
                    _parsing_info._ch = EOF;
                }

                return _parsing_info;
            }

            const ParseStepInfo& parse_step_back(int steps_back = 1) {
                for (int i = 0; i < steps_back; i++) {
                    /* _parsing_info = _parsing_info_snaps.at(_parsing_info_snaps.size()-1); */
                    _parsing_info = *_parsing_info_snaps.end();
                    _parsing_info_snaps.pop_back();
                }

                return _parsing_info;
            }

            BlockInfo() {  }
            BlockInfo(const Lines& block_text, int max_back_steps_history = 100)
                : _lines(block_text),
                _parsing_info()
            { }
            ~BlockInfo()
            {  }
    };

    /**********************************************************************
    *                              Parsers                               *
    **********************************************************************/

    class ConfigParser {
        private:
            /* virtual ConfigNode* parseFile(const std::string& path) = 0; */
            virtual std::unique_ptr<ConfigNode> parseFile(const std::string& path) = 0;
            virtual std::string  reverseParse(ConfigNode * node) = 0;
    };

    class sgxConfigParser : public ConfigParser {
        public:
            sgxConfigParser();

            /* virtual ConfigNode* parseFile(const std::string& file_path) override; */
            virtual std::unique_ptr<ConfigNode> parseFile(const std::string& file_path) override;
            virtual std::string  reverseParse(ConfigNode * node) override;

            virtual ~sgxConfigParser();
        private:
            void bufferizeFile(const std::string& file_path);
            int prepare_to_sign_cmp(const ISignature * sign, std::string& res);
            bool isOnSignature(const std::string& sign_name);

            std::unique_ptr<ConfigNode> parse();

            void print_error();

            void skipSpaces();
            void seekSignature(const ISignature * searchSign);

            /* std::string stringBeforeSignature(const ISignature * searchSign); */

            int parseBlockName(std::string& res);
            int parseBool(bool& res);
            int parseDecimalType(double& res, bool& is_int);
            int parseString(std::string& res);
            int parseArray(ArrayUnit& res);

            sgx_parser_errno _errno;

            BlockInfo   _block;
            std::string _parsing_file;
    };

    /**********************************************************************
    *                              Readers                               *
    **********************************************************************/

    class ConfigReader {
        public:
            ConfigReader();

            virtual std::unique_ptr<ConfigNode> read(const std::string& path) = 0;
            virtual bool write(const ConfigNode&, const std::string& path) = 0;

            virtual ~ConfigReader();
        private:
        protected:
            signatureManager * _signatureManager;
    };

    class sgxConfigReader : public ConfigReader {
        public:
            sgxConfigReader();

            virtual std::unique_ptr<ConfigNode> read(const std::string& path) override;
            virtual bool write(const ConfigNode&, const std::string& path) override;

            virtual ~sgxConfigReader();
    };

    //TODO add json, xml ...

} /* sgx  */ 


#endif /* end of include guard: CONFIGREADER_HPP_RTPUGKYJ */
