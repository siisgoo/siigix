#include "siigix/Data/Markup/MarkupParser.hpp"

#include <iostream>
#include <fstream>
#include <string>

namespace sgx::Markup {

    IMarkupParser::~IMarkupParser() {  }

    #define SGX_PARSER_ERRSTR_GEN(num, desk) {"SGXP_" #num, desk},
        static struct {
            const char *name;
            const char *description;
        } sgx_parser_strerror_tab[] = {
            SGX_PARSER_ERROR_MAP(SGX_PARSER_ERRSTR_GEN)
        };
    #undef SGX_PARSER_ERRSTR_GEN

    sgxMarkupParser::sgxMarkupParser() {  }
    sgxMarkupParser::~sgxMarkupParser() {  }

    std::unique_ptr<MarkupNode>
    sgxMarkupParser::parseFile(const std::string& file_path)
    {
        _parsing_file = file_path;
        std::ifstream file(_parsing_file);
        for (std::string buff; std::getline(file, buff); ) {
            _buffer.add_line(buff);
        }

        file.close();

        return std::move(parse());
    }

    std::string
    sgxMarkupParser::reverseParse(MarkupNode * node)
    {
        return "";
    }

    std::unique_ptr<MarkupNode>
    sgxMarkupParser::parse()
    {
        if (_buffer.linesCount() <= 0) {
            return 0;
        }

        std::unique_ptr<MarkupNode> root = std::make_unique<MarkupNode>("root", nullptr);
        int cur_depth = -1;
        int even = 0;

        while (_buffer.step()) {
            /* On block start char */
            if (seekSignatureStrict("sgx_mrk_block_start"))
            {
                if (cur_depth >= 0) { //root
                    MarkupNode * node = root.get();
                    //find target node
                    for (int i = 0; i < cur_depth; i++) {
                        node = node->getSubNodes().at(node->subNodesCount()-1).get();
                    }
                    node->addSubNode("unnamed");
                }
                even++;
                cur_depth++;
            /* On block end char */
            }
            else if (isOnSignature("sgx_mrk_block_end"))
            {
                if (cur_depth < 0) {
                    _errno = SGXP_END_SIGN_BEFORE_START;
                    break;
                }
                even++;
                cur_depth--;
            }
            else if (cur_depth < 0 && even % 2 != 0)
            {
                _errno = SGXP_DATA_OUTSIDE_BLOCK;
                print_error();
                throw std::runtime_error(eprintf("TUTA BEDA"));
                break;
            /* On variable name char */
            }
            else if (isOnSignature("sgx_mrk_var_name"))
            {
                Unit unit;

                _buffer.step_back();
                if (parseUnit(unit)) {
                    if (cur_depth > 1) {
                        MarkupNode * node = root.get();
                        for (int i = 0; i < cur_depth; i++) {
                            node = node->getSubNodes().at(node->subNodesCount()-1).get();
                        }
                        node->addUnit(unit);
                    } else {
                        root->addUnit(unit);
                    }
                } else {
                    break;
                }
            /* On block name char */
            }
            else if (isOnSignature("sgx_mrk_block_name_start"))
            {
                std::string name;
                if (parseBlockName(name)) {
                    if (cur_depth > 0) {
                        MarkupNode * node = root.get();
                        for (int i = 0; i < cur_depth; i++) {
                            node = node->getSubNodes().at(node->subNodesCount()-1).get();
                        }
                        node->setName(name);
                    } else {
                        root->setName(name);
                    }
                } else {
                    break;
                }
                continue;
            }
            else if (!isOnSignature("space") || !isOnSignature("sgx_mrk_end_line"))
            {
                _errno = SGXP_DATA_OUTSIDE_BLOCK;
                break;
            }
        } /* End main parse loop */

        if (_errno != SGXP_OK) {
            print_error();
        }

        return std::move(root);
    }

    int
    sgxMarkupParser::prepare_to_sign_cmp(const ISignature * sign, std::string& res)
    {
        res.clear();
        int i;
        for (i = 0; i < sign->maxLen(); i++, _buffer.step()) {
            if (_buffer.info())
                res.append(1, _buffer.info().ch());
        }

        _buffer.step_back(i);
        return i;
    }

    void
    sgxMarkupParser::skipSpaces(bool assert_new_line)
    {
        bool newline = false;
        while (_buffer.info() && _buffer.info().ch() == ' ') {
            if (_buffer.info().jumped_to_new_line() && assert_new_line) { break; }
            _buffer.step();
        }
    }

    bool
    sgxMarkupParser::isOnSignature(const std::string& sign_name)
    {
        return isOnSignature(signatureManager::getSignature(sign_name));
    }

    bool
    sgxMarkupParser::isOnSignature(const ISignature * sign)
    {
        std::string cmp_str;
        prepare_to_sign_cmp(sign, cmp_str);
        if (sign->isSign(cmp_str)) return true;
        return false;
    }

    bool
    sgxMarkupParser::seekSignature(const ISignature * sign)
    {
        do {
            if ( isOnSignature(sign) ) return true;
        } while (_buffer.step());

        return false;
    }

    bool
    sgxMarkupParser::seekSignature(const std::string& name)
    {
        return seekSignature(signatureManager::getSignature(name));
    }

    bool
    sgxMarkupParser::seekSignatureStrict(const ISignature * sign, char ch)
    {
        do {
            if (_buffer.info().ch() != ch) { /*skip only ch*/
                if (isOnSignature(sign))
                    return true;
                else
                    return false;
            }
        } while (_buffer.step());

        return false;
    }

    bool
    sgxMarkupParser::seekSignatureStrict(const std::string& name, char ch)
    {
        return seekSignatureStrict(signatureManager::getSignature(name), ch);
    }

    bool
    sgxMarkupParser::seekThisLine(const ISignature * sign)
    {
        int i = 0;
        do {
            if (i && _buffer.info().jumped_to_new_line()) {
                return false;
            } else if (isOnSignature(sign)) {
                return true;
            }
            i = 1;
        } while (_buffer.step());
        return false;
    }

    bool
    sgxMarkupParser::seekThisLine(const std::string& name)
    {
        return seekThisLine(signatureManager::getSignature(name));
    }

    bool
    sgxMarkupParser::parseBlockName(std::string& res)
    {
        res.clear();
        while (_buffer.step()) {
            if (!isOnSignature("sgx_mrk_name")) {
                switch (_buffer.info().ch()) {
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
                res.append(1, _buffer.info().ch());
            }
        }

        return false;
    }

    std::string
    sgxMarkupParser::parseUnitName()
    {
        std::string name;

        //add errno setup
        while (_buffer.step()) {
            if (isOnSignature("sgx_mrk_var_name"))
            {
                name.append(1, _buffer.info().ch());
            }
            else
            {
                skipSpaces();
                if (!_buffer.info().jumped_to_new_line())
                {
                    if (isOnSignature("sgx_mrk_assign"))
                    {
                        //skip assign sign
                        _buffer.step_back();
                        if (seekSignature("sgx_mrk_assign"))
                        {
                            //skip to value
                            _buffer.step();
                            skipSpaces();
                            _buffer.step_back();
                            return name;
                        }
                        else
                        {
                            _errno = SGXP_NO_VARIABLE_LOAD;
                            return "";
                        }
                    }
                    else
                    {
                        _errno = SGXP_NO_ASSIGN_SIGNATURE;
                        break;
                    }
                }
                else
                {
                    _errno = SGXP_NO_ASSIGN_SIGNATURE;
                    break;
                }
            }
        }

        return "";
    }

    bool
    sgxMarkupParser::parseUnitValue(std::string& value)
    {
        bool single = true;
        value.clear();
        while (_buffer.step()) {
            if (isOnSignature("sgx_mrk_var_value")) {
                value.append(1, _buffer.info().ch());
            } else if (isOnSignature("space")) { /* is array? */
                skipSpaces();
            } else if (isOnSignature("sgx_mrk_array_separator")) {
                value.append(1, _buffer.info().ch());
                single = false;
            } else if (isOnSignature("sgx_mrk_end_line")) {
                break;
            } else {
                _errno = SGXP_AMBIGOUS;
                return "";
            }
        }

        return single;
    }

    bool
    sgxMarkupParser::parseUnit(Unit& u)
    {
        bool brakets_used = false;
        bool name_readed = false;
        bool value_readed = false;
        bool assign_found = false;
        std::string value;

        u.name() = parseUnitName();
        if (u.name().length() <= 0) {
            return false;
        }

        bool isSingle = parseUnitValue(value);

        if (value.length() > 0) {
            if (isSingle) {

            } else {

            }
        } else {
            return false;
        }

        if (isSingle) {
            //define data type
        } else {
            //do all as with single each ","
        }
        u.setAny(value); //todo

        return true;
    }

    //mb logger must be print?
    void
    sgxMarkupParser::print_error()
    {
        int err_line = _buffer.info().notEnd() ? _buffer.info().getpos().row : _buffer.info().getpos().row-1;
        std::cerr << "Error occured during parsing. Parsing file: " << _parsing_file << std::endl;
        std::cerr << "  >> " << "\"" << _buffer[err_line] << "\"" << std::endl;
        std::cerr << "Error number: " << _errno << " Reason: " << sgx_parser_strerror_tab[_errno].name << ": " <<
            sgx_parser_strerror_tab[_errno].description << std::endl << std::endl;
    }

} /* sgx::Markup */ 
