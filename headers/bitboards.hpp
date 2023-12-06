#pragma once

#include "square.hpp"

#include <algorithm>
#include <array>
#include <bitset>
#include <iostream>

namespace chess {
    class Bitboard {
    public:
        constexpr Bitboard() : m_squares(0){};
        constexpr Bitboard(U64 squares) : m_squares(squares) {
        }

        constexpr Bitboard(Square square) : m_squares(1ULL << square.sq()) {
        }

        constexpr Bitboard(File f) : m_squares(MASK_FILE[static_cast<int>(f)]) {
        }

        constexpr Bitboard(Rank r) : m_squares(MASK_RANK[static_cast<int>(r)]) {
        }

        constexpr Bitboard(std::initializer_list<Square> squares) : m_squares(0) {
            for (const auto& sq : squares) {
                set(sq);
            }
        }

        constexpr inline void set(Square square) {
            m_squares |= square.bb();
        }

        constexpr inline void set(File f) {
            m_squares |= MASK_FILE[static_cast<int>(f)];
        }

        constexpr inline void set(Rank r) {
            m_squares |= MASK_RANK[static_cast<int>(r)];
        }

        constexpr inline void zero() {
            m_squares = 0;
        }

        constexpr inline void clear(Square square) {
            m_squares &= ~square.bb();
        }

        constexpr inline void clear(File f) {
            m_squares &= ~MASK_FILE[static_cast<int>(f)];
        }

        constexpr inline void clear(Rank r) {
            m_squares &= ~MASK_RANK[static_cast<int>(r)];
        }

        constexpr inline Square lsb() const {
            return _lsb_internal(m_squares);
        }
        constexpr inline int popcount() const {
            return _popcount_internal(m_squares);
        }

        constexpr inline Square poplsb() {
            return _poplsb_internal(m_squares);
        }

        template <Direction d>
        constexpr inline Bitboard shift() {
            return Bitboard(_shift_internal<d>(m_squares));
        }

        constexpr inline bool empty() const {
            return m_squares == 0;
        }

        constexpr Bitboard operator&(const Bitboard& rhs) const {
            return Bitboard(m_squares & rhs.m_squares);
        }

        constexpr Bitboard operator|(const Bitboard& rhs) const {
            return Bitboard(m_squares | rhs.m_squares);
        }

        constexpr Bitboard operator^(const Bitboard& rhs) const {
            return Bitboard(m_squares ^ rhs.m_squares);
        }

        constexpr Bitboard operator~() const {
            return Bitboard(~m_squares);
        }

        constexpr Bitboard operator<<(int i) const {
            return Bitboard(m_squares << i);
        }

        constexpr Bitboard operator>>(int i) const {
            return Bitboard(m_squares >> i);
        }

        constexpr Bitboard& operator&=(const Bitboard& rhs) {
            m_squares &= rhs.m_squares;
            return *this;
        }

        constexpr Bitboard& operator|=(const Bitboard& rhs) {
            m_squares |= rhs.m_squares;
            return *this;
        }

        constexpr Bitboard& operator^=(const Bitboard& rhs) {
            m_squares ^= rhs.m_squares;
            return *this;
        }

        constexpr Bitboard& operator<<=(int i) {
            m_squares <<= i;
            return *this;
        }

        constexpr Bitboard& operator>>=(int i) {
            m_squares >>= i;
            return *this;
        }

        constexpr bool operator==(const Bitboard& rhs) const {
            return m_squares == rhs.m_squares;
        }

        constexpr bool operator!=(const Bitboard& rhs) const {
            return m_squares != rhs.m_squares;
        }

        constexpr bool operator<(const Bitboard& rhs) const {
            return m_squares < rhs.m_squares;
        }

        constexpr bool operator>(const Bitboard& rhs) const {
            return m_squares > rhs.m_squares;
        }

        constexpr bool operator<=(const Bitboard& rhs) const {
            return m_squares <= rhs.m_squares;
        }

        constexpr bool operator>=(const Bitboard& rhs) const {
            return m_squares >= rhs.m_squares;
        }

        constexpr operator U64() const {
            return m_squares;
        }

        constexpr operator bool() const {
            return m_squares != 0;
        }

    private:
        U64 m_squares;

        template <Direction d>
        static constexpr inline U64 _shift_internal(U64 b) {
            if constexpr (d == Direction::NORTH) {
                return b << 8;
            } else if constexpr (d == Direction::SOUTH) {
                return b >> 8;
            } else if constexpr (d == Direction::EAST) {
                return (b & ~MASK_FILE[7]) << 1;
            } else if constexpr (d == Direction::WEST) {
                return (b & ~MASK_FILE[0]) >> 1;
            } else if constexpr (d == Direction::NORTH_EAST) {
                return (b & ~MASK_FILE[7]) << 9;
            } else if constexpr (d == Direction::NORTH_WEST) {
                return (b & ~MASK_FILE[0]) << 7;
            } else if constexpr (d == Direction::SOUTH_EAST) {
                return (b & ~MASK_FILE[7]) >> 7;
            } else if constexpr (d == Direction::SOUTH_WEST) {
                return (b & ~MASK_FILE[0]) >> 9;
            } else {
                return 0;
            }
        }

        static constexpr inline Square _lsb_internal(U64 b) {
#if defined(__GNUC__) || defined(__clang__)
            return Square(__builtin_ctzll(b));
#elif defined(_MSC_VER)
            unsigned long idx;
            _BitScanForward64(&idx, b);
            return static_cast<Square>(int(idx));
#endif
        }

        static constexpr inline Square _poplsb_internal(U64& b) {
            Square sq = _lsb_internal(b);
            b &= b - 1;
            return sq;
        }

        static constexpr inline int _popcount_internal(U64 b) {
#if defined(_MSC_VER) || defined(__INTEL_COMPILER)
            return (uint8_t) _mm_popcnt_u64(b);
#else
            return __builtin_popcountll(b);
#endif
        }
    };

    inline std::ostream& operator<<(std::ostream& os, const Bitboard& bb) {
        constexpr int squareWidth = 8; // Width of each row

        // Convert the Bitboard to a string representation
        std::bitset<chess::NUM_SQUARES> b(static_cast<U64>(bb));
        std::string                     str_bitset = b.to_string();

        // Output the Bitboard in a more readable format
        for (int i = 0; i < chess::NUM_SQUARES; i += squareWidth) {
            // Extract a row of bits
            std::string row = str_bitset.substr(i, squareWidth);

            // Reverse the row for correct bit order
            std::reverse(row.begin(), row.end());

            // Output the reversed row
            os << row << '\n';
        }

        // Add a newline for better separation
        os << std::endl;

        return os;
    }
} // namespace chess