#include <IOBuff.hpp>
#include <string.h>

namespace siigix {

IOBuff::IOBuff(size_t cap) :
    _len(0), _size(cap),
    _buff(nullptr)
{
    resize(cap);
}

IOBuff::IOBuff(const IOBuff& other)
{
    resize(other.size());
    memcpy(_buff, other.read(), other.len());
}

IOBuff::~IOBuff()
{  }

//todo add chunk
bool
IOBuff::add(void *buff, size_t len, size_t chunk_size) {
    //im not loose somesing?
    if (len == 0) {
        return true;
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

bool
IOBuff::add(std::string buff)
{
    char str[buff.length()];
    memcpy(str, buff.c_str(), buff.length());

    return add(&str, buff.length());
}

bool
IOBuff::add(const IOBuff& buff)
{
    size_t req_size = _len + buff.len();
    if (buff.len() <= 0) { /* do nothing */
        return true;
    } else if (_size < req_size) { /* need resize */
        if (!resize(req_size)) {
            throw "Cant resize IOBuff";
        }
    }
    ::memcpy(_buff + _len, buff.pointer(), buff.len());
    return true;
}

unsigned char *
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
        unsigned char *tmp = new (std::nothrow)unsigned char [new_size];
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

unsigned char* IOBuff::pointer() const { return _buff; } //CARE, may be kause to vare big arror
size_t IOBuff::len()  const { return _len; }
size_t IOBuff::size() const { return _size; }
bool   IOBuff::drop()       { return resize(0); }

} /* siigix */
