#pragma once

#include "../misc/utils.hpp"
#include "attacks.hpp"
#include "bitboards.hpp"
#include "castling.hpp"
#include "mailbox.hpp"
#include "moves.hpp"
#include "square.hpp"
#include "types.hpp"
#include "zobrist.hpp"

#include "fens.hpp"
#include <charconv>

namespace chess {

    class Board {
    public:
        Board(std::string_view fen = FENS::STARTPOS);

        constexpr inline auto ply() const {
            return m_ply;
        }

        constexpr inline auto halfMoveClock() const {
            return m_halfmoveClock;
        }

        inline CastlingRights castlingRights() const {
            return m_castlingRights;
        }

        inline Square enPassant() const {
            return m_enPassantSq;
        }

        inline Color sideToMove() const {
            return m_sideToMove;
        }

        Bitboard occupied() const {
            return m_occupancy;
        }

        // Returns the piece on a square
        Piece at(Square sq) const {
            return m_pieces.get(sq);
        }

        PieceType pieceTypeAt(Square sq) const {
            if (at(sq) == Piece::NONE) {
                return PieceType::NONE;
            }
            return pieceToPieceType(at(sq));
        }

        Bitboard us(Color c) const {
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
        Bitboard us() const {
            // clang-format off
            return m_bitboards[static_cast<int>(c)][static_cast<int>(PieceType::PAWN)] |
                   m_bitboards[static_cast<int>(c)][static_cast<int>(PieceType::KNIGHT)] |
                   m_bitboards[static_cast<int>(c)][static_cast<int>(PieceType::BISHOP)] |
                   m_bitboards[static_cast<int>(c)][static_cast<int>(PieceType::ROOK)] |
                   m_bitboards[static_cast<int>(c)][static_cast<int>(PieceType::QUEEN)] |
                   m_bitboards[static_cast<int>(c)][static_cast<int>(PieceType::KING)];
            // clang-format on
        }

        Bitboard them(Color c) const {
            return us(~c);
        }

        template <Color c>
        Bitboard them() const {
            return us<~c>();
        }

        // Returns all pieces occupied by both colors
        Bitboard all() const {
            return us<Color::WHITE>() | us<Color::BLACK>();
        }

        // Returns the bitboard of a given color and piece type
        template <Color c, PieceType pt>
        Bitboard bitboard() const {
            return m_bitboards[static_cast<int>(c)][static_cast<int>(pt)];
        }

        // Returns the bitboard of a given color and piece type
        Bitboard bitboard(Color c, PieceType pt) const {
            return m_bitboards[static_cast<int>(c)][static_cast<int>(pt)];
        }

        // Returns the bitboard of a given piece type (includes both colors)
        template <PieceType pt>
        Bitboard bitboard() const {
            return bitboard<Color::WHITE, pt>() | bitboard<Color::BLACK, pt>();
        }

        // Returns the bitboard of a given piece type (includes both colors)
        Bitboard bitboard(PieceType pt) const {
            return bitboard(Color::WHITE, pt) | bitboard(Color::BLACK, pt);
        }

        // Place a piece on a square
        void placePiece(Piece piece, Square sq) {
            m_pieces.set(piece, sq);

            // Update bitboards
            m_bitboards[static_cast<int>(pieceToColor(piece))][static_cast<int>(pieceToPieceType(piece))].set(sq);

            // Update occupancy bitboard
            m_occupancy.set(sq);
        }

        // Remove a piece from a square
        void removePiece(Piece piece, Square sq) {
            assert(m_pieces.get(sq) == piece && piece != Piece::NONE);
            m_pieces.clear(sq);

            // Update bitboards
            m_bitboards[static_cast<int>(pieceToColor(piece))][static_cast<int>(pieceToPieceType(piece))].clear(sq);

            // Update occupancy bitboard
            m_occupancy.clear(sq);
        }

        // Move a piece from one square to another
        void movePiece(Piece piece, Square from, Square to) {
            m_pieces.move(piece, from, to);
        }

        // Returns the mailbox of 64 squares containing a piece
        const Mailbox64& pieces() const {
            return m_pieces;
        }

        // set the board to a given fen
        void setFen(std::string_view fen);

        bool isAttacked(Square s, Color c) const {
            if (Attacks::pawnAttacks(s, ~c) & bitboard(c, PieceType::PAWN)) {
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

        Square kingSq(Color c) const {
            return m_bitboards[static_cast<bool>(c)][static_cast<int>(PieceType::KING)].lsb();
        }

        template <Color c>
        constexpr Square kingSq() const {
            return m_bitboards[static_cast<bool>(c)][static_cast<int>(PieceType::KING)].lsb();
        }

        bool isCheck() const {
            return isAttacked(kingSq(sideToMove()), ~sideToMove());
        }

        constexpr bool isCheck(Color c) const {
            return isAttacked(kingSq(c), ~c);
        }

        constexpr bool isCapture(const Move& move) const {
            return at(move.to()) != Piece::NONE;
        }

        constexpr bool isNoisy(const Move& move) const {
            return isCapture(move) || move.type() == MoveType::PROMOTION;
        }

        constexpr bool isQuiet(const Move& move) const {
            return !isNoisy(move);
        }

        constexpr Piece capturedPiece(const Move& move) const {
            return at(move.to());
        }

        constexpr Piece movedPiece(const Move& move) const {
            return at(move.from());
        }

        Color colorOf(const Square sq) const {
            return pieceToColor(at(sq));
        }

        void makeMove(const Move& move);
        void unmakeMove(const Move& move);

        void makeNullMove();
        void unmakeNullMove();

        bool hasNonPawnMat(Color c) const {
            return (bitboard(c, PieceType::KNIGHT) | bitboard(c, PieceType::BISHOP) | bitboard(c, PieceType::ROOK) |
                    bitboard(c, PieceType::QUEEN));
        }

        bool hasNonPawnMat() const {
            return hasNonPawnMat(sideToMove());
        }

        constexpr U64 hash() const {
            return m_hash;
        }

        constexpr U64 genHash() const {
            U64 hash_key = 0;

            Bitboard white = us<Color::WHITE>();
            Bitboard black = us<Color::BLACK>();

            BitboardIterator(white) {
                Square sq = white.poplsb();
                hash_key ^= Zobrist::pieceKey(at(sq), sq);
            }

            BitboardIterator(black) {
                Square sq = black.poplsb();
                hash_key ^= Zobrist::pieceKey(at(sq), sq);
            }

            U64 enpassanthash = 0;
            U64 sidehash      = 0;
            U64 castlehash    = 0;

            if (m_enPassantSq.isValid()) {
                enpassanthash ^= Zobrist::enpassantKey(m_enPassantSq.file());
            }
            if (m_sideToMove == Color::WHITE) {
                sidehash ^= Zobrist::sideKey();
            }

            castlehash ^= Zobrist::castlingKey(m_castlingRights.index());

            return hash_key ^ enpassanthash ^ sidehash ^ castlehash;
        }

        inline Move uciToMove(std::string_view uci) const {
            Square from = Square(uci.substr(0, 2));
            Square to   = Square(uci.substr(2, 2));

            PieceType pt = pieceToPieceType(at(from));

            if (pt == PieceType::PAWN && uci.size() == 5) {
                return Move::makePromotion(from, to, charToPieceType(uci[4]));
            }

            if (pt == PieceType::KING && Square::squareDistance(from, to) == 2) {
                return Move::makeCastling(from, Square(to > from ? File::FILE_H : File::FILE_A, from.rank()));
            }

            if (pt == PieceType::PAWN && to == m_enPassantSq) {
                return Move::makeEnpassant(from, to);
            }

            return Move::makeNormal(from, to);
        }

        bool isRepetition(int count = 2) const {
            int n = 0;

            for (int i = static_cast<int>(m_history.size()) - 2;
                 i >= 0 && i >= static_cast<int>(m_history.size()) - m_halfmoveClock - 1; i -= 2) {
                if (m_history[i].hash == m_hash) {
                    n++;
                }

                if (n == count) {
                    return true;
                }
            }

            return false;
        }

        template <Color c>
        Bitboard attackers(Square sq, Bitboard occ) const {
            Bitboard bishops = bitboard<c, PieceType::BISHOP>() | bitboard<c, PieceType::QUEEN>();
            Bitboard rooks   = bitboard<c, PieceType::ROOK>() | bitboard<c, PieceType::QUEEN>();
            Bitboard knights = bitboard<c, PieceType::KNIGHT>();
            Bitboard pawns   = bitboard<c, PieceType::PAWN>();
            Bitboard kings   = bitboard<c, PieceType::KING>();

            Bitboard dRays  = Attacks::bishopAttacks(sq, occ);
            Bitboard hvRays = Attacks::rookAttacks(sq, occ);

            Bitboard attackers_bb = dRays & bishops;
            attackers_bb |= hvRays & rooks;
            attackers_bb |= Attacks::knightAttacks(sq) & knights;
            attackers_bb |= Attacks::pawnAttacks(sq, c) & pawns;
            attackers_bb |= Attacks::kingAttacks(sq) & kings;

            return attackers_bb;
        }

        Bitboard attackers(Square sq, Bitboard occ) const {
            return attackers<Color::WHITE>(sq, occ) | attackers<Color::BLACK>(sq, occ);
        }

    private:
        struct State {
            CastlingRights m_castlingRights;
            Piece          m_capturedPiece;
            Square         m_enPassantSq;
            int            m_halfmoveClock;
            U64            hash;

            State(CastlingRights castle, Square enPassant, Piece capturedPiece, int halfmoveClock, U64 hash)
                : m_castlingRights(castle)
                , m_capturedPiece(capturedPiece)
                , m_enPassantSq(enPassant)
                , m_halfmoveClock(halfmoveClock)
                , hash(hash) {
            }
        };

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

        // Hash
        U64 m_hash{0};

        // History
        std::vector<State> m_history{};

        void _clearAllPieces() {
            m_pieces.clear();
            m_occupancy.zero();
            for (int i = 0; i < NUM_COLORS; ++i) {
                for (int j = 0; j < NUM_PIECE_TYPES; ++j) {
                    m_bitboards[i][j].zero();
                }
            }
        }

        void _recordState(Piece capturedPiece) {
            m_history.emplace_back(m_castlingRights, m_enPassantSq, capturedPiece, m_halfmoveClock, m_hash);
        }

        Piece _restoreState() {
            const State& state = m_history.back();

            m_castlingRights = state.m_castlingRights;
            m_enPassantSq    = state.m_enPassantSq;
            m_halfmoveClock  = state.m_halfmoveClock;
            m_hash           = state.hash;

            m_history.pop_back();

            return state.m_capturedPiece;
        }
    };

    inline Board::Board(std::string_view fen) {
        setFen(fen);
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
        os << "Hash: " << std::hex << board.hash() << std::dec << std::endl;

        return os;
    }

    inline void Board::makeMove(const Move& move) {
        const Color side = sideToMove();

        const Piece piece = movedPiece(move);
        assert(piece != Piece::NONE);

        const PieceType pt = pieceToPieceType(piece);

        // In this engine, castling is represented as king -> rook square.
        // Therefore, the rook on move.to() must not be treated as a capture.
        const bool isCapture = move.type() != MoveType::CASTLING && at(move.to()) != Piece::NONE;

        const Piece capturedPiece = isCapture ? at(move.to()) : Piece::NONE;

        _recordState(capturedPiece);

        ++m_ply;

        /*
         * Halfmove clock
         *
         * Reset after a pawn move or capture.
         * Otherwise increment it.
         */
        if (pt == PieceType::PAWN || isCapture) {
            m_halfmoveClock = 0;
        } else {
            ++m_halfmoveClock;
        }

        /*
         * Remove the previous en-passant hash and clear the square.
         */
        if (m_enPassantSq.isValid()) {
            m_hash ^= Zobrist::enpassantKey(m_enPassantSq.file());
        }

        m_enPassantSq = Square::NO_SQ;

        /*
         * Helper for changing one castling right while keeping the
         * castling Zobrist key consistent.
         */
        const auto clearCastlingRight = [this](Color color, CastlingSide castlingSide) {
            if (!m_castlingRights.hasCastlingRights(color, castlingSide)) {
                return;
            }

            m_hash ^= Zobrist::castlingKey(m_castlingRights.index());

            m_castlingRights.setCastlingRights(color, castlingSide, false);

            m_hash ^= Zobrist::castlingKey(m_castlingRights.index());
        };

        /*
         * If a rook is captured on its original castling square,
         * remove the corresponding castling right.
         *
         * Do this before removing the captured piece from the board.
         */
        if (isCapture && pieceToPieceType(capturedPiece) == PieceType::ROOK) {
            const Color capturedColor = ~side;

            const Square kingSideRookSquare = CastlingRights::rookFrom(capturedColor, CastlingSide::KING_SIDE);

            const Square queenSideRookSquare = CastlingRights::rookFrom(capturedColor, CastlingSide::QUEEN_SIDE);

            if (move.to() == kingSideRookSquare) {
                clearCastlingRight(capturedColor, CastlingSide::KING_SIDE);
            }

            if (move.to() == queenSideRookSquare) {
                clearCastlingRight(capturedColor, CastlingSide::QUEEN_SIDE);
            }
        }

        /*
         * Remove an ordinary captured piece.
         *
         * En-passant is handled separately because the captured pawn
         * is not located on move.to().
         */
        if (isCapture) {
            removePiece(capturedPiece, move.to());

            m_hash ^= Zobrist::pieceKey(capturedPiece, move.to());
        }

        /*
         * Update castling rights after moving a king or rook.
         */
        if (pt == PieceType::KING) {
            clearCastlingRight(side, CastlingSide::KING_SIDE);

            clearCastlingRight(side, CastlingSide::QUEEN_SIDE);
        } else if (pt == PieceType::ROOK) {
            const Square kingSideRookSquare = CastlingRights::rookFrom(side, CastlingSide::KING_SIDE);

            const Square queenSideRookSquare = CastlingRights::rookFrom(side, CastlingSide::QUEEN_SIDE);

            if (move.from() == kingSideRookSquare) {
                clearCastlingRight(side, CastlingSide::KING_SIDE);
            }

            if (move.from() == queenSideRookSquare) {
                clearCastlingRight(side, CastlingSide::QUEEN_SIDE);
            }
        }

        /*
         * Set an en-passant square after a double pawn push, but only
         * when an opposing pawn can actually capture en passant.
         */
        if (pt == PieceType::PAWN && Square::squareDistance(move.from(), move.to()) == 2) {
            const Square possibleEnPassant = static_cast<Square>(static_cast<int>(move.to()) ^ 8);

            const Bitboard potentialCapturers = Attacks::pawnAttacks(possibleEnPassant, side);

            if (potentialCapturers & bitboard(~side, PieceType::PAWN)) {
                m_enPassantSq = possibleEnPassant;

                m_hash ^= Zobrist::enpassantKey(m_enPassantSq.file());
            }
        }

        /*
         * Apply the move.
         */
        if (move.type() == MoveType::CASTLING) {
            assert(at(move.from()) == makePiece(side, PieceType::KING));

            assert(at(move.to()) == makePiece(side, PieceType::ROOK));

            const CastlingSide castlingSide = CastlingRights::getCastlingSide(move.to(), move.from());

            const Square kingFrom = move.from();
            const Square rookFrom = move.to();

            const Square kingTo = CastlingRights::kingTo(side, castlingSide);

            const Square rookTo = CastlingRights::rookTo(side, castlingSide);

            const Piece king = makePiece(side, PieceType::KING);

            const Piece rook = makePiece(side, PieceType::ROOK);

            removePiece(king, kingFrom);
            removePiece(rook, rookFrom);

            placePiece(king, kingTo);
            placePiece(rook, rookTo);

            m_hash ^= Zobrist::pieceKey(king, kingFrom) ^ Zobrist::pieceKey(king, kingTo);

            m_hash ^= Zobrist::pieceKey(rook, rookFrom) ^ Zobrist::pieceKey(rook, rookTo);

        } else if (move.type() == MoveType::PROMOTION) {
            const Piece promotedPiece = makePiece(side, move.promoted());

            removePiece(piece, move.from());
            placePiece(promotedPiece, move.to());

            m_hash ^= Zobrist::pieceKey(piece, move.from()) ^ Zobrist::pieceKey(promotedPiece, move.to());

        } else {
            removePiece(piece, move.from());
            placePiece(piece, move.to());

            m_hash ^= Zobrist::pieceKey(piece, move.from()) ^ Zobrist::pieceKey(piece, move.to());
        }

        /*
         * Remove the captured pawn for en passant.
         */
        if (move.type() == MoveType::ENPASSANT) {
            const Square capturedPawnSquare = Square(static_cast<int>(move.to()) ^ 8);

            const Piece capturedPawn = makePiece(~side, PieceType::PAWN);

            assert(at(capturedPawnSquare) == capturedPawn);

            removePiece(capturedPawn, capturedPawnSquare);

            m_hash ^= Zobrist::pieceKey(capturedPawn, capturedPawnSquare);
        }

        m_hash ^= Zobrist::sideKey();
        m_sideToMove = ~m_sideToMove;
    }

    inline void Board::unmakeMove(const Move& move) {
        const Piece previouslyCaptured = _restoreState();

        m_sideToMove = ~m_sideToMove;
        --m_ply;

        const Color side = m_sideToMove;

        /*
         * Hash, castling rights, en-passant square, and halfmove clock
         * have already been restored by _restoreState().
         *
         * Only restore the pieces here.
         */
        if (move.type() == MoveType::CASTLING) {
            const CastlingSide castlingSide = CastlingRights::getCastlingSide(move.to(), move.from());

            const Square kingTo = CastlingRights::kingTo(side, castlingSide);

            const Square rookTo = CastlingRights::rookTo(side, castlingSide);

            const Piece king = makePiece(side, PieceType::KING);

            const Piece rook = makePiece(side, PieceType::ROOK);

            assert(at(kingTo) == king);
            assert(at(rookTo) == rook);

            removePiece(king, kingTo);
            removePiece(rook, rookTo);

            placePiece(king, move.from());
            placePiece(rook, move.to());

            return;
        }

        if (move.type() == MoveType::PROMOTION) {
            const Piece promotedPiece = at(move.to());

            assert(pieceToPieceType(promotedPiece) == move.promoted());

            removePiece(promotedPiece, move.to());

            placePiece(makePiece(side, PieceType::PAWN), move.from());

            if (previouslyCaptured != Piece::NONE) {
                placePiece(previouslyCaptured, move.to());
            }

            return;
        }

        const Piece movedPiece = at(move.to());

        assert(movedPiece != Piece::NONE);

        removePiece(movedPiece, move.to());

        placePiece(movedPiece, move.from());

        if (move.type() == MoveType::ENPASSANT) {
            const Square capturedPawnSquare = Square(static_cast<int>(move.to()) ^ 8);

            placePiece(makePiece(~side, PieceType::PAWN), capturedPawnSquare);
        } else if (previouslyCaptured != Piece::NONE) {
            placePiece(previouslyCaptured, move.to());
        }
    }

    inline void Board::makeNullMove() {
        _recordState(Piece::NONE);

        if (m_enPassantSq.isValid()) {
            m_hash ^= Zobrist::enpassantKey(m_enPassantSq.file());
        }

        m_enPassantSq = Square();

        m_ply++;

        m_hash ^= Zobrist::sideKey();
        m_sideToMove = ~m_sideToMove;
    }

    inline void Board::unmakeNullMove() {
        const State& state = m_history.back();

        m_castlingRights = state.m_castlingRights;
        m_enPassantSq    = state.m_enPassantSq;
        m_halfmoveClock  = state.m_halfmoveClock;
        m_hash           = state.hash;

        m_history.pop_back();

        m_ply--;
        m_sideToMove = ~m_sideToMove;
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
        m_hash      = genHash();
        m_history.clear();
        m_history.reserve(256);
    }

} // namespace chess