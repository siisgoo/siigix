#include <IOBuff.hpp>

using namespace siigix;

IOBuff::IOBuff() :
    _len(0), _size(0),
    _buff(nullptr)
{}

IOBuff::IOBuff(const IOBuff& other)
{
    _buff = new __uint8_t [other.size()];
    memcpy(_buff, other.read(), other.len());
}

IOBuff::~IOBuff()
{  }

//todo add chunk
bool
IOBuff::add(void *buff, size_t len, size_t chunk_size) {
    //im not loose somesing?
    if (len == 0) {
        return false;
    } else if (buff == nullptr) {
        return false;
    } else if (_size <= _len+len) { //do non allocate another memeory
    } else if (!resize(_size+len)) { //allocate a little bit
        return false;
    }
    memcpy(_buff+_len, buff, len);
    _len += len;
    return true;
}

__uint8_t *
IOBuff::read(size_t offset) const {
    if (offset < 0) {
        return _buff;
    } else {
        return _buff+offset;
    }
    return nullptr;
}

bool
IOBuff::remove(size_t offset, size_t len) {
    if (len == 0) { //zochem togda, ya luchhe popiu chi?
    } else if (len + offset > _size) {
        return false;
    } else { // need resize
        _size -= len;
    }
    return true;
}

bool
IOBuff::resize(size_t new_size) {
    if (new_size <= 0) { //clean
        _size = _len = 0;
        delete [] _buff;
        _buff = nullptr;
    } else if (_size != new_size) { //buff havve been with zero size
        __uint8_t *tmp = new (std::nothrow)__uint8_t [new_size];
        if (tmp) {
            /* size_t toalloc = new_size < _len ? new_size : _len; //do not allocate bigger then need */
            _size = new_size;
            memcpy(tmp, _buff, _size);
            memset(_buff, 0, _size);
            delete[] _buff;
            _buff = tmp;
        } else {
            return false;
        }
    } else {
        //critical error, wtf you doing?
        return false;
    }

    return true;
}

__uint8_t* IOBuff::pointer() const { return _buff; } //CARE, may be kause to vare big arror
size_t IOBuff::len()  const { return _len; }
size_t IOBuff::size() const { return _size; }
bool   IOBuff::drop()       { return resize(0); }
