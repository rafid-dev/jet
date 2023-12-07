#pragma once

#include "types.hpp"
#include <array>
#include <iostream>

namespace chess {

    // clang-format off
    enum class Direction : int8_t { 
        NORTH = 8, 
        WEST = -1, 
        SOUTH = -8, 
        EAST = 1,
        NORTH_EAST = 9, 
        NORTH_WEST = 7, 
        SOUTH_WEST = -9, 
        SOUTH_EAST = -7 
    };

    constexpr Direction relativeDirection(Color c, Direction d) {
        if (c == Color::WHITE) {
            return d;
        } else {
            return static_cast<Direction>(-static_cast<int8_t>(d));
        }
    }

    template <Color c, Direction d>
    constexpr Direction relativeDirection() {
        if constexpr (c == Color::WHITE) {
            return d;
        } else {
            return static_cast<Direction>(-static_cast<int8_t>(d));
        }
    }
    // clang-format on

    enum class File { FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H, NO_FILE };

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

    // Addition overloads
    constexpr File operator+(File f, int i) {
        return static_cast<File>(static_cast<int>(f) + i);
    }

    constexpr Rank operator+(Rank r, int i) {
        return static_cast<Rank>(static_cast<int>(r) + i);
    }

    // Substraction overloads
    constexpr File operator-(File f, int i) {
        return static_cast<File>(static_cast<int>(f) - i);
    }

    constexpr Rank operator-(Rank r, int i) {
        return static_cast<Rank>(static_cast<int>(r) - i);
    }

    // clang-format off
    static constexpr U64 MASK_RANK[8] = {
        0xff, 0xff00, 0xff0000, 0xff000000, 0xff00000000, 0xff0000000000, 0xff000000000000, 0xff00000000000000
    };

    static constexpr U64 MASK_FILE[8] = {
        0x101010101010101, 0x202020202020202, 0x404040404040404, 0x808080808080808, 0x1010101010101010, 0x2020202020202020, 0x4040404040404040, 0x8080808080808080,
    };

    // clang-format on

    class Square {
    private:
        uint8_t m_sq;

        // clang-format off
        static constexpr inline std::string_view squareToString[65] = {
            "a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1",
            "a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
            "a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
            "a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
            "a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
            "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
            "a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
            "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8",
            "no_sq", 
        };
        // clang-format on 

    public:
        // clang-format off
        enum : uint8_t {
             A1,  B1,  C1,  D1,  E1,  F1,  G1,  H1,
             A2,  B2,  C2,  D2,  E2,  F2,  G2,  H2,
             A3,  B3,  C3,  D3,  E3,  F3,  G3,  H3,
             A4,  B4,  C4,  D4,  E4,  F4,  G4,  H4,
             A5,  B5,  C5,  D5,  E5,  F5,  G5,  H5,
             A6,  B6,  C6,  D6,  E6,  F6,  G6,  H6,
             A7,  B7,  C7,  D7,  E7,  F7,  G7,  H7,
             A8,  B8,  C8,  D8,  E8,  F8,  G8,  H8,
            NO_SQ
        };
        // clang-format on

        constexpr Square() : m_sq(NO_SQ) {
        }

        constexpr Square(uint8_t sq) : m_sq(sq) {
        }

        constexpr Square(int file, int rank) : m_sq(rank * 8 + file) {
        }

        constexpr Square(File file, Rank rank) : m_sq(static_cast<int>(rank) * 8 + static_cast<int>(file)) {
        }

        constexpr Square(std::string_view str) : m_sq((str[0] - 'a') + (str[1] - '1') * 8) {
        }

        constexpr inline int sq() const {
            return m_sq;
        }
        constexpr inline U64 bb() const {
            return 1ULL << m_sq;
        }

        constexpr inline File file() const {
            return static_cast<File>(m_sq & 7);
        }

        constexpr inline Rank rank() const {
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

        constexpr inline std::string_view toString() const {
            return squareToString[m_sq];
        }

        constexpr inline bool operator==(const Square& rhs) const {
            return m_sq == rhs.m_sq;
        }

        constexpr inline bool operator!=(const Square& rhs) const {
            return m_sq != rhs.m_sq;
        }

        constexpr inline operator int() const {
            return sq();
        }

        template <Color c>
        static constexpr inline Rank relativeRank(Rank r) {
            if constexpr (c == Color::WHITE) {
                return r;
            } else {
                return static_cast<Rank>(static_cast<int>(r) ^ 7);
            }
        }

        template <Color c>
        static constexpr inline File relativeFile(File f) {
            if constexpr (c == Color::WHITE) {
                return f;
            } else {
                return static_cast<File>(static_cast<int>(f) ^ 7);
            }
        }

        template <Color c, Rank r>
        static constexpr inline Rank relativeRank() {
            if constexpr (c == Color::WHITE) {
                return r;
            } else {
                return static_cast<Rank>(static_cast<int>(r) ^ 7);
            }
        }

        static constexpr inline Rank relativeRank(Color c, Rank r) {
            return static_cast<Rank>(static_cast<int>(r) ^ (static_cast<int>(c) * 7));
        }

        template <Color c, File f>
        static constexpr inline File relativeFile() {
            if constexpr (c == Color::WHITE) {
                return f;
            } else {
                return static_cast<File>(static_cast<int>(f) ^ 7);
            }
        }

        static constexpr inline int squareDistance(Square x, Square y) {
            int dx = std::abs(static_cast<int>(x.file()) - static_cast<int>(y.file()));
            int dy = std::abs(static_cast<int>(x.rank()) - static_cast<int>(y.rank()));
            return std::max(dx, dy);
        }

        static constexpr inline Square relativeSquare(Square sq, Color c) {
            return static_cast<Square>(int(sq) ^ (static_cast<int>(c) * 56));
        }

        template <Color c>
        static constexpr inline Square relativeSquare(Square sq) {
            return static_cast<Square>(int(sq) ^ (static_cast<int>(c) * 56));
        }

        static constexpr inline bool isOurBackRank(Square sq, Color c) {
            return sq.rank() == static_cast<Rank>(static_cast<int>(c) * 7);
        }

        static constexpr inline bool isTheirBackRank(Square sq, Color c) {
            return sq.rank() == relativeRank(~c, Rank::RANK_1);
        }
    };

    // Used to iterate through all squares
    class SquareIterator {
    public:
        static constexpr std::array<Square, NUM_SQUARES> SQUARES = {
            Square::A1, Square::B1, Square::C1, Square::D1, Square::E1, Square::F1, Square::G1, Square::H1,
            Square::A2, Square::B2, Square::C2, Square::D2, Square::E2, Square::F2, Square::G2, Square::H2,
            Square::A3, Square::B3, Square::C3, Square::D3, Square::E3, Square::F3, Square::G3, Square::H3,
            Square::A4, Square::B4, Square::C4, Square::D4, Square::E4, Square::F4, Square::G4, Square::H4,
            Square::A5, Square::B5, Square::C5, Square::D5, Square::E5, Square::F5, Square::G5, Square::H5,
            Square::A6, Square::B6, Square::C6, Square::D6, Square::E6, Square::F6, Square::G6, Square::H6,
            Square::A7, Square::B7, Square::C7, Square::D7, Square::E7, Square::F7, Square::G7, Square::H7,
            Square::A8, Square::B8, Square::C8, Square::D8, Square::E8, Square::F8, Square::G8, Square::H8,
        };

        SquareIterator() = default;

        static constexpr auto begin() {
            return std::begin(SQUARES);
        }

        static constexpr auto end() {
            return std::end(SQUARES);
        }

        static constexpr auto size() {
            return std::size(SQUARES);
        }
    };

    inline std::ostream& operator<<(std::ostream& os, const Square& sq) {
        os << sq.toString();
        return os;
    }

} // namespace chess