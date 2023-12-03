#pragma once

#include "square.hpp"
#include "types.hpp"

#include <algorithm>
#include <array>
#include <iostream>

namespace chess {

    // Implementation taken from Disservin/chess-library
    struct Move {
    public:
        static constexpr uint16_t NO_MOVE   = 0;
        static constexpr uint16_t NULL_MOVE = 65;
        static constexpr uint16_t NORMAL    = 0;
        static constexpr uint16_t PROMOTION = 1 << 14;
        static constexpr uint16_t ENPASSANT = 2 << 14;
        static constexpr uint16_t CASTLING  = 3 << 14;

        Move() = default;
        constexpr Move(uint16_t move) : m_move(move), m_score(0) {
        }

        template <uint16_t MoveType = 0>
        static constexpr Move make(Square source, Square target, PieceType pt = PieceType::KNIGHT) {
            return Move(MoveType + ((uint16_t(pt) - uint16_t(PieceType::KNIGHT)) << 12) + uint16_t(source << 6) + uint16_t(target));
        }

        constexpr Square from() const {
            return static_cast<Square>((m_move >> 6) & 0x3F);
        }

        constexpr Square to() const {
            return static_cast<Square>(m_move & 0x3F);
        }

        constexpr uint16_t type() const {
            return static_cast<uint16_t>(m_move & (3 << 14));
        }

        constexpr PieceType promoted() const {
            return static_cast<PieceType>(((m_move >> 12) & 3) + static_cast<int>(PieceType::KNIGHT));
        }

        constexpr bool isPromotion() const {
            return type() == PROMOTION;
        }

        constexpr void setScore(int16_t score) {
            m_score = score;
        }

        constexpr uint16_t move() const {
            return m_move;
        }
        constexpr int16_t score() const {
            return m_score;
        }

        bool operator==(const Move& rhs) const {
            return m_move == rhs.m_move;
        }
        bool operator!=(const Move& rhs) const {
            return m_move != rhs.m_move;
        }

    private:
        uint16_t m_move;
        int16_t  m_score;
    };

    constexpr std::ostream& operator<<(std::ostream& os, const Move& move) {
        Square from = move.from();
        Square to   = move.to();

        if (move.type() == Move::CASTLING) {
            to = Square(to > from ? File::FILE_G : File::FILE_C, from.rank());
        }

        os << from;
        os << to;

        if (move.isPromotion()) {
            os << pieceTypeToChar(move.promoted());
        }
        return os;
    }

    struct Movelist {
    private:
        std::array<Move, MAX_MOVES> m_moves{};
        int                         m_size = 0;

    public:
        constexpr void add(Move move) {
            m_moves[m_size++] = move;
        }

        constexpr std::size_t size() const {
            return m_size;
        }

        constexpr bool empty() const {
            return m_size == 0;
        }

        constexpr void clear() {
            m_size = 0;
        }

        constexpr Move& front() {
            return m_moves[0];
        }

        constexpr const Move& front() const {
            return m_moves[0];
        }

        constexpr Move& back() {
            return m_moves[m_size - 1];
        }

        constexpr const Move& back() const {
            return m_moves[m_size - 1];
        }

        constexpr void sort() {
            std::sort(m_moves.begin(), m_moves.begin() + m_size, [](const Move& a, const Move& b) { return a.score() > b.score(); });
        }

        constexpr Move operator[](std::size_t index) const {
            return m_moves[index];
        }

        constexpr Move& operator[](std::size_t index) {
            return m_moves[index];
        }

        using iterator       = Move*;
        using const_iterator = const Move*;

        constexpr iterator begin() {
            return m_moves.data();
        }

        constexpr iterator end() {
            return m_moves.data() + m_size;
        }

        constexpr const_iterator begin() const {
            return m_moves.data();
        }

        constexpr const_iterator end() const {
            return m_moves.data() + m_size;
        }
    };

} // namespace chess