#pragma once

#include "square.hpp"
#include <iostream>

namespace chess {

    using U64 = uint64_t;

    inline Square lsb(U64 bb);

#if defined(__GNUC__) || defined(__clang__)
    inline Square lsb(U64 b) {
        return Square(__builtin_ctzll(b));
    }

    inline Square msb(U64 b) {
        return Square(63 ^ __builtin_clzll(b));
    }

#elif defined(_MSC_VER)

#    ifdef _WIN64
#        include <intrin.h>
    inline Square lsb(U64 b) {
        unsigned long idx;
        _BitScanForward64(&idx, b);
        return static_cast<Square>(int(idx));
    }

    inline Square msb(U64 b) {
        unsigned long idx;
        _BitScanReverse64(&idx, b);
        return static_cast<Square>(int(idx));
    }

#    else

#        error "MSVC 32-bit not supported."

#    endif

#else

#    error "Compiler not supported."

#endif

    inline int popcount(U64 b) {
#if defined(_MSC_VER) || defined(__INTEL_COMPILER)
        return (uint8_t) _mm_popcnt_u64(b);

#else
        return __builtin_popcountll(b);

#endif
    }

    inline Square poplsb(U64& b) {
        Square sq = lsb(b);
        b &= b - 1;
        return sq;
    }

    inline U64 makeBitboard(Square sq) {
        return U64(1ULL) << sq;
    }

} // namespace chess