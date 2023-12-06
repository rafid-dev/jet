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
            return m_squares[sq.sq()];
        }

        constexpr void set(Piece piece, Square sq) {
            m_squares[sq.sq()] = piece;
        }

        constexpr void move(Piece piece, Square from, Square to) {
            m_squares[to.sq()]   = piece;
            m_squares[from.sq()] = Piece::NONE;
        }

        constexpr void clear(Square sq) {
            m_squares[sq.sq()] = Piece::NONE;
        }

        constexpr void clear() {
            m_squares.fill(Piece::NONE);
        }

    private:
        std::array<Piece, 64> m_squares;
    };
}; // namespace chess