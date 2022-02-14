#include <ConfigReader.hpp>
#include <ConfigSignatures.hpp>

#include <iostream>
#include <fstream>

namespace sgx {

    ConfigReader::ConfigReader()
        : _signatureManager(new signatureManager)
    {
    }

    ConfigReader::~ConfigReader()
    {
        delete _signatureManager;
    }

    /**********************************************************************
    *                          sgxConfigReader                           *
    **********************************************************************/

    std::unique_ptr<ConfigRoot>
    sgxConfigReader::read(const std::string& path)
    {
        std::unique_ptr<ConfigRoot> config(new ConfigRoot());
        std::ifstream file(path, std::ios::out);
        std::string line;

        /* find block start with {
         * skip spaces before first AlhpaNum character
         */
        while (std::getline(file, line))
        {
        for (int pos = 0; pos > 0 && pos < line.length(); pos++)
        {
            pos = skipSpaces(line);

            /* check states */
            if (rState.isSetted(ReadingState::field_readed)) {
                pos = skipBeforeSignature(line, signatureManager::getSignature("sgx_assign"));
            }
        }
        }

        return config;
    }

    bool
    sgxConfigReader::write(const ConfigRoot& conf, const std::string& path)
    {
        return false;
    }

    int
    sgxConfigReader::skipSpaces(const std::string& line)
    {
        int i = 0;
        for (; i < line.length(); i++) {
            if (line[i] != ' ') {
                return i-1;
            }
        }

        return -1;
    }

    //TODO now only one char signatures can be handled
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

    void
    sgxConfigReader::printErrorLine(const std::string& file, const std::string& err, int err_line)
    {
        std::cerr << "Error in line " << err_line << std::endl <<
            " >> " << err << std::endl <<
            "Terminating reading precess";
    }

} /* sgx  */ 
