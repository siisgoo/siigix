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


    struct blockBorders {
        linePosition start;
        linePosition end;

        bool operator == (const blockBorders& other) { return start == other.start && end == other.end; }
        bool operator != (const blockBorders& other) { return !(*this == other); }
    };

    class BlockInfo;
    using BlockLines = std::vector<std::string>;
    using SubBlockVec = std::vector<std::unique_ptr<BlockInfo>>;
    using SubBlockVecIter = SubBlockVec::iterator;
    class BlockInfo {
        private:
            BlockLines   _block;                 /* contain formatted(no ws, no tab, no empty lines) block text */
            linePosition _parsing_pos { 0, 0 };  /* position when parsing block */
            linePosition _prev_parsing_pos;      /* position when parsing block */
            blockBorders _real_block_borders;    /* info about block in file(need for error printing) */
            SubBlockVec  _sub_blocks;            /* childs */
            BlockInfo * _parent_block = nullptr; /* parent */

            bool isInBlock(const linePosition& pos) const {
                return pos.row >= 0 && pos.row <= _block.size() &&
                    pos.col >= 0 && pos.col <= _block.at(pos.row).length();
            }
        public:
            void add_sub_block(const std::unique_ptr<BlockInfo> new_sub_block) {
                _sub_blocks.push_back(new_sub_block);
            }

            void add_sub_block(const blockBorders& real_borders, const BlockLines block_text)
            {
                std::unique_ptr<BlockInfo> new_block(new BlockInfo(real_borders, block_text, this));
                _sub_blocks.push_back(new_block);
            }

            void remove_sub_block(const SubBlockVecIter& iter) {
                _sub_blocks.erase(iter);
            }

            void remove_sub_block(const int i) {
                _sub_blocks.erase(_sub_blocks.begin()+i); //is ok??
            }

            //iterate with BlockLines
            std::string& operator[] (unsigned i) { return _block.at(i); }
            const std::string& operator[] (unsigned i) const { return _block.at(i); }
            void add_line(const std::string line) { _block.push_back(line); }
            void remove_line(const unsigned i) { _block.erase(_block.begin() + i); } /*its ok??*/

            /* borders getters */
            const blockBorders& real_block_borders() const { return  _real_block_borders; }
            const int lines() const { return  _block.size(); }
            const linePosition& parsing_position() const { return _parsing_pos; }

            /*
             * Increment parsing pos col, if its end on cur line in _block, increment row;
             * Sets is_on_new_line true if start reading next line
             * char& res - character on parsing position;
             *
             * return true if on all lines read
             */
            bool parser_step(char& res, bool& is_on_new_line) {
                is_on_new_line = false;
                _prev_parsing_pos = _parsing_pos;
                int &l_col = _parsing_pos.col;
                int &l_row = _parsing_pos.row;

                /* not the whole line has been read yet */
                if (l_col < _block.at(l_row).length()) {
                    l_col++;
                    res = _block.at(l_row).at(l_col);
                }
                /* not last line in block */
                else if (l_row < _block.size()) {
                    /* reset to next line */
                    is_on_new_line = true;
                    l_row++;
                    l_col = 0;
                    res = _block.at(l_row).at(l_col);
                } else {
                    return false;
                }

                return true;
            }

            /* bool parse_step_back(char& res) { */
            /*     bool is_prev_line = false; */

            /*     return is_prev_line; */
            /* } */

            /* TODO */
            const linePosition& blok_pos_to_real(const linePosition& pos) const {
                if (isInBlock(pos)) {
                    /* go to all childrens ... calculate ... */
                    return pos;
                }
                throw std::runtime_error(eprintf("BlockInfo::", __func__, " Cannto convert position col: ", pos.col, " , row: ", pos.row));
            }

            BlockInfo(const blockBorders& rb, const BlockLines& block_text, BlockInfo * parent = nullptr)
                : _block(block_text),
                _real_block_borders(rb),
                _parsing_pos(0, 0)
            {

            }
    };

    /**********************************************************************
    *                              Parsers                               *
    **********************************************************************/

    class ConfigParser {
        private:
            virtual IConfigNode* parseFile(std::ifstream& file) = 0;
            virtual std::string  reverseParse(IConfigNode * node) = 0;
    };

    class sgxConfigParser : public ConfigParser {
        public:
            sgxConfigParser();

            virtual IConfigNode* parseFile(std::ifstream& file) override; //TODO change ifstream to some FileManager class ...
            virtual std::string  reverseParse(IConfigNode * node) override;

            virtual ~sgxConfigParser();
        private:
            int findBlocks(std::ifstream& file); /* return count of founded blocks */
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

            std::mutex       _errno_mutex;
            sgx_parser_errno _errno;

            /* thread_pool _thread_pool; */
            std::unique_ptr<IConfigNode> _root_node;
            std::unique_ptr<BlockInfo>   _root_block;
            std::string  _file_path; //setting by parseFile(file) used in print_error()
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
