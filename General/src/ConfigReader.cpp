#include <ConfigReader.hpp>
#include <ConfigSignatures.hpp>

#include <algorithm>
#include <iostream>
#include <fstream>

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
        std::ifstream file(path, std::ios::out);

        sgxConfigParser parser;

        IConfigNode * root = parser.parseFile(file);

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
    sgxConfigParser::parseFile(std::ifstream& file)
    {
        _file_path = "File path defenition future not implemented yet!";
        std::unique_ptr<IConfigNode> root = nullptr;

        return root.release(); //its ok??
    }

    std::string
    sgxConfigParser::reverseParse(IConfigNode * node)
    {
        return "";
    }

    int
    sgxConfigParser::findBlocks(std::ifstream& file)
    {
        int blocks_count = 0;
        int l_real_col, l_real_row, l_col, l_row;
        std::string line;

        bool sgx_cfg_block_start_found = false;

        while (std::getline(file, line)) {
            for (int l_col = 0, l_row = 0; l_col < line.length(); l_col++, l_real_col++) {

            }

            l_row++;
            l_real_row++;
        }

        return blocks_count;
    }

    void
    sgxConfigReader::skipSpaces()
    {
        int i = 0;
        for (; i < line.length(); i++) {
            if (line[i] != ' ') {
                return i-1;
            }
        }

        return -1;
    }

    int
    sgxConfigReader::skipBeforeSignature(const std::string& line, const ISignature * sig)
    {
        int i = 0;
        char buf[sig->maxLen()];
        for (; i < line.length(); i++) {
            std::memcpy(&buf[0], &line[i], sig->maxLen());
            buf[sig->maxLen()] = '\0';
            if ( sig->isSign( buf ) ) {
                return i-1;
            }
        }
        return -1;
    }

    //mb logger must be print?
    void
    sgxConfigParser::print_error(const BlockInfo& err_block)
    {
        int err_line = err_block.parsing_pos.row;
        std::cerr << "Prsing error in file: " << _file_path << std::endl <<
            "  Error in line: " << err_line << std::endl <<
            "  >> " << err_block[err_line] << std::endl <<
            "  Reason: " << sgx_parser_strerror_tab[_errno].name << ": " <<
            sgx_parser_strerror_tab[_errno].description << std::endl <<
            "Terminating reading precess" << std::endl;
    }

} /* sgx  */ 
