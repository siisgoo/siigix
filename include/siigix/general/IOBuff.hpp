#ifndef IOBUFF_HPP_OWUP1XLR
#define IOBUFF_HPP_OWUP1XLR

#include <iostream>
#include <sys/types.h>
#include <limits>
#include <string.h>

namespace siigix {
    class IOBuff {
        public:
            IOBuff(size_t cap = 1024);
            IOBuff(const IOBuff& other);
            virtual ~IOBuff();

            operator char*() { return reinterpret_cast<char*>(_buff); }
            operator void*() { return static_cast<void*>(_buff); }
            operator unsigned char*() { return _buff; }

            //todo add chunk
            bool add(void *buff, size_t len, size_t chunk_size = 8);
            bool add(std::string buff);
            bool add(const IOBuff& buff);
            unsigned char *read(size_t offset = -1) const;

            unsigned char *pointer() const;
            bool remove(size_t offset, size_t len);
            bool resize(size_t new_size);
            size_t len()  const;
            size_t size() const;
            bool drop();
        private:
            unsigned char *_buff;
            size_t _size; //alloc bytes
            size_t _len;  //used bytes
    };
} /* siigix */ 

#endif /* end of include guard: IOBUFF_HPP_OWUP1XLR */
