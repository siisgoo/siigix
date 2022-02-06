#ifndef IOBUFF_HPP_OWUP1XLR
#define IOBUFF_HPP_OWUP1XLR

#include <iostream>
#include <sys/types.h>
#include <limits>
#include <string.h>

class IOBuff {
    public:
        IOBuff();
        IOBuff(const IOBuff& other);
        virtual ~IOBuff();

        //todo add chunk
        bool add(void *buff, size_t len, size_t chunk_size = 8);
        __uint8_t *read(size_t offset = -1) const;

        __uint8_t *pointer() const;
        bool remove(size_t offset, size_t len);
        bool resize(size_t new_size);
        size_t len()  const;
        size_t size() const;
        bool drop();
    private:
        __uint8_t *_buff;
        size_t _size; //alloc bytes
        size_t _len;  //used bytes
};

#endif /* end of include guard: IOBUFF_HPP_OWUP1XLR */
