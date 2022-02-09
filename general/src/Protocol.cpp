#include <Protocol.hpp>

namespace siigix {
    class StringSizer
    {
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

    IProtocol::IProtocol(TransSocket& ds) :
        _socket(ds)
    {  }

    IProtocol::~IProtocol()
    {  }

    namespace TCP {
        void
        Protocol::sendMessage(const std::string& data)
        {
            _socket.sendMessage(data.c_str(), data.size());
            /* _socket.sendMessageClose(); */
        }

        void
        Protocol::recvMessage(std::string& data)
        {
            size_t dataRead = 0;
            data.clear();

            while (true)
            {
                StringSizer stringSizer(data, dataRead);
                size_t dataMax = data.capacity() - 1;
                char   *buff   = &data[0];

                size_t got = _socket.recvMessage(buff + dataRead, dataMax - dataRead, [](size_t) { return false; });
                dataRead  += got;

                if (got == 0) {
                    break;
                }

                data.reserve(data.capacity() * 1.5 + 10);
            }
        }

        /* namespace HTTP { */
        /*     Protocol::Protocol(TransSocket& socket) : */
        /*         TCP::Protocol(socket) */
        /*     { */
        /*     } */
        /* } /1* HTTP *1/ */ 

    } /* TCP */ 
} /* siigix */ 
