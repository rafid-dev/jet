#pragma once

#include "square.hpp"
#include "types.hpp"

#include <algorithm>
#include <array>
#include <cassert>
#include <type_traits>

namespace chess {

    enum class MoveType : uint16_t { NORMAL = 0, ENPASSANT, CASTLING, PROMOTION };

    struct Move {
    private:
        uint16_t m_data;
        int16_t  m_score;

        template <MoveType mt = MoveType::NORMAL>
        static inline constexpr Move _make(Square from, Square to, PieceType promoted = PieceType::KNIGHT) {
            if constexpr (mt != MoveType::PROMOTION) {
                return Move((static_cast<uint16_t>(from) << 10) | (static_cast<uint16_t>(to) << 4) |
                            (static_cast<uint16_t>(mt)));
            } else {
                return Move((static_cast<uint16_t>(from) << 10) | (static_cast<uint16_t>(to) << 4) |
                            (_encodePieceType(promoted) << 2) | (static_cast<uint16_t>(mt)));
            }
        }

        static constexpr uint8_t _encodePieceType(PieceType pt) {
            assert(static_cast<uint8_t>(pt) >= static_cast<uint8_t>(PieceType::KNIGHT) &&
                   static_cast<uint8_t>(pt) <= static_cast<uint8_t>(PieceType::QUEEN));
            return static_cast<uint8_t>(pt) - 1;
        }

        static constexpr PieceType _decodePieceType(uint8_t pt) {
            assert(pt >= 0 && pt <= 3);
            return static_cast<PieceType>(pt + 1);
        }

    public:
        Move() = default;
        constexpr Move(uint16_t data) : m_data(data), m_score(0) {
        }

        static inline constexpr Move none() {
            return Move(0);
        }

        static inline constexpr Move nullmove() {
            return _make<MoveType::NORMAL>(63, 63);
        }

        static inline constexpr Move makeNormal(Square from, Square to) {
            return _make<MoveType::NORMAL>(from, to);
        }

        static inline constexpr Move makeEnpassant(Square from, Square to) {
            return _make<MoveType::ENPASSANT>(from, to);
        }

        static inline constexpr Move makeCastling(Square from, Square to) {
            return _make<MoveType::CASTLING>(from, to);
        }

        static inline constexpr Move makePromotion(Square from, Square to, PieceType promoted) {
            return _make<MoveType::PROMOTION>(from, to, promoted);
        }

        constexpr Square from() const {
            return static_cast<Square>(m_data >> 10);
        }

        constexpr Square to() const {
            return static_cast<Square>((m_data >> 4) & 0x3f);
        }

        constexpr MoveType type() const {
            return static_cast<MoveType>(m_data & 0x3);
        }

        constexpr PieceType promoted() const {
            return _decodePieceType((m_data >> 2) & 0x3);
        }

        constexpr bool isPromotion() const {
            return type() == MoveType::PROMOTION;
        }

        constexpr bool isEnPassant() const {
            return type() == MoveType::ENPASSANT;
        }

        constexpr bool isCastling() const {
            return type() == MoveType::CASTLING;
        }

        constexpr auto move() const {
            return m_data;
        }

        constexpr bool isValid() const {
            return !(from() == to());
        }

        constexpr void setScore(int16_t s) {
            m_score = s;
        }

        constexpr auto score() const {
            return m_score;
        }

        constexpr bool operator==(const Move& rhs) const {
            return m_data == rhs.m_data;
        }
    };

    inline std::ostream& operator<<(std::ostream& os, const Move& m) {
        os << m.from();
        if (m.type() == MoveType::CASTLING) {
            os << (Square(m.to() > m.from() ? File::FILE_G : File::FILE_C, m.from().rank()));
        } else {
            os << m.to();
        }

        if (m.isPromotion()) {
            os << pieceTypeToChar(m.promoted());
        }

        return os;
    }

    struct Movelist {
    public:
        Movelist() = default;

        Movelist(std::initializer_list<Move> moves) : m_size(moves.size()) {
            assert(moves.size() <= MAX_SIZE);
            std::copy(moves.begin(), moves.end(), m_moves.begin());
        }

        constexpr void add(Move m) {
            assert(m_size < MAX_SIZE);
            m_moves[m_size++] = m;
        }

        constexpr void addIf(Move m, bool condition) {
            assert(m_size < MAX_SIZE);
            if (!condition) {
                return;
            }
            m_moves[m_size++] = m;
        }

        constexpr void addPromotions(Square from, Square to) {
            add(Move::makePromotion(from, to, PieceType::KNIGHT));
            add(Move::makePromotion(from, to, PieceType::BISHOP));
            add(Move::makePromotion(from, to, PieceType::ROOK));
            add(Move::makePromotion(from, to, PieceType::QUEEN));
        }

        constexpr void clear() {
            m_size = 0;
        }

        constexpr void setSize(int size) {
            m_size = size;
        }

        constexpr int size() const {
            return m_size;
        }

        constexpr Move operator[](int i) const {
            assert(i >= 0 && i < m_size);
            return m_moves[i];
        }

        constexpr Move& operator[](int i) {
            assert(i >= 0 && i < m_size);
            return m_moves[i];
        }

        constexpr Move& front() {
            assert(m_size > 0);
            return m_moves[0];
        }

        constexpr Move& back() {
            assert(m_size > 0);
            return m_moves[m_size - 1];
        }

        constexpr Move front() const {
            assert(m_size > 0);
            return m_moves[0];
        }

        constexpr Move back() const {
            assert(m_size > 0);
            return m_moves[m_size - 1];
        }

        constexpr bool empty() const {
            return m_size == 0;
        }

        constexpr void pop_back() {
            assert(m_size > 0);
            --m_size;
        }

        void nextmove(const int move_index) {
            Move temp;

            int     i         = 0;
            int16_t bestscore = INT16_MIN;

            for (int j = move_index; j < m_size; ++j) {
                if (m_moves[j].score() > bestscore) {
                    bestscore = m_moves[j].score();
                    i         = j;
                }
            }

            temp                = m_moves[move_index];
            m_moves[move_index] = m_moves[i];
            m_moves[i]          = temp;
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

        static constexpr int MAX_SIZE = 128;

    private:
        int                        m_size = 0;
        std::array<Move, MAX_SIZE> m_moves{};
    };

    inline std::ostream& operator<<(std::ostream& os, const Movelist& ml) {
        for (int i = 0; i < ml.size(); ++i) {
            os << ml[i] << ": 1\n";
        }

        os << "Total moves: " << ml.size() << "\n";

        return os;
    }

} // namespace chess
