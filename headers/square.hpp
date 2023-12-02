#pragma once

#include "types.hpp"

namespace chess {

    enum class File { NO_FILE = -1, FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H };

    enum class Rank { RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8, NO_RANK };

    // Pre-increment overloads
    constexpr File& operator++(File& f) {
        return f = static_cast<File>(static_cast<int>(f) + 1);
    }

    constexpr Rank& operator++(Rank& r) {
        return r = static_cast<Rank>(static_cast<int>(r) + 1);
    }

    // Pre-decrement overloads
    constexpr File& operator--(File& f) {
        return f = static_cast<File>(static_cast<int>(f) - 1);
    }

    constexpr Rank& operator--(Rank& r) {
        return r = static_cast<Rank>(static_cast<int>(r) - 1);
    }

    // Post-increment overloads
    constexpr File operator++(File& f, int) {
        File old = f;
        ++f;
        return old;
    }

    constexpr Rank operator++(Rank& r, int) {
        Rank old = r;
        ++r;
        return old;
    }

    // Post-decremnt overloads
    constexpr File operator--(File& f, int) {
        File old = f;
        --f;
        return old;
    }

    constexpr Rank operator--(Rank& r, int) {
        Rank old = r;
        --r;
        return old;
    }

    class Square {
    private:
        uint8_t m_sq;

    public:
        // clang-format off
        enum : uint8_t {
            SQ_A1, SQ_B1, SQ_C1, SQ_D1, SQ_E1, SQ_F1, SQ_G1, SQ_H1,
            SQ_A2, SQ_B2, SQ_C2, SQ_D2, SQ_E2, SQ_F2, SQ_G2, SQ_H2,
            SQ_A3, SQ_B3, SQ_C3, SQ_D3, SQ_E3, SQ_F3, SQ_G3, SQ_H3,
            SQ_A4, SQ_B4, SQ_C4, SQ_D4, SQ_E4, SQ_F4, SQ_G4, SQ_H4,
            SQ_A5, SQ_B5, SQ_C5, SQ_D5, SQ_E5, SQ_F5, SQ_G5, SQ_H5,
            SQ_A6, SQ_B6, SQ_C6, SQ_D6, SQ_E6, SQ_F6, SQ_G6, SQ_H6,
            SQ_A7, SQ_B7, SQ_C7, SQ_D7, SQ_E7, SQ_F7, SQ_G7, SQ_H7,
            SQ_A8, SQ_B8, SQ_C8, SQ_D8, SQ_E8, SQ_F8, SQ_G8, SQ_H8,
            NO_SQ
        };
        // clang-format on

        constexpr Square() : m_sq(NO_SQ) {
        }

        constexpr Square(uint8_t sq) : m_sq(sq) {
        }

        constexpr Square(int file, int rank) : m_sq(rank * 8 + file) {
        }

        constexpr Square(int sq) : m_sq(sq) {
        }

        constexpr Square(uint64_t sq) : m_sq(static_cast<uint8_t>(sq)) {
        }

        constexpr Square(File file, Rank rank) : m_sq(static_cast<int>(rank) * 8 + static_cast<int>(file)) {
        }

    public:
        constexpr File file() const {
            return static_cast<File>(m_sq & 7);
        }

        constexpr Rank rank() const {
            return static_cast<Rank>(m_sq >> 3);
        }

        constexpr uint8_t index() const {
            return m_sq;
        }

        constexpr int diagonal() const {
            return 7 + int(file()) - int(rank());
        }

        constexpr int antiDiagonal() const {
            return int(file()) + int(rank());
        }

        constexpr bool backRank(Color c) const {
            if (c == Color::WHITE) {
                return rank() == Rank::RANK_1;
            } else {
                return rank() == Rank::RANK_8;
            }
        }

        constexpr bool isValid() const {
            return m_sq != NO_SQ;
        }

        constexpr bool isLight() const {
            return (static_cast<int>(file()) + static_cast<int>(rank())) % 2 == 0;
        }

        constexpr bool isDark() const {
            return !isLight();
        }

        constexpr int operator<<(int i) const {
            return m_sq << i;
        }

        constexpr int operator>>(int i) const {
            return m_sq >> i;
        }

        // overloads
        constexpr operator int() const {
            return m_sq;
        }

        constexpr Square operator+(int i) const {
            return Square(m_sq + i);
        }

        constexpr Square operator-(int i) const {
            return Square(m_sq - i);
        }

        constexpr Square& operator+=(int i) {
            m_sq += i;
            return *this;
        }

        constexpr Square& operator-=(int i) {
            m_sq -= i;
            return *this;
        }

        constexpr Square& operator++() {
            ++m_sq;
            return *this;
        }

        constexpr Square& operator--() {
            --m_sq;
            return *this;
        }

        constexpr Square operator++(int) {
            Square old = *this;
            ++m_sq;
            return old;
        }

        constexpr Square operator--(int) {
            Square old = *this;
            --m_sq;
            return old;
        }

        constexpr bool operator==(const Square& other) const {
            return m_sq == other.m_sq;
        }

        constexpr bool operator!=(const Square& other) const {
            return m_sq != other.m_sq;
        }

        constexpr bool operator<(const Square& other) const {
            return m_sq < other.m_sq;
        }

        constexpr bool operator>(const Square& other) const {
            return m_sq > other.m_sq;
        }

        constexpr bool operator<=(const Square& other) const {
            return m_sq <= other.m_sq;
        }

        constexpr bool operator>=(const Square& other) const {
            return m_sq >= other.m_sq;
        }

        constexpr Square operator~() const {
            return Square(63 - m_sq);
        }

        constexpr bool operator<(const int& other) const {
            return m_sq < other;
        }

        constexpr bool operator>(const int& other) const {
            return m_sq > other;
        }

        constexpr bool operator<=(const int& other) const {
            return m_sq <= other;
        }

        constexpr bool operator>=(const int& other) const {
            return m_sq >= other;
        }

        // operator overloads with uint8_t
        constexpr bool operator==(uint8_t other) const {
            return m_sq == other;
        }

        constexpr bool operator!=(uint8_t other) const {
            return m_sq != other;
        }

        // operator overloads for use with Direction
        constexpr Square operator+(Direction dir) const {
            return Square(m_sq + static_cast<int>(dir));
        }

        constexpr Square operator-(Direction dir) const {
            return Square(m_sq - static_cast<int>(dir));
        }

        constexpr Square& operator+=(Direction dir) {
            m_sq += static_cast<int>(dir);
            return *this;
        }

        constexpr Square& operator-=(Direction dir) {
            m_sq -= static_cast<int>(dir);
            return *this;
        }
    };

    inline std::ostream& operator<<(std::ostream& os, const Square& sq) {
        os << squareToString[sq];
        return os;
    }

    inline int squareDistance(Square x, Square y) {
        int dx = std::abs(static_cast<int>(x.file()) - static_cast<int>(y.file()));
        int dy = std::abs(static_cast<int>(x.rank()) - static_cast<int>(y.rank()));
        return std::max(dx, dy);
    }

    constexpr Square relativeSquare(Square sq, Color c) {
        return static_cast<Square>(static_cast<int>(sq) ^ (static_cast<int>(c) * 56));
    }

    constexpr bool ourBackRank(Square sq, Color c) {
        return sq.rank() == static_cast<Rank>(static_cast<int>(c) * 7);
    }
} // namespace chess