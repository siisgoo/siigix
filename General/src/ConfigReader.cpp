#include <ConfigReader.hpp>
#include <ConfigSignatures.hpp>

#include <algorithm>
#include <iostream>
#include <fstream>
#include <memory>

namespace sgx {

    /**********************************************************************
    *                            ConfigReader                            *
    **********************************************************************/

    ConfigReader::ConfigReader()
        : _signatureManager(new signatureManager)
    {
    }

    ConfigReader::~ConfigReader()
    {
        delete _signatureManager;
    }

    /*--------------------*/

    sgxConfigReader::sgxConfigReader()
        : ConfigReader() {  }

    IConfigNode *
    sgxConfigReader::read(const std::string& path)
    {
        sgxConfigParser parser;

        IConfigNode * root = parser.parseFile(path);

        return root;
    }

    bool
    sgxConfigReader::write(const ConfigRoot& conf, const std::string& path)
    {
        return false;
    }

    /*--------------------*/

    /**********************************************************************
    *                            ConfigParser                            *
    **********************************************************************/

    #define SGX_PARSER_ERRSTR_GEN(num, desk) {"SGXP_" #num, desk},
        static struct {
            const char *name;
            const char *description;
        } sgx_parser_strerror_tab[] = {
            SGX_PARSER_ERROR_MAP(SGX_PARSER_ERRSTR_GEN)
        };
    #undef SGX_PARSER_ERRSTR_GEN

    sgxConfigParser::sgxConfigParser() {  }
    sgxConfigParser::~sgxConfigParser() {  }

    IConfigNode *
    sgxConfigParser::parseFile(const std::string& file_path)
    {
        _parsing_file = file_path;
        std::unique_ptr<IConfigNode> root = nullptr;

        _root_block = nullptr;

        return root.release(); //its ok??
    }

    std::string
    sgxConfigParser::reverseParse(IConfigNode * node)
    {
        return "";
    }

    void
    sgxConfigParser::bufferizeFile(const std::string& path)
    {
        std::ifstream file(path);
        std::string file_content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        std::copy(file_content.begin(), file_content.end(), _buffer.begin());
        file.close();
    }

    int
    sgxConfigParser::prepare_to_sign_cmp(const ISignature * sign, const linePosition pos, std::string& res)
    {
        return _buffer.getline(pos.row, pos.col, pos.col + sign->maxLen(), res);
    }

    std::unique_ptr<BlockInfo>
    sgxConfigParser::createBlock(int i, linePosition pre_pos, BlockInfo * parent)
    {
        const ISignature * start_sign = signatureManager::getSignature("sgx_cfg_block_start");
        const ISignature * end_sign   = signatureManager::getSignature("sgx_cfg_block_end");
        int col = pre_pos.col,
            row = pre_pos.row;
        char ch;
        BlockInfo::Borders block_borders;
        BlockInfo::Lines lines;
        std::string line_buffer; //prepare before add to lines

        i = col = row = 0;
        for (; _buffer[i] < _buffer.size(); i++)
        {
            ch = _buffer[i];
            if (ch == '\n') {
                col = 0;
                row++;
                continue;
            }

            std::string cmp_bs;
            std::string cmp_be;
            prepare_to_sign_cmp(start_sign, linePosition(row, col), cmp_bs);
            prepare_to_sign_cmp(end_sign, linePosition(row, col), cmp_be);

            if (start_sign->isSign(cmp_be)) {
                block_borders.start = linePosition(row, col);
            } else if (end_sign->isSign(cmp_be)) {
                block_borders.end = linePosition(row, col);
            }

            col++;
        }
    }

    int
    sgxConfigParser::createBlocks()
    {
        if (_buffer.size() <= 0) {
            return 0;
        }

        const ISignature * start_sign = signatureManager::getSignature("sgx_cfg_block_start");
        const ISignature * end_sign   = signatureManager::getSignature("sgx_cfg_block_end");
        char ch;
        int col, row;
        int depth = 0;

        std::vector<BlockInfo::Borders> blocks_borders;

        /* find blocks borders */
        col = row = 0;
        for (; _buffer[col] < _buffer.size(); col++)
        {
            ch = _buffer[col];
            if (ch == '\n') {
                row++;
                continue;
            }

            std::string cmp_bs;
            std::string cmp_be;
            prepare_to_sign_cmp(start_sign, linePosition(row, col), cmp_bs);
            prepare_to_sign_cmp(end_sign, linePosition(row, col), cmp_be);

            if (start_sign->isSign(cmp_bs)) {
                BlockInfo::Borders cur_borders(linePosition(row, col), linePosition());
                blocks_borders.push_back(cur_borders);
                depth++;
            } else if (end_sign->isSign(cmp_be)) {
                depth--;
                blocks_borders.at(depth).end = linePosition(row, col);
            }

            col++;
        }

        return blocks_borders.size();
    }

    /* void */
    /* sgxConfigReader::skipSpaces(BlockInfo& block) */
    /* { */
    /*     BlockInfo::ParseStepInfo psi; */
    /*     while ((psi = block.parser_step()).no_more_counten()) { */
    /*         if (psi.ch() != ' ') return; */
    /*     } */
    /* } */

    /* int */
    /* sgxConfigReader::skipBeforeSignature() */
    /* { */
    /*     int i = 0; */
    /*     char buf[sig->maxLen()]; */
    /*     for (; i < line.length(); i++) { */
    /*         std::memcpy(&buf[0], &line[i], sig->maxLen()); */
    /*         buf[sig->maxLen()] = '\0'; */
    /*         if ( sig->isSign( buf ) ) { */
    /*             return i-1; */
    /*         } */
    /*     } */
    /*     return -1; */
    /* } */

    //mb logger must be print?
    void
    sgxConfigParser::print_error(const BlockInfo& err_block)
    {
        int err_line = err_block.parsing_info().getpos().row;
        std::cerr << "Prsing error in file: " << _parsing_file << std::endl <<
            "  Error in line: " << err_line << std::endl <<
            "  >> " << err_block[err_line] << std::endl <<
            "  Reason: " << sgx_parser_strerror_tab[_errno].name << ": " <<
            sgx_parser_strerror_tab[_errno].description << std::endl <<
            "Terminating reading precess" << std::endl;
    }

} /* sgx  */ 
