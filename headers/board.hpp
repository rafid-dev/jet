#pragma once

#include "attacks.hpp"
#include "bitboards.hpp"
#include "misc.hpp"
#include "move.hpp"
#include "types.hpp"

#include <cassert>
#include <charconv>

namespace chess {

    enum CastlingSide : uint8_t { KING_SIDE, QUEEN_SIDE };

    class Board {
    public:
        int makeMoveCalled   = 0;
        int unmakeMoveCalled = 0;
        using CastlingRights = std::array<std::array<bool, 2>, 2>;

    private:
        std::array<Piece, NUM_SQUARES> m_pieces{};
        Bitboard                       m_bitboards[2][NUM_PIECE_TYPES]{};

        Color  m_turn      = Color::WHITE;
        Square m_enPassant = Square(Square::NO_SQ);

        uint8_t m_halfMoveClock = 0;
        int     m_plies         = 0;

        // Castling rights
        // bool m_castlingRights[2][2] = {{true, true}, {true, true}};
        CastlingRights m_castlingRights{};

        // Zobrist Hash
        U64 m_hash  = 0;
        U64 occ_all = 0;

        std::string m_fen;

        struct State {
            U64            hash           = 0;
            CastlingRights castling       = {};
            Square         enpassant      = Square(Square::NO_SQ);
            uint8_t        half_moves     = 0;
            Piece          captured_piece = Piece::NONE;

            State(const U64& hash, const CastlingRights& castling, const Square& enpassant, const uint8_t& half_moves, const Piece& captured_piece)
                : hash(hash), castling(castling), enpassant(enpassant), half_moves(half_moves), captured_piece(captured_piece) {
            }
        };

        std::vector<State> m_states;

    public:
        Board(std::string_view fen = START_FEN);

        constexpr Bitboard bitboard(Color c, PieceType pt) const {
            return m_bitboards[static_cast<int>(c)][static_cast<int>(pt)];
        }

        constexpr Bitboard bitboard(PieceType pt) const {
            return bitboard(Color::WHITE, pt) | bitboard(Color::BLACK, pt);
        }

        constexpr Bitboard bitboard(Piece p) const {
            return bitboard(pieceToColor(p), pieceToPieceType(p));
        }

        void makeMove(const Move& move);
        bool makeMovePsuedoLegal(const Move& move);
        void unmakeMove(const Move& move);

        constexpr bool isAttacked(Square sq, Color c) const {
            if (attacks::pawn(sq, ~c) & bitboard(c, PieceType::PAWN)) {
                return true;
            }
            if (attacks::knight(sq) & bitboard(c, PieceType::KNIGHT)) {
                return true;
            }
            if (attacks::king(sq) & bitboard(c, PieceType::KING)) {
                return true;
            }

            if (attacks::bishop(sq, occ_all) & (bitboard(c, PieceType::BISHOP) | bitboard(c, PieceType::QUEEN))) {
                return true;
            }
            if (attacks::rook(sq, occ_all) & (bitboard(c, PieceType::ROOK) | bitboard(c, PieceType::QUEEN))) {
                return true;
            }
            return false;
        }

        constexpr Bitboard us(Color color) const {
            return bitboard(color, PieceType::PAWN) | bitboard(color, PieceType::KNIGHT) | bitboard(color, PieceType::BISHOP) | bitboard(color, PieceType::ROOK) | bitboard(color, PieceType::QUEEN) |
                   bitboard(color, PieceType::KING);
        }

        constexpr Bitboard them(Color color) const {
            return us(~color);
        }

        constexpr Bitboard all() const {
            return us(Color::WHITE) | us(Color::BLACK);
        }

        constexpr Bitboard occupied() const {
            return occ_all;
        }

        constexpr Square kingSq(Color c) const {
            return lsb(bitboard(c, PieceType::KING));
        }

        constexpr Square enPassant() const {
            return m_enPassant;
        }

        constexpr Piece at(Square sq) const {
            return m_pieces[sq];
        }

        constexpr Color sideToMove() const {
            return m_turn;
        }

        constexpr auto castlingRights() const {
            return m_castlingRights;
        }

        constexpr bool hasCastlingRights(Color c, CastlingSide side) const {
            return m_castlingRights[static_cast<int>(c)][static_cast<int>(side)];
        }

        constexpr bool hasCastlingRights(Color c) const {
            return m_castlingRights[static_cast<int>(c)][KING_SIDE] || m_castlingRights[static_cast<int>(c)][QUEEN_SIDE];
        }

        constexpr uint8_t halfMoveClock() const {
            return m_halfMoveClock;
        }

        constexpr int ply() const {
            return m_plies;
        }

        constexpr void setFen(std::string_view fen);

        constexpr void placePiece(Piece p, Square sq);
        constexpr void removePiece(Piece p, Square sq);
    };

    constexpr void Board::placePiece(Piece p, Square sq) {
        assert(m_pieces[sq] == Piece::NONE);

        m_pieces[sq] = p;
        m_bitboards[static_cast<int>(pieceToColor(p))][static_cast<int>(pieceToPieceType(p))] |= (1ULL << sq);
        occ_all |= (1ULL << sq);
    }

    constexpr void Board::removePiece(Piece p, Square sq) {
        assert(m_pieces[sq] != Piece::NONE);

        m_pieces[sq] = Piece::NONE;
        m_bitboards[static_cast<int>(pieceToColor(p))][static_cast<int>(pieceToPieceType(p))] &= ~(1ULL << sq);
        occ_all &= ~(1ULL << sq);
    }

    inline Board::Board(std::string_view fen) {
        setFen(fen);
    }

    constexpr void Board::setFen(std::string_view fen) {
        m_fen = fen;

        m_plies = 0;

        std::fill(m_pieces.begin(), m_pieces.end(), Piece::NONE);

        while (fen[0] == ' ') {
            fen.remove_prefix(1);
        } // remove leading spaces

        occ_all = 0ULL;

        for (const auto c : {Color::WHITE, Color::BLACK}) {
            for (const auto pt : {PieceType::PAWN, PieceType::KNIGHT, PieceType::BISHOP, PieceType::ROOK, PieceType::QUEEN, PieceType::KING}) {
                m_bitboards[static_cast<int>(c)][static_cast<int>(pt)] = 0ULL;
            }
        }

        std::vector<std::string_view> params = misc::split(fen, ' ');

        std::string_view position  = params[0];
        std::string_view side      = params[1];
        std::string_view castling  = params[2];
        std::string_view enPassant = params[3];

        if (params.size() > 4) {
            std::from_chars(params[4].data(), params[4].data() + params[4].size(), m_halfMoveClock);
        } else {
            m_halfMoveClock = 0;
        }

        if (params.size() > 5) {
            std::from_chars(params[5].data(), params[5].data() + params[5].size(), m_plies);
        } else {
            m_plies = 0;
        }

        m_turn = side == "w" ? Color::WHITE : Color::BLACK;

        if (m_turn == Color::BLACK) {
            m_plies++;
        }

        Square square = Square(56);

        for (char c : position) {
            if (charToPiece(c) != Piece::NONE) {
                const Piece piece = charToPiece(c);
                placePiece(piece, square);

                // m_hash ^= zobrist::pieceKey(piece, square);

                square = Square(square + 1);
            } else if (c == '/') {
                square = Square(square - 16);
            } else if (std::isdigit(c)) {
                square = Square(square + (c - '0'));
            }
        }

        if (enPassant != "-") {
            m_enPassant = Square((enPassant[0] - 'a') + ((enPassant[1] - '1') * 8));
        } else {
            m_enPassant = Square(Square::NO_SQ);
        }

        std::memset(m_castlingRights.data(), 0, sizeof(m_castlingRights));

        for (char c : castling) {
            if (c == '-') {
                break;
            }

            if (c == 'K' || c == 'k') {
                m_castlingRights[static_cast<int>(c == 'k')][0] = true;
            } else if (c == 'Q' || c == 'q') {
                m_castlingRights[static_cast<int>(c == 'q')][1] = true;
            }
        }

        occ_all = all();

        m_states.clear();
        m_states.reserve(256);
    }

    inline std::ostream& operator<<(std::ostream& os, const Board& b) {
        for (Rank r = Rank::RANK_8; r >= Rank::RANK_1; r--) {
            for (File f = File::FILE_A; f <= File::FILE_H; f++) {
                os << pieceToChar(b.at(Square(f, r))) << ' ';
            }

            os << '\n';
        }

        os << "\nTurn: " << (b.sideToMove() == Color::WHITE ? "White" : "Black") << '\n';

        if (b.enPassant() != Square(Square::NO_SQ)) {
            os << "En Passant: " << b.enPassant() << '\n';
        } else {
            os << "En Passant: None\n";
        }

        auto castlingRights = b.castlingRights();

        os << "Castling Rights: ";

        if (castlingRights[0][0]) {
            // White can castle kingside
            os << 'K';
        }
        if (castlingRights[0][1]) {
            // White can castle queenside
            os << 'Q';
        }

        if (castlingRights[1][0]) {
            // Black can castle kingside
            os << 'k';
        }

        if (castlingRights[1][1]) {
            // Black can castle queenside
            os << 'q';
        }

        os << '\n';

        os << "Half Move Clock: " << int(b.halfMoveClock()) << '\n';
        os << "Plies: " << b.ply() << '\n';

        return os;
    }

    inline void Board::makeMove(const Move& move) {
        const bool      capture  = at(move.to()) != Piece::NONE && move.type() != Move::CASTLING;
        const Piece     captured = at(move.to());
        const PieceType pt       = pieceToPieceType(at(move.from()));

        m_states.emplace_back(m_hash, m_castlingRights, m_enPassant, m_halfMoveClock, captured);

        m_halfMoveClock++;
        m_plies++;

        makeMoveCalled++;

        m_enPassant = Square(Square::NO_SQ);

        if (capture) {
            m_halfMoveClock = 0;

            removePiece(captured, move.to());

            const Rank rank = move.to().rank();

            if (pieceToPieceType(captured) == PieceType::ROOK && ((rank == Rank::RANK_1 && m_turn == Color::BLACK) || (rank == Rank::RANK_8 && m_turn == Color::WHITE))) {
                const auto king_sq = kingSq(~m_turn);
                const auto side    = move.to() > king_sq ? CastlingSide::KING_SIDE : CastlingSide::QUEEN_SIDE;

                if (hasCastlingRights(~m_turn, side)) {
                    m_castlingRights[static_cast<int>(~m_turn)][static_cast<int>(side)] = false;
                }
            }
        }

        if (pt == PieceType::KING && hasCastlingRights(m_turn)) {
            m_castlingRights[static_cast<int>(m_turn)][static_cast<int>(CastlingSide::KING_SIDE)]  = false;
            m_castlingRights[static_cast<int>(m_turn)][static_cast<int>(CastlingSide::QUEEN_SIDE)] = false;
        } else if (pt == PieceType::ROOK && ourBackRank(move.from(), m_turn)) {
            const auto king_sq = kingSq(m_turn);
            const auto side    = move.from() > king_sq ? CastlingSide::KING_SIDE : CastlingSide::QUEEN_SIDE;

            if (hasCastlingRights(m_turn, side)) {
                m_castlingRights[static_cast<int>(m_turn)][static_cast<int>(side)] = false;
            }
        } else if (pt == PieceType::PAWN) {
            m_halfMoveClock = 0;

            const auto possible_ep = static_cast<Square>(int(move.to()) ^ 8);

            if (std::abs(int(move.to()) - int(move.from())) == 16) {
                Bitboard ep_mask = attacks::pawn(possible_ep, m_turn);

                if (ep_mask & bitboard(~m_turn, PieceType::PAWN)) {
                    m_enPassant = possible_ep;
                }
            }
        }

        if (move.type() == Move::CASTLING) {
            bool king_side = move.to() > move.from();

            auto rookTo = relativeSquare(king_side ? Square(Square::SQ_F1) : Square(Square::SQ_D1), m_turn);
            auto kingTo = relativeSquare(king_side ? Square(Square::SQ_G1) : Square(Square::SQ_C1), m_turn);

            const auto king = at(move.from());
            const auto rook = at(move.to());

            removePiece(king, move.from());
            removePiece(rook, move.to());

            placePiece(king, kingTo);
            placePiece(rook, rookTo);
        } else if (move.type() == Move::PROMOTION) {
            const auto piece_pawn = colorPiece(m_turn, PieceType::PAWN);
            const auto piece      = colorPiece(m_turn, move.promoted());

            removePiece(piece_pawn, move.from());
            placePiece(piece, move.to());
        } else {
            const auto piece = at(move.from());

            removePiece(piece, move.from());
            placePiece(piece, move.to());
        }

        if (move.type() == Move::ENPASSANT) {
            const auto piece = colorPiece(~m_turn, PieceType::PAWN);

            removePiece(piece, Square(int(move.to()) ^ 8));
        }

        m_turn = ~m_turn;
    }

    inline bool Board::makeMovePsuedoLegal(const Move& move) {
        makeMove(move);

        if (isAttacked(kingSq(~m_turn), m_turn)) {
            unmakeMove(move);
            return false;
        }

        return true;
    }

    inline void Board::unmakeMove(const Move& move) {
        const auto prev = m_states.back();
        m_states.pop_back();

        m_hash           = prev.hash;
        m_enPassant      = prev.enpassant;
        m_halfMoveClock  = prev.half_moves;
        m_castlingRights = prev.castling;

        m_plies--;

        m_turn = ~m_turn;

        unmakeMoveCalled++;

        if (move.type() == Move::CASTLING) {
            const bool king_side = move.to() > move.from();

            const Rank backRank   = m_turn == Color::WHITE ? Rank::RANK_1 : Rank::RANK_8;
            const auto rookFromSq = Square(king_side ? File::FILE_F : File::FILE_D, backRank);
            const auto kingToSq   = Square(king_side ? File::FILE_G : File::FILE_C, backRank);

            const auto rook = at(rookFromSq);
            const auto king = at(kingToSq);

            removePiece(rook, rookFromSq);
            removePiece(king, kingToSq);

            placePiece(rook, move.to());
            placePiece(king, move.from());

            return;
        } else if (move.type() == Move::PROMOTION) {
            const auto piece_pawn = colorPiece(m_turn, PieceType::PAWN);
            const auto piece      = at(move.to());

            removePiece(piece, move.to());
            placePiece(piece_pawn, move.from());

            if (prev.captured_piece != Piece::NONE) {
                placePiece(prev.captured_piece, move.to());
            }

            return;

        } else {
            const auto piece = at(move.to());

            removePiece(piece, move.to());
            placePiece(piece, move.from());
        }

        if (move.type() == Move::ENPASSANT) {
            const auto pawn   = colorPiece(~m_turn, PieceType::PAWN);
            const auto pawnTo = static_cast<Square>(int(move.to()) ^ 8);

            placePiece(pawn, pawnTo);

        } else if (prev.captured_piece != Piece::NONE) {
            placePiece(prev.captured_piece, move.to());
        }
    }
} // namespace chess