#include <ConfigReader.hpp>
#include <ConfigSignatures.hpp>

#include <algorithm>
#include <iostream>
#include <fstream>
#include <memory>
#include <system_error>

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

    sgxConfigReader::~sgxConfigReader()
    {  }

    [[nodiscard]] std::unique_ptr<ConfigNode>
    sgxConfigReader::read(const std::string& path)
    {
        sgxConfigParser parser;
        return std::move(parser.parseFile(path));;
    }

    bool
    sgxConfigReader::write(const ConfigNode& conf, const std::string& path)
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

    std::unique_ptr<ConfigNode>
    sgxConfigParser::parseFile(const std::string& file_path)
    {
        _parsing_file = file_path;
        std::ifstream file(_parsing_file);
        for (std::string buff; std::getline(file, buff); ) {
            _block.add_line(buff);
        }

        file.close();

        return std::move(parse());
    }

    std::string
    sgxConfigParser::reverseParse(ConfigNode * node)
    {
        return "";
    }

    std::unique_ptr<ConfigNode>
    sgxConfigParser::parse()
    {
        if (_block.linesCount() <= 0) {
            return 0;
        }

        std::unique_ptr<ConfigNode> root = std::make_unique<ConfigNode>("root", nullptr);
        int cur_depth = -1;
        int even = 0;

        while (_block.parser_step()) {
            if (seekSignatureStrict("sgx_cfg_block_start")) {
                if (cur_depth >= 0) { //root
                    ConfigNode * node = root.get();
                    //find target node
                    for (int i = 0; i < cur_depth; i++) {
                        node = node->getSubNodes().at(node->subNodesCount()-1).get();
                    }
                    node->addSubNode("unnamed");
                }
                even++;
                cur_depth++;
                continue;
            }

            if (isOnSignature("sgx_cfg_block_end")) {
                if (cur_depth < 0) {
                    _errno = SGXP_END_SIGN_BEFORE_START;
                    print_error();
                    throw std::runtime_error(eprintf("ДЕБИЛКА, НЕ ПРАВИЛЬНЫЙ КФГ!"));
                }
                even++;
                cur_depth--;

            }

            if (cur_depth < 0 && even % 2 != 0) {
                _errno = SGXP_DATA_OUTSIDE_BLOCK;
                print_error();
                throw std::runtime_error(eprintf("TUTA BEDA"));
            }

            //if find var name
            /* if (isOnSignature("sgx_cfg_var")) { */
            /*     Unit unit; */

            /*     if (parseUnit(unit)) { */
            /*         if (cur_depth == 1) { */
            /*             root->addUnit(unit); */
            /*         } else { */
            /*             ConfigNode * node = root.get(); */
            /*             //find target node */
            /*             for (int i = 0; i < cur_depth; i++) { */
            /*                 node = node->getSubNodes().at(node->subNodesCount()-1).get(); */
            /*             } */
            /*             node->addUnit(unit); */
            /*         } */
            /*     } */
            /* } */

            if (isOnSignature("sgx_cfg_block_name_start")) {
                std::string name;
                if (parseBlockName(name)) {
                    if (cur_depth > 0) {
                        ConfigNode * node = root.get();
                        for (int i = 0; i < cur_depth; i++) {
                            node = node->getSubNodes().at(node->subNodesCount()-1).get();
                        }
                        node->setName(name);
                    } else {
                        root->setName(name);
                    }
                } else {
                    print_error();
                }
            }
        }

        return std::move(root);
    }

    int
    sgxConfigParser::prepare_to_sign_cmp(const ISignature * sign, std::string& res)
    {
        res.clear();
        int i;
        for (i = 0; i < sign->maxLen(); i++, _block.parser_step()) {
            if (_block.parsing_info().notEnd())
                res.append(1, _block.parsing_info().ch());
        }

        _block.parse_step_back(i);
        return i;
    }

    void
    sgxConfigParser::skipSpaces()
    {
        BlockInfo::ParseStepInfo step;
        int i = 0;
        while (_block.parsing_info().notEnd() && _block.parsing_info().ch() == ' ') {
            _block.parser_step();
            i = 1;
        }
        _block.parse_step_back(i ? 1 : 0); //handle if already no more chars in buffer
    }

    bool
    sgxConfigParser::isOnSignature(const std::string& sign_name)
    {
        return isOnSignature(signatureManager::getSignature(sign_name));
    }

    bool
    sgxConfigParser::isOnSignature(const ISignature * sign)
    {
        std::string cmp_str;
        prepare_to_sign_cmp(sign, cmp_str);
        if (sign->isSign(cmp_str)) return true;
        return false;
    }

    bool
    sgxConfigParser::seekSignature(const ISignature * sign)
    {
        do {
            if ( isOnSignature(sign) ) return true;
        } while (_block.parser_step());

        return false;
    }

    bool
    sgxConfigParser::seekSignature(const std::string& name)
    {
        return seekSignature(signatureManager::getSignature(name));
    }

    bool
    sgxConfigParser::seekSignatureStrict(const ISignature * sign, char ch)
    {
        do {
            if (_block.parsing_info().ch() != ch) { /*skip only ch*/
                if (isOnSignature(sign))
                    return true;
                else {
                    /* _block.parse_step_back(); */
                    return false;
                }
            }
        } while (_block.parser_step());

        return false;
    }

    bool
    sgxConfigParser::seekSignatureStrict(const std::string& name, char ch)
    {
        return seekSignatureStrict(signatureManager::getSignature(name), ch);
    }

    bool
    sgxConfigParser::parseBlockName(std::string& res)
    {
        res.clear();
        while (_block.parser_step()) {
            if (!isOnSignature("sgx_cfg_name")) {
                switch (_block.parsing_info().ch()) {
                    case ']':
                        return true;
                    case '[':
                    case '{':
                    case '}':
                    default:
                        _errno = SGXP_NO_END_SIGNATURE;
                        break;
                }
            } else {
                res.append(1, _block.parsing_info().ch());
            }
        }

        return false;
    }

    bool
    sgxConfigParser::parseUnit(Unit& u)
    {

        return false;
    }

    //mb logger must be print?
    void
    sgxConfigParser::print_error()
    {
        int err_line = _block.parsing_info().getpos().row;
        std::cerr << "Error occured during parsing. Parsing file: " << _parsing_file << std::endl;
        std::cerr << "  >> " << _block[err_line] << std::endl;
        std::cerr << "Error number: " << _errno << " Reason: " << sgx_parser_strerror_tab[_errno].name << ": " <<
            sgx_parser_strerror_tab[_errno].description << std::endl;
    }

} /* sgx  */ 
