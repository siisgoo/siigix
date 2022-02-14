#include <Protocol.hpp>

namespace sgx {
    namespace Net {

        class StringSizer {
            std::string&    stringData;
            std::size_t&    currentSize;
            public:
                StringSizer(std::string& stringData, std::size_t& currentSize) :
                    stringData(stringData), currentSize(currentSize)
                {
                    stringData.resize(stringData.capacity());
                }
                ~StringSizer()
                {
                    stringData.resize(currentSize);
                }
                void incrementSize(std::size_t amount)
                {
                    currentSize += amount;
                }
        };

        /**********************************************************************
        *                           ProtocolSimple                           *
        **********************************************************************/

        void
        ProtocolSimple::sendMessage(TransSocket& sock, const std::string& data)
        {
            sock.sendBytes(data.c_str(), data.size());
            /* sock.sendCloseMessage(); */
        }

        void
        ProtocolSimple::reciveMessage(TransSocket& sock, std::string& data)
        {
            std::size_t dataRead = 0;
            data.clear();

            while (true)
            {
                StringSizer stringSizer(data, dataRead);
                std::size_t dataMax = data.capacity() - 1;
                char   *buff        = &data[0];

                size_t got = sock.reciveBytes(buff + dataRead, dataMax - dataRead, [](size_t) { return false; });
                dataRead  += got;

                if (got == 0) {
                    break;
                }

                data.reserve(data.capacity() * 1.5 + 10);
            }
        }

        /**********************************************************************
        *                            ProtocolHTTP                            *
        **********************************************************************/

        void
        ProtocolHTTP::sendMessage(TransSocket& sock, const std::string& data)
        {

        }

        void
        ProtocolHTTP::reciveMessage(TransSocket& sock, std::string& data)
        {

        }

    } /* Net */ 
} /* sgx */ 
