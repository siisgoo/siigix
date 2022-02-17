#ifndef CONFIGSIGNATURES_HPP_WIAKZLUE
#define CONFIGSIGNATURES_HPP_WIAKZLUE

// MOVE IT TO SOURCE CODE FILE

#include <functional>
#include <cctype>
#include <siigix/General/eprintf.hpp>
#include <initializer_list>
#include <string>
#include <vector>
#include <regex>
#include <map>
#include <cstring>
#include <functional>


/*
 *
 *       **************
 *       *  ATENTION! *
 *       **************
 *
 * Signatures cannot contain spaces!
 *
 */

namespace sgx {
    class ISignature {
        public:
            ISignature() {  }
            virtual ~ISignature() {  }
            virtual bool isSign(const std::string) const = 0;
            virtual unsigned maxLen() const = 0;
    };

    class signVec : public ISignature {
        private:
            std::map<std::string, unsigned> _map;
            int _max_len;
        public:
            virtual bool isSign(const std::string s) const {
                for (auto& sign: _map) {
                    std::string tmp = s;
                    tmp.resize(sign.second);
                    if (tmp == sign.first) { return true; }
                }
                return false;
            }

            virtual unsigned maxLen() const {
                unsigned max=0;
                for (auto& sign: _map) {
                    if (sign.second > max) max = sign.second;
                }
                return max;
            };

            signVec(const std::vector<std::string>& ilist)
            {
                for (auto& i: ilist) {
                    _map[i] = i.length();
                }
            }

    };

    class signChar : public ISignature {
        private:
            const char _ch;
        public:
            virtual bool isSign(const std::string ch) const {
                return _ch == ch[0];
            }

            virtual unsigned maxLen() const { return 1; };
            signChar(const char ch)
                : _ch(ch) {  }
    };

    class signCharFun : public ISignature {
        private:
            const std::function<bool(const char)> _f;
        public:
            virtual bool isSign(const std::string ch) const {
                return _f(ch[0]);
            }

            virtual unsigned maxLen() const { return 1; }

            signCharFun(const std::function<bool(const char)>& f) : _f(f) {  }
    };

    //singleton
    class signatureManager {
        private:
            static signatureManager * _smInstance;
            std::map<std::string, ISignature*> _signatures = {
                { "space", new signChar(' ') },


                { "sgx_cfg_assign",           new signVec({ ":", "=", ":=" }) },

                //do some thing with this copy
                { "sgx_cfg_var_name",         new signCharFun([](const char ch){ return std::isalnum(ch) || ch == '_'; }) },
                { "sgx_cfg_var_value",        new signCharFun([](const char ch){ return std::isalnum(ch) || ch == '_' || ch == '"'; }) },
                //mean block name, raname??
                { "sgx_cfg_name",             new signCharFun([](const char ch){ return std::isalnum(ch) ||
                                                                                        std::isspace(ch) ||
                                                                                        ch == '-' ||
                                                                                        ch == '_' ||
                                                                                        ch == '.' ||
                                                                                        ch == '@'; }) },

                { "sgx_cfg_end_line",         new signChar(';') },
                { "sgx_cfg_block_start",      new signChar('{') },
                { "sgx_cfg_block_end",        new signChar('}') },
                { "sgx_cfg_block_name_start", new signChar('[') },
                { "sgx_cfg_block_name_end",   new signChar(']') },
                { "sgx_cfg_array_separator",  new signChar(',') },
                /* { "sgx_cfg_array_start",      new signChar('[') }, */
                /* { "sgx_cfg_array_end",        new signChar(']') }, */
                { "sgx_cfg_string",           new signChar('"') },
                { "sgx_cfg_escape_new_line",  new signChar('\\') },
            };
        public:
            signatureManager();

            static const ISignature * getSignature(const std::string& signName);
            static void addSignature(const std::string& name, ISignature* sign);

            virtual ~signatureManager();
    };

} /* sgx  */ 

#endif /* end of include guard: CONFIGSIGNATURES_HPP_WIAKZLUE */
