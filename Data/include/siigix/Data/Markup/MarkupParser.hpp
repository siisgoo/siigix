#ifndef MARKUPPARSER_HPP_PI97MFOT
#define MARKUPPARSER_HPP_PI97MFOT

#include "siigix/General/Types.hpp"
#include "siigix/General/Formatter.hpp"
#include "siigix/Data/Markup/MarkupObject.hpp"
#include "siigix/Data/Markup/MarkupSignatures.hpp"

#define SGX_PARSER_ERROR_MAP(XX) \
    XX(OK,                    "success")                                                                    \
                                                                                                            \
    XX(AMBIGOUS,              "ambigous")                                                                   \
    XX(SYNTAX,                "syntax error")                                                               \
    XX(EMPTY,                 "file empty")                                                                 \
    XX(NO_BLOCK_NAME,         "block have no name")                                                         \
    XX(DATA_OUTSIDE_BLOCK,    "data outside of block")                                                      \
    XX(NO_END_SIGNATURE,      "data have not end signature")                                                \
    XX(END_SIGN_BEFORE_START, "found end block signature without start pair")                               \
    XX(NO_ASSIGN_SIGNATURE,   "variable name has been readed, but assign signature not found")              \
    XX(NO_VARIABLE_LOAD,      "variable name and assign signature has been readed, but cannot find value")  \


namespace sgx::Markup {

    /*
     * ERRNO
     */

    #define SGX_PARSER_ERRNO_GEN(num, str) SGXP_##num,
    enum sgx_parser_errno {
        SGX_PARSER_ERROR_MAP(SGX_PARSER_ERRNO_GEN)
    };
    #undef SGX_PARSER_ERRNO_GEN

    /*
     * Parse buffer
     * steps char by char
     */
    class ParseBuffer {
        public:
            using Lines = std::vector<std::string>; //try to use charVectorBuffer
            class ParseStepInfo;

            //rewrite with context oriented class to save memeory!
            class ParseStepInfo {
                friend class ParseBuffer;
                linePosition _position;
                bool _jumped_to_new_line;
                bool _end;
                char _ch;
                public:
                    operator bool() const              { return !_end; }
                    const linePosition& getpos() const { return _position; }
                    char ch() const                    { return _ch; }
                    bool jumped_to_new_line() const    { return _jumped_to_new_line; }
                    bool notEnd() const                { return !_end; }
                    ParseStepInfo snapshot() const     { return ParseStepInfo((_position.col == -1) ? linePosition(0,0) : _position , _jumped_to_new_line, _end, _ch); }

                    ParseStepInfo()
                        : _position(0, -1),
                        _jumped_to_new_line(false),
                        _end(false),
                        _ch('\0') {  }

                    ParseStepInfo(const ParseStepInfo& other)
                        : _position(other._position),
                        _jumped_to_new_line(other._jumped_to_new_line),
                        _end(other._end),
                        _ch(other._ch) {  }

                protected:
                    ParseStepInfo(const linePosition& pos, bool jump, bool end, char ch)
                        : _position(pos),
                        _jumped_to_new_line(jump),
                        _end(end),
                        _ch(ch) {  }
            };

        private:
            //rename to better
            Lines         _lines;
            ParseStepInfo _step_info;
            std::vector<ParseStepInfo> _step_snaps;
            int _max_snaps = 100; /* -1 mean no limitation */

        public:
            //iterate with Lines
            std::string& operator[] (int i)             { return _lines.at(i); }
            const std::string& operator[] (int i) const { return _lines.at(i); }
            void remove_line(const int i)               { _lines.erase(_lines.begin() + i); } /*its ok??*/
            void add_line(std::string line)             {
                Formatter::trim(line);
                if (line.size() > 0) {
                    _lines.push_back(line);
                }
            }

            const int linesCount() const      { return _lines.size(); }
            const Lines& lines() const        { return _lines; }
            const ParseStepInfo& info() const { return _step_info; }

            /* Increment parsing pos col, if its end on cur line in _block, increment row; */
            const ParseStepInfo& step() {
                _step_info._jumped_to_new_line = false;

                if (_step_snaps.size() >= _max_snaps) {
                    if (_max_snaps > 0)
                        _step_snaps.erase(_step_snaps.begin());
                }
                _step_snaps.push_back(_step_info.snapshot());

                int &col_r = _step_info._position.col;
                int &row_r = _step_info._position.row;
                bool &jump_r = _step_info._jumped_to_new_line;
                bool &end_r = _step_info._end;
                char &ch_r = _step_info._ch;

                if (++col_r < _lines[row_r].length())
                {
                    jump_r = false;
                    ch_r = _lines[row_r][col_r];
                }
                else if (++row_r < _lines.size()) {
                    jump_r = true;
                    col_r = 0;
                    ch_r = _lines[row_r][col_r];
                } else {
                    end_r = true;
                    ch_r = EOF;
                }

                return _step_info;
            }

            //redo with .erase()
            const ParseStepInfo& step_back(int steps_back = 1) {
                if (steps_back >= _step_snaps.size())
                    throw std::runtime_error(eprintf("ParseStepInfo::", __func__, " Cannot load snap, his deleted or not exists"));

                for (int i = 0; i < steps_back; i++) {
                    _step_info = _step_snaps.at(_step_snaps.size()-1);
                    _step_snaps.pop_back();
                }

                return _step_info;
            }

            ParseBuffer(int max_back_steps_history = 100)
            { }
            ~ParseBuffer()
            {  }
    };

    /**********************************************************************
    *                              Parsers                               *
    **********************************************************************/

    class IMarkupParser {
        public:
            virtual std::unique_ptr<MarkupNode> parseFile(const std::string& path) = 0;
            virtual std::string  reverseParse(MarkupNode * node) = 0;
            virtual ~IMarkupParser();
    };

    class sgxMarkupParser : public IMarkupParser {
        public:
            sgxMarkupParser();

            virtual std::unique_ptr<MarkupNode> parseFile(const std::string& file_path) override;
            virtual std::string  reverseParse(MarkupNode * node) override;

            virtual ~sgxMarkupParser();
        private:
            std::unique_ptr<MarkupNode> parse();

            int prepare_to_sign_cmp(const ISignature * sign, std::string& res);

            void skipSpaces(bool assert_new_line = false); //stops on any char
            bool isOnSignature(const ISignature* sign);
            bool isOnSignature(const std::string& sign_name);
            bool seekThisLine(const ISignature * sign);
            bool seekThisLine(const std::string& sign_name);
            bool seekSignature(const ISignature * sign);
            bool seekSignature(const std::string& sign_name);
            bool seekSignatureStrict(const ISignature * sign, char ch = ' ');
            bool seekSignatureStrict(const std::string& sign_name, char ch = ' ');

            bool parseBlockName(std::string& res);

            std::string parseUnitName(); /*stops on assign signature, or return empty line*/
            bool parseUnitValue(std::string& ref); /* true mean single, false - array */
            bool parseUnit(Unit& u);

            void print_error();

            sgx_parser_errno _errno = SGXP_OK;

            ParseBuffer _buffer;
            std::string _parsing_file;
    };

} /* sgx::Markup  */ 

#endif /* end of include guard: MARKUPPARSER_HPP_PI97MFOT */
