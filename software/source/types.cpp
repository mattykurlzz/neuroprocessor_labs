#include "types.hpp"

bool
sum_overflow(const fixed32_t addr1, const fixed32_t addr2) {
    int64_t test = ((int64_t)addr1.value.wholenum) + ((int64_t)addr2.value.wholenum);
    for (int i = 31; i < 62; ++i) {
        if (test & (1 << i)) {
            return true;
        }
    }
    return false;
}

bool
mul_overflow(const fixed32_t mul1, const fixed32_t mul2) {
    int64_t test = ((int64_t)mul1.value.wholenum) * ((int64_t)mul1.value.wholenum);
    for (int i = 31; i < 62; ++i) {
        if (test & (1 << i)) {
            return true;
        }
    }
    return false;
}

fixed32_t::fixed32_t() { value.wholenum = 0; }

fixed32_t::fixed32_t(int32_t rhs) { value.wholenum = rhs; }

fixed32_t&
fixed32_t::operator=(const fixed32_t& rhs) {
    value = rhs.value;
    return *this;
}

fixed32_t&
fixed32_t::operator=(const double& rhs) {
    if (rhs >= 0) {
        value.partedNum.wholeValue = (int8_t)std::floor(rhs);
        value.partedNum.fractionValue = ((uint32_t)((rhs - floor(rhs)) * (2 ^ 24))) << 8 >> 8;
    }
    return *this;
}

fixed32_t&
fixed32_t::operator+=(const fixed32_t& rhs) {
    *this = *this + rhs; // as fixed-point num obeys integer arithmetics
    return *this;
}

fixed32_t&
fixed32_t::operator-=(const fixed32_t& rhs) {
    *this = *this - rhs; // as fixed-point num obeys integer arithmetics
    return *this;
}

fixed32_t
fixed32_t::operator+(const fixed32_t& rhs) {
    fixed32_t ret(value.wholenum);
    if (sum_overflow(rhs, *this)) {
        ret.value.wholenum = INT32_MAX;
    } else {
        ret.value.wholenum += rhs.value.wholenum; // as fixed-point num obeys integer arithmetics
    }
    return ret;
}

fixed32_t
fixed32_t::operator+(const int& rhs) {
    fixed32_t ret(value.wholenum);
    if (sum_overflow(*this, fixed32_t(1))) {
        ret.value.wholenum = INT32_MAX;
    } else {
        ret.value.wholenum += rhs; // as fixed-point num obeys integer arithmetics
    }
    return ret;
}

fixed32_t
fixed32_t::operator-() const {
    fixed32_t ret(-value.wholenum);
    return ret;
}

fixed32_t
fixed32_t::operator-(const fixed32_t& rhs) {
    return *this + (-rhs);
}

bool
fixed32_t::operator==(const fixed32_t& rhs) {
    return value.wholenum == rhs.value.wholenum;
}

fixed32_t
fixed32_t::operator*(const fixed32_t& rhs) const {
    fixed32_t ret;
    if (mul_overflow(rhs, *this)) {
        ret.value.wholenum = INT32_MAX;
    } else {
        ret.value.wholenum = value.wholenum * rhs.value.wholenum;
    }
    return ret;
}

fixed32_t
fixed32_t::operator/(const fixed32_t& rhs) const {
    fixed32_t ret;
    ret.value.wholenum = (int32_t)((((int64_t)value.wholenum) << 30) / ((int64_t)rhs.value.wholenum));
    return ret;
}

fixed32_t
fixed32_t::abs() {
    if (value.wholenum >= 0) {
        return *this;
    } else {
        return *this * (-1);
    }
}

std::ostream&
fixed32_t::operator<<(std::ostream& os) {
    float print = 0;
    print += value.partedNum.wholeValue;
    print += value.partedNum.fractionValue / (2 ^ 24);
    os << print;
    return os;
}
