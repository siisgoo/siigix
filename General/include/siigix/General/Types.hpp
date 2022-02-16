#ifndef TYPES_HPP_I4HURKXF
#define TYPES_HPP_I4HURKXF

#include <stdexcept>
#if defined(__linux__)
#include <sys/types.h>
#else
#include <Windows.h>
#endif

#include "eprintf.hpp"
#include <utility>
#include <vector>
#include <string>
#include <cinttypes>

namespace sgx {
    using int8    = std::int8_t;
    using uint8   = std::uint8_t;
    using int16   = std::int16_t;
    using uint16  = std::uint16_t;
    using int32   = std::int32_t;
    using uint32  = std::uint32_t;
    using int64   = std::int64_t;
    using uint64  = std::uint64_t;
    using intPtr  = std::intptr_t;
    using uintPtr = std::uintptr_t;
    using Byte    = uint8;

    template<typename T>
    struct start_end {
        T start, end;
        T diff() const { return end-start; }
        start_end() : start_end(0, 0) {}
        start_end(T s, T e) : start(s), end(e) {}
    };

    class charVectorBuffer : public std::vector<char> {
        public:
            bool getline(const unsigned i, std::string& ret) {
                ret.clear();
                for (unsigned col = 0, row = 0; col < this->size(); col++) {
                    ret.append(sizeof(char), this->at(col));
                    if (this->at(col) == '\n') {
                        if (row == i) {
                            return true;
                        }
                        ret.clear();
                        row++;
                    }
                }

                return false;
            };

            /* get defined chars from start to end in line line_n
             * return count of writen chars */
            int getline(const unsigned line_n, const unsigned start, const unsigned end, std::string& ret) {
                ret.clear();
                if (start > end) {
                    throw std::runtime_error(eprintf("charVectorBuffer::", __func__, " Cant read chars from: ", start, " to: ", end));
                } else if (start == end) { /* one char need */
                    ret.append(1, this->at(start));
                    return 1;
                }

                std::string source;
                if (!this->getline(line_n, source)) {
                    return 0;
                }

                int readed = 0;
                for (int col = start; col < end && col < source.length(); col++, readed++) {
                    ret.append(1, source[col]);
                }

                return readed;
            }
            int getline(const unsigned line_n, const start_end<unsigned> s_e, std::string& ret) { return getline(line_n, s_e.start, s_e.end, ret); }

            charVectorBuffer& addline(const std::string& s) {
                int c;
                for (c = 0; c < s.length(); c++) {
                    this->push_back(s.at(c));
                }
                if (s.at(c) != '\n') { this->push_back('\n'); }
                return *this;
            }
            charVectorBuffer& addchar(const char c) { this->push_back(c); return *this; }
    };

    //mb remove it with stringPosition??
    struct linePosition {
        int row, col;

        linePosition& operator = (const linePosition& other) {
            row = other.row;
            col = other.col;
            return *this;
        }

        bool operator == (const linePosition& other) const { return row == other.row && col == other.col; }
        bool operator != (const linePosition& other) const { return !(*this == other); }
        bool operator >  (const linePosition& other) const { return row > other.row && col > other.col; }
        bool operator <  (const linePosition& other) const { return row < other.row && col < other.col; }
        bool operator >= (const linePosition& other) const { return (*this == other) || (*this > other); }
        bool operator <= (const linePosition& other) const { return (*this == other) || (*this < other); }

        linePosition() : linePosition(0, 0) {  }
        linePosition(int r, int c) : row(r), col(c) {  }
    };

} /* sgx  */ 

#endif /* end of include guard: TYPES_HPP_I4HURKXF */
