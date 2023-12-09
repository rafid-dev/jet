#pragma once

#include "square.hpp"

#include <algorithm>
#include <array>
#include <bitset>
#include <immintrin.h>
#include <iostream>

namespace chess {

#define BitboardIterator(X) for (; X;)

    // Bit manipulation functions
    namespace bitops {
        static constexpr inline void resetLowestBit(U64& b) {
            // b = _blsr_u64(b);
            b &= b - 1;
        }
        constexpr inline int popcount(U64 b) {
            // return static_cast<int>(_mm_popcnt_u64(b));
            return __builtin_popcountll(b); // let compiler choose
        }

        static constexpr inline Square lsb(U64 b) {
            // #if defined(__GNUC__) || defined(__clang__)
            // return Square(__builtin_ctzll(b));
            // #elif defined(_MSC_VER)
            //             unsigned long idx;
            //             _BitScanForward64(&idx, b);
            //             return static_cast<Square>(int(idx));
            // #endif
            return static_cast<Square>(_tzcnt_u64(b));
        }

        static constexpr inline Square poplsb(U64& b) {
            Square sq = lsb(b);
            resetLowestBit(b);
            return sq;
        }
    } // namespace bitops

    class Bitboard {
    public:
        constexpr Bitboard() : m_squares(0){};
        constexpr Bitboard(U64 squares) : m_squares(squares) {
        }

        constexpr Bitboard(Square square) : m_squares(1ULL << static_cast<uint8_t>(square)) {
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
            return bitops::lsb(m_squares);
        }
        constexpr inline int popcount() const {
            return bitops::popcount(m_squares);
        }

        constexpr inline Square poplsb() {
            return bitops::poplsb(m_squares);
        }

        template <Direction d>
        constexpr inline Bitboard shift() {
            return Bitboard(_shift_internal<d>(m_squares));
        }

        constexpr inline bool empty() const {
            return m_squares == 0;
        }

        constexpr inline bool single() const {
            return popcount() == 1;
        }

        constexpr inline bool multiple() const {
            return popcount() > 1;
        }

        constexpr Bitboard operator+(const Bitboard& rhs) const {
            return Bitboard(m_squares + rhs.m_squares);
        }

        constexpr Bitboard operator-(const Bitboard& rhs) const {
            return Bitboard(m_squares - rhs.m_squares);
        }

        constexpr Bitboard operator*(const Bitboard& rhs) const {
            return Bitboard(m_squares * rhs.m_squares);
        }

        constexpr Bitboard operator*(const bool& rhs) const {
            return Bitboard(m_squares * rhs);
        }

        constexpr Bitboard operator*(const U64& rhs) const {
            return Bitboard(m_squares * rhs);
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

            // Output the reversed row with spaces between each number
            for (char bit : row) {
                os << bit << ' ';
            }

            // Add a newline after each row
            os << '\n';
        }

        // Add a newline for better separation
        os << std::endl;

        return os;
    }

} // namespace chess