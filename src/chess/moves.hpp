#pragma once

#include "square.hpp"
#include "types.hpp"

#include <algorithm>
#include <array>
#include <cassert>

namespace chess {

    enum class MoveType : uint16_t { NORMAL, ENPASSANT, CASTLING, PROMOTION };

    struct Move {
    private:
        template <typename T>
        struct MoveBase {
            T from : 6;
            T to : 6;
            T type : 2;
            T promoted : 2;
        };
        MoveBase<uint16_t> m_data;
        int16_t            m_score = 0;

        template <MoveType mt = MoveType::NORMAL>
        static inline constexpr Move _make(Square from, Square to, PieceType promoted = PieceType::KNIGHT) {
            Move m;
            m.m_data.from     = static_cast<uint16_t>(from);
            m.m_data.to       = static_cast<uint16_t>(to);
            m.m_data.type     = static_cast<uint16_t>(mt);
            m.m_data.promoted = _encodePieceType(promoted);
            return m;
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
            return static_cast<Square>(m_data.from);
        }

        constexpr Square to() const {
            return static_cast<Square>(m_data.to);
        }

        constexpr MoveType type() const {
            return MoveType(m_data.type);
        }

        constexpr PieceType promoted() const {
            return _decodePieceType(m_data.promoted);
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

        constexpr void setScore(int16_t score) {
            m_score = score;
        }

        constexpr auto score() const {
            return m_score;
        }

        constexpr auto move() const {
            return m_data;
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

        constexpr void sort() {
            std::sort(m_moves.begin(), m_moves.begin() + m_size,
                      [](const Move& a, const Move& b) { return a.score() > b.score(); });
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

    private:
        static constexpr std::size_t MAX_SIZE = 128;
        int                          m_size   = 0;
        std::array<Move, MAX_SIZE>   m_moves{};
    };

    inline std::ostream& operator<<(std::ostream& os, const Movelist& ml) {
        for (int i = 0; i < ml.size(); ++i) {
            os << ml[i] << ": 1\n";
        }

        os << "Total moves: " << ml.size() << "\n";

        return os;
    }

} // namespace chess
