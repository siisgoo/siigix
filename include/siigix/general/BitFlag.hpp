#ifndef BITFLAG_HPP_FLNKC4B2
#define BITFLAG_HPP_FLNKC4B2

#include <sys/types.h>

namespace siigix {

    typedef __uint8_t  bitflag8_t;
    typedef __uint16_t bitflag16_t;
    typedef __uint32_t bitflag32_t;
    typedef __uint64_t bitflag64_t;
    typedef bitflag8_t bitflag_t; //default

    template<typename Cap>
    class BitFlag {
        public:
            BitFlag(Cap cap) {};
            virtual ~BitFlag() {};

            virtual void set(Cap f)      { _flag |= f; }
            virtual void remove(Cap f)   { _flag &= ~f; }
            virtual bool isSetted(Cap f) { if ((_flag & f) == f) return true; return false; }
            virtual Cap raw()            { return _flag; };
            virtual void clean()         { _flag = 0; }
        private:
            Cap _flag = 0;
    };

} /* siigix */ 

#endif /* end of include guard: BITFLAG_HPP_FLNKC4B2 */
