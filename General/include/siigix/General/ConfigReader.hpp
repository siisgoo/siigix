#ifndef CONFIGREADER_HPP_RTPUGKYJ
#define CONFIGREADER_HPP_RTPUGKYJ

#include <algorithm>
#include <filesystem>
#include <iostream>

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

    class BlockInfo {
        public:
            using Lines = std::vector<std::string>; //try to use charVectorBuffer
            using SubBlockVec = std::vector<std::unique_ptr<BlockInfo>>;
            using SubBlockIter = SubBlockVec::iterator;

            struct Borders;
            class ParseStepInfo;
            class RealBlockRowsNumbers; /* rename, worst name ever */

            struct Borders {
                linePosition start;
                linePosition end;

                bool operator == (const Borders& other) { return start == other.start && end == other.end; }
                bool operator != (const Borders& other) { return !(*this == other); }

                Borders(const linePosition& start, const linePosition& end) : start(start), end(end) {  }
                Borders() {  }
            };

            class ParseStepInfo {
                friend class BlockInfo;
                linePosition _position;
                bool _jumped_to_new_line = false;
                bool _all_readed = false;
                char _ch;
                public:
                    operator bool() const           { return _all_readed; }
                    const linePosition& getpos() const { return _position; }
                    char ch() const                 { return _ch; }
                    bool jumped_to_new_line() const { return _jumped_to_new_line; }
                    bool no_more_counten() const    { return _all_readed; }
                    ParseStepInfo snupshot() const  { return ParseStepInfo(*this); }

                    ParseStepInfo() {}
                    ParseStepInfo(const ParseStepInfo& other)
                        : _position(other._position),
                        _jumped_to_new_line(other._jumped_to_new_line),
                        _all_readed(other._all_readed),
                        _ch(other._ch) {  }
            };

            class RealBlockRowsNumbers {
                friend class BlockInfo;
                std::vector<start_end<unsigned>> _rows;
                public:
                    const std::vector<start_end<unsigned>>& getRowsNumbers() const { return _rows; }
                    const int getRowNumber(const int search) {
                        if (search < 0) { return -1; }

                        for (auto& r: _rows) {
                            if ((r.diff()+1) <= search) { /* is serching row number in this cell */
                                return r.start+search; //its ok??
                            }
                        }
                        return -1;
                    }
                    RealBlockRowsNumbers& operator = (const RealBlockRowsNumbers& other) {
                        _rows = other._rows;
                        return *this;
                    }
                    RealBlockRowsNumbers() {  }
                    RealBlockRowsNumbers(std::vector<start_end<unsigned>> rows) : _rows(rows) {  }
            };

        private:
            Lines         _block;                    /* Contain formatted(no ws, no tab, no empty lines) block text */
            ParseStepInfo _parsing_info;             /* Info when parsing block */
            ParseStepInfo _prev_parsing_info;        /* save */
            SubBlockVec   _sub_blocks;               /* Childs */
            BlockInfo     * _parent_block = nullptr; /* Parent */
            /* _real_block_lines_used:
             *   Contain line numbers of real file/buffer passed to parser.
             *   Each cell defines range where:
             *    >> start - line number of the beginig this block data members
             *    >> end - line number of the end this block data members */
            RealBlockRowsNumbers _real_block_rows;
        public:
            BlockInfo * parent() const { return _parent_block; };
            void set_parent(BlockInfo * parent) { _parent_block = parent; } //DAGEROUS!!!
            void set_real_block_rows(const RealBlockRowsNumbers& rbrn) { _real_block_rows = rbrn; }

            void add_sub_block(const std::unique_ptr<BlockInfo> new_sub_block) {
                new_sub_block->_parent_block = this;
                _sub_blocks.push_back(new_sub_block);
            }

            void add_sub_block(const Lines block_text, const RealBlockRowsNumbers& lines)
            {
                _sub_blocks.push_back(
                        std::make_unique<BlockInfo>(this, block_text, lines)
                                     );
            }

            void remove_sub_block(const SubBlockIter& iter) {
                _sub_blocks.erase(iter);
            }

            void remove_sub_block(const int i) {
                _sub_blocks.erase(_sub_blocks.begin()+i); //is ok??
            }

            //iterate with Lines
            std::string& operator[] (unsigned i)             { return _block.at(i); }
            const std::string& operator[] (unsigned i) const { return _block.at(i); }
            void add_line(const std::string line) { _block.push_back(line); }
            void remove_line(const unsigned i)    { _block.erase(_block.begin() + i); } /*its ok??*/

            const RealBlockRowsNumbers& real_block_rows() const { return _real_block_rows; }
            const int lines() const { return  _block.size(); }
            const ParseStepInfo& parsing_info() const { return _parsing_info; }

            /* Increment parsing pos col, if its end on cur line in _block, increment row; */
            const ParseStepInfo& parser_step() {
                _parsing_info._jumped_to_new_line = false;
                _prev_parsing_info = _parsing_info.snupshot();

                int &l_col = _parsing_info._position.col;
                int &l_row = _parsing_info._position.row;

                /* not the whole line has been read yet */
                if (l_col < _block.at(l_row).length()) {
                    l_col++;
                    _parsing_info._ch = _block.at(l_row).at(l_col);
                }
                /* not last line in block */
                else if (l_row < _block.size()) {
                    /* reset to next line */
                    _parsing_info._jumped_to_new_line = true;
                    l_row++;
                    l_col = 0;
                    _parsing_info._ch = _block.at(l_row).at(l_col);
                } else {
                    _parsing_info._all_readed = true;
                }

                return _parsing_info;
            }

            /* bool parse_step_back(char& res) { */
            /*     bool is_prev_line = false; */

            /*     return is_prev_line; */
            /* } */

            /* TODO */
            const linePosition block_pos_to_real(const linePosition& pos) const {
                /* if (isInBlock(pos)) { */
                    /* go to all childrens ... calculate ... */
                    /* return pos; */
                /* } */
                return { -1, -1 };
                throw std::runtime_error(eprintf("BlockInfo::", __func__, " Cannto convert position col: ", pos.col, " , row: ", pos.row));
            }

            BlockInfo() //DANGEROUS
                : _block(),
                _real_block_rows(),
                _parsing_info() {  }

            BlockInfo(const BlockInfo * parent, const Lines& block_text, const RealBlockRowsNumbers& row_numbers)
                : _block(block_text),
                _real_block_rows(row_numbers),
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
            virtual IConfigNode* parseFile(const std::string& path) = 0;
            virtual std::string  reverseParse(IConfigNode * node) = 0;
    };

    class sgxConfigParser : public ConfigParser {
        public:
            sgxConfigParser();

            virtual IConfigNode* parseFile(const std::string& file_path) override;
            virtual std::string  reverseParse(IConfigNode * node) override;

            virtual ~sgxConfigParser();
        private:
            void bufferizeFile(const std::string& file_path);
            int prepare_to_sign_cmp(const ISignature * sign, const linePosition pos, std::string& res); /* get string from _buffer */

            std::unique_ptr<BlockInfo> createBlock(int i, linePosition save, BlockInfo * parent);
            int  createBlocks(); /* return count of founded blocks */
            std::unique_ptr<IConfigNode> parseBlocks();
            std::unique_ptr<IConfigNode> parseBlock(BlockInfo& block);

            void print_error(const BlockInfo& error_block);

            void skipSpaces(BlockInfo& blcok);
            const ISignature* skipBeforeAnySignature(BlockInfo& block);
            const void skipBeforeSignature(const ISignature * searchSign, BlockInfo& block);

            std::string getBeforeSignature(const ISignature * stopSign, BlockInfo& block);

            bool parseBlockName(std::string& res, BlockInfo& block);
            bool parseBool(bool& res, BlockInfo& block);
            bool parseDecimalType(double& res, bool& is_int, BlockInfo& block);
            bool parseString(std::string& res, BlockInfo& block);
            bool parseArray(ArrayUnit& res, BlockInfo& block);

            /* std::mutex       _errno_mutex; */
            sgx_parser_errno _errno;

            /* thread_pool _thread_pool; */
            std::unique_ptr<IConfigNode> _root_node;
            std::unique_ptr<BlockInfo>   _root_block;
            std::string _parsing_file;
            charVectorBuffer _buffer;
    };

    /**********************************************************************
    *                              Readers                               *
    **********************************************************************/

    class ConfigReader {
        public:
            ConfigReader();

            virtual IConfigNode* read(const std::string& path) = 0;
            virtual bool write(const ConfigRoot&, const std::string& path) = 0;

            virtual ~ConfigReader();
        private:
        protected:
            signatureManager * _signatureManager;
    };

    class sgxConfigReader : public ConfigReader {
        public:
            sgxConfigReader();

            virtual IConfigNode* read(const std::string& path) override;
            virtual bool write(const ConfigRoot&, const std::string& path) override;

            virtual ~sgxConfigReader();
    };

    //TODO add json, xml ...

} /* sgx  */ 


#endif /* end of include guard: CONFIGREADER_HPP_RTPUGKYJ */
