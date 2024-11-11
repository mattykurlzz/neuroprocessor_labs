#ifndef _TYPES_BSO_H
#define _TYPES_BSO_H

#include <math.h>
#include <ostream>
#include <stdint.h>
#include <systemc>

struct __attribute__((packed, aligned(1))) fixed32_parted {
    int8_t wholeValue      : 8;
    uint32_t fractionValue : 24;
};

union fixed32_data {
    int32_t wholenum;
    struct fixed32_parted partedNum;
};

struct fixed32_t { // формат числа с фиксированной точкой
    fixed32_data value;

    fixed32_t();
    fixed32_t(int32_t rhs);

    fixed32_t abs();

    fixed32_t& operator=(const fixed32_t& rhs);
    fixed32_t& operator=(const double& rhs);
    fixed32_t& operator+=(const fixed32_t& rhs);
    fixed32_t& operator-=(const fixed32_t& rhs);
    fixed32_t operator+(const fixed32_t& rhs);
    fixed32_t operator+(const int& rhs);
    fixed32_t operator-(const fixed32_t& rhs);
    fixed32_t operator*(const fixed32_t& rhs) const;
    fixed32_t operator/(const fixed32_t& rhs) const;
    fixed32_t operator-() const;
    bool operator==(const fixed32_t& rhs);
    std::ostream& operator<<(std::ostream& os);
};

namespace sc_core {
inline void
sc_trace(sc_trace_file*& f, const fixed32_t& val, std::string name) {
    sc_trace(f, val.value.wholenum, name);
}
} // namespace sc_core

bool sum_overflow(const fixed32_t, const fixed32_t);
bool mul_overflow(const fixed32_t, const fixed32_t);

#endif /*_TYPES_BSO_H*/