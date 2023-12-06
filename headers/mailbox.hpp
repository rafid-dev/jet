#pragma once

#include "bitboards.hpp"
#include "square.hpp"
#include "types.hpp"

#include <array>

namespace chess {

    // A simple mailbox of 64 squares containing a piece
    class Mailbox64 {
    public:
        Mailbox64() = default;

        constexpr Piece get(Square sq) const {
            return m_squares[sq];
        }

        constexpr void set(Piece piece, Square sq) {
            m_squares[sq] = piece;
        }

        constexpr void move(Piece piece, Square from, Square to) {
            m_squares[to]   = piece;
            m_squares[from] = Piece::NONE;
        }

        constexpr void clear(Square sq) {
            m_squares[sq] = Piece::NONE;
        }

        constexpr void clear() {
            m_squares.fill(Piece::NONE);
        }

        inline std::string toString() const {
            std::string str;
            for (Rank r = Rank::RANK_8; r >= Rank::RANK_1; r--) {
                for (File f = File::FILE_A; f <= File::FILE_H; f++) {
                    Square sq    = Square(f, r);
                    Piece  piece = get(sq);
                    str += ' ';
                    str += pieceToChar(piece);
                }
                str += '\n';
            }

            return str;
        }

    private:
        std::array<Piece, 64> m_squares;
    };

}; // namespace chess