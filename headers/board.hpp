#pragma once

#include "bitboards.hpp"
#include "castling.hpp"
#include "mailbox.hpp"
#include "misc.hpp"
#include "square.hpp"
#include "types.hpp"
#include <charconv>

namespace chess {
    class Board {
    public:
        constexpr Board(std::string_view fen = START_FEN);

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
            return us(Color::WHITE) | us(Color::BLACK);
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

        constexpr void clearAllPieces() {
            m_pieces.clear();
            m_occupancy.zero();
            for (int i = 0; i < NUM_COLORS; ++i) {
                for (int j = 0; j < NUM_PIECE_TYPES; ++j) {
                    m_bitboards[i][j].zero();
                }
            }
        }

        // Move a piece from one square to another
        constexpr void movePiece(Piece piece, Square from, Square to) {
            m_pieces.move(piece, from, to);
        }

        void setFen(std::string_view fen);

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
    };

    constexpr inline Board::Board(std::string_view fen) {
        setFen(fen);
    }

    inline void Board::setFen(std::string_view fen) {
        while (fen[0] == ' ') {
            fen.remove_prefix(1);
        } // remove leading spaces

        m_ply = 0;
        clearAllPieces();

        std::vector<std::string_view> params = misc::splitString(fen, ' ');

        std::string_view position  = params[0];
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

        for (char c : position) {
            if (charToPiece(c) != Piece::NONE) {
                const Piece piece = charToPiece(c);
                placePiece(piece, square);

                square = Square(square.sq() + 1);
            } else if (c == '/') {
                square = Square(square.sq() - 16);
            } else if (std::isdigit(c)) {
                square = Square(square.sq() + (c - '0'));
            }
        }

        if (enPassant != "-") {
            m_enPassantSq = (enPassant[0] - 'a') + ((enPassant[1] - '1') * 8);
        } else {
            m_enPassantSq = Square::NO_SQ;
        }

        m_castlingRights.clear();

        for (char c : castling) {
            if (c == '-') {
                break;
            }

            if (c == 'K' || c == 'k') {
                m_castlingRights.setCastlingRights(static_cast<Color>(c == 'K'), CastlingSide::KING_SIDE);
            } else if (c == 'Q' || c == 'q') {
                m_castlingRights.setCastlingRights(static_cast<Color>(c == 'Q'), CastlingSide::QUEEN_SIDE);
            }
        }

        m_occupancy = all();
    }

    inline std::ostream& operator<<(std::ostream& os, const Board& board) {
        for (Rank r = Rank::RANK_8; r >= Rank::RANK_1; r--) {
            for (File f = File::FILE_A; f <= File::FILE_H; f++) {
                os << pieceToChar(board.at(Square(f, r))) << ' ';
            }

            os << '\n';
        }

        os << "\nSide to move: " << (board.sideToMove() == Color::WHITE ? "White" : "Black") << '\n';

        os << "\nTurn: " << (board.sideToMove() == Color::WHITE ? "White" : "Black") << '\n';

        if (board.enPassant().sq() != Square::NO_SQ) {
            os << "En Passant: " << board.enPassant() << '\n';
        } else {
            os << "En Passant: None\n";
        }

        auto castlingRights = board.castlingRights();

        os << "Castling Rights: ";
        if (castlingRights.hasCastlingRights(Color::WHITE, CastlingSide::KING_SIDE)) {
            // White can castle kingside
            os << 'K';
        }
        if (castlingRights.hasCastlingRights(Color::WHITE, CastlingSide::QUEEN_SIDE)) {
            // White can castle queenside
            os << 'Q';
        }

        if (castlingRights.hasCastlingRights(Color::BLACK, CastlingSide::KING_SIDE)) {
            // Black can castle kingside
            os << 'k';
        }

        if (castlingRights.hasCastlingRights(Color::BLACK, CastlingSide::QUEEN_SIDE)) {
            // Black can castle queenside
            os << 'q';
        }

        os << '\n';

        os << "Half Move Clock: " << int(board.halfMoveClock()) << '\n';
        os << "Plies: " << board.ply() << '\n';
        // os << "Hash: " << std::hex << b.hash() << std::dec << '\n';

        return os;
    }

} // namespace chess