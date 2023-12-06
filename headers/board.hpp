#pragma once

#include "attacks.hpp"
#include "bitboards.hpp"
#include "castling.hpp"
#include "mailbox.hpp"
#include "misc.hpp"
#include "moves.hpp"
#include "square.hpp"
#include "types.hpp"

#include "fens.hpp"
#include <charconv>

namespace chess {
    class Board {
    public:
        constexpr Board(std::string_view fen = FENS::STARTPOS);

        constexpr inline auto ply() const {
            return m_ply;
        }

        constexpr inline auto halfMoveClock() const {
            return m_halfmoveClock;
        }

        constexpr inline CastlingRights castlingRights() const {
            return m_castlingRights;
        }

        constexpr inline Square enPassant() const {
            return m_enPassantSq;
        }

        constexpr inline Color sideToMove() const {
            return m_sideToMove;
        }

        constexpr Bitboard occupied() const {
            return m_occupancy;
        }

        // Returns the piece on a square
        constexpr Piece at(Square sq) const {
            return m_pieces.get(sq);
        }

        constexpr Bitboard us(Color c) const {
            // clang-format off
            return m_bitboards[static_cast<int>(c)][static_cast<int>(PieceType::PAWN)] |
                   m_bitboards[static_cast<int>(c)][static_cast<int>(PieceType::KNIGHT)] |
                   m_bitboards[static_cast<int>(c)][static_cast<int>(PieceType::BISHOP)] |
                   m_bitboards[static_cast<int>(c)][static_cast<int>(PieceType::ROOK)] |
                   m_bitboards[static_cast<int>(c)][static_cast<int>(PieceType::QUEEN)] |
                   m_bitboards[static_cast<int>(c)][static_cast<int>(PieceType::KING)];
            // clang-format on
        }

        template <Color c>
        constexpr Bitboard us() const {
            // clang-format off
            return m_bitboards[static_cast<int>(c)][static_cast<int>(PieceType::PAWN)] |
                   m_bitboards[static_cast<int>(c)][static_cast<int>(PieceType::KNIGHT)] |
                   m_bitboards[static_cast<int>(c)][static_cast<int>(PieceType::BISHOP)] |
                   m_bitboards[static_cast<int>(c)][static_cast<int>(PieceType::ROOK)] |
                   m_bitboards[static_cast<int>(c)][static_cast<int>(PieceType::QUEEN)] |
                   m_bitboards[static_cast<int>(c)][static_cast<int>(PieceType::KING)];
            // clang-format on
        }

        constexpr Bitboard them(Color c) const {
            return us(~c);
        }

        template <Color c>
        constexpr Bitboard them() const {
            return us<~c>();
        }

        // Returns all pieces occupied by both colors
        constexpr Bitboard all() const {
            return us<Color::WHITE>() | us<Color::BLACK>();
        }

        // Returns the piece on a square
        template <Color c, PieceType pt>
        constexpr Bitboard bitboard() const {
            return m_bitboards[static_cast<int>(c)][static_cast<int>(pt)];
        }

        // Returns the piece on a square
        constexpr Bitboard bitboard(Color c, PieceType pt) const {
            return m_bitboards[static_cast<int>(c)][static_cast<int>(pt)];
        }

        // Place a piece on a square
        constexpr void placePiece(Piece piece, Square sq) {
            m_pieces.set(piece, sq);

            // Update bitboards
            m_bitboards[static_cast<int>(pieceToColor(piece))][static_cast<int>(pieceToPieceType(piece))].set(sq);

            // Update occupancy bitboard
            m_occupancy.set(sq);
        }

        // Remove a piece from a square
        constexpr void removePiece(Piece piece, Square sq) {
            m_pieces.clear(sq);

            // Update bitboards
            m_bitboards[static_cast<int>(pieceToColor(piece))][static_cast<int>(pieceToPieceType(piece))].clear(sq);

            // Update occupancy bitboard
            m_occupancy.clear(sq);
        }

        // Move a piece from one square to another
        constexpr void movePiece(Piece piece, Square from, Square to) {
            m_pieces.move(piece, from, to);
        }

        // Returns the mailbox of 64 squares containing a piece
        constexpr const Mailbox64& pieces() const {
            return m_pieces;
        }

        // set the board to a given fen
        void setFen(std::string_view fen);

        constexpr bool isAttacked(Square s, Color c) const {
            if (Attacks::pawnAttacks(s, c) & bitboard(c, PieceType::PAWN)) {
                return true;
            }

            if (Attacks::knightAttacks(s) & bitboard(c, PieceType::KNIGHT)) {
                return true;
            }

            if (Attacks::bishopAttacks(s, occupied()) & (bitboard(c, PieceType::BISHOP) | bitboard(c, PieceType::QUEEN))) {
                return true;
            }

            if (Attacks::rookAttacks(s, occupied()) & (bitboard(c, PieceType::ROOK) | bitboard(c, PieceType::QUEEN))) {
                return true;
            }

            if (Attacks::kingAttacks(s) & bitboard(c, PieceType::KING)) {
                return true;
            }

            return false;
        }

        constexpr Square kingSq(Color c) const {
            return m_bitboards[static_cast<int>(c)][static_cast<int>(PieceType::KING)].lsb();
        }

        constexpr bool isCheck() const {
            return isAttacked(kingSq(sideToMove()), ~sideToMove());
        }

        constexpr bool isCheck(Color c) const {
            return isAttacked(kingSq(c), ~c);
        }

        constexpr void makeMove(Move move);
        constexpr void unmakeMove(Move move);

    private:
        // Bitboards for each color , corressponding to each piece type
        Bitboard m_bitboards[NUM_COLORS][NUM_PIECE_TYPES]{};

        // Mailbox of 64 squares containing a piece
        Mailbox64 m_pieces{};

        // Occupancy bitboard
        Bitboard m_occupancy{};

        // Side to move
        Color m_sideToMove{Color::NO_COLOR};

        // Castling rights
        CastlingRights m_castlingRights;

        // En passant square
        Square m_enPassantSq{Square::NO_SQ};

        // Halfmove clock
        int m_halfmoveClock{0};

        // Ply count
        int m_ply{0};

        constexpr void _clearAllPieces() {
            m_pieces.clear();
            m_occupancy.zero();
            for (int i = 0; i < NUM_COLORS; ++i) {
                for (int j = 0; j < NUM_PIECE_TYPES; ++j) {
                    m_bitboards[i][j].zero();
                }
            }
        }
    };

    constexpr inline Board::Board(std::string_view fen) {
        setFen(fen);
    }

    inline void Board::setFen(std::string_view fen) {
        while (fen[0] == ' ') {
            fen.remove_prefix(1);
        } // remove leading spaces

        m_ply = 0;
        _clearAllPieces();

        std::vector<std::string_view> params = misc::splitString(fen, ' ');

        std::string_view pssition  = params[0];
        std::string_view side      = params[1];
        std::string_view castling  = params[2];
        std::string_view enPassant = params[3];

        if (params.size() > 4) {
            std::from_chars(params[4].data(), params[4].data() + params[4].size(), m_halfmoveClock);
        } else {
            m_halfmoveClock = 0;
        }

        if (params.size() > 5) {
            std::from_chars(params[5].data(), params[5].data() + params[5].size(), m_ply);
        } else {
            m_ply = 0;
        }

        m_sideToMove = side == "w" ? Color::WHITE : Color::BLACK;

        if (m_sideToMove == Color::BLACK) {
            m_ply++;
        }

        Square square = Square(56);

        for (char c : pssition) {
            if (charToPiece(c) != Piece::NONE) {
                const Piece piece = charToPiece(c);
                placePiece(piece, square);

                square = Square(square + 1);

            } else if (c == '/') {
                square = Square(square - 16);
            } else if (c >= '1' && c <= '8') {
                square = Square(square + (c - '0'));
            }
        }

        if (enPassant != "-") {
            m_enPassantSq = Square(enPassant);
        } else {
            m_enPassantSq = Square();
        }

        m_castlingRights.loadFromString(castling);

        m_occupancy = all();
    }

    inline std::ostream& operator<<(std::ostream& os, const Board& board) {
        os << board.pieces().toString() << '\n';
        os << "\nSide to move: " << (board.sideToMove() == Color::WHITE ? "White" : "Black") << '\n';

        if (board.enPassant().isValid()) {
            os << "En Passant: " << board.enPassant() << '\n';
        } else {
            os << "En Passant: None\n";
        }

        os << "Castling Rights: ";
        os << board.castlingRights().toString();
        os << '\n';

        os << "Half Move Clock: " << board.halfMoveClock() << '\n';
        os << "Plies: " << board.ply() << '\n';
        return os;
    }

} // namespace chess