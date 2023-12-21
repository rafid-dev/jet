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

        constexpr PieceType pieceTypeAt(Square sq) const {
            return pieceToPieceType(at(sq));
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

        // Returns the bitboard of a given color and piece type
        template <Color c, PieceType pt>
        constexpr Bitboard bitboard() const {
            return m_bitboards[static_cast<int>(c)][static_cast<int>(pt)];
        }

        // Returns the bitboard of a given color and piece type
        constexpr Bitboard bitboard(Color c, PieceType pt) const {
            return m_bitboards[static_cast<int>(c)][static_cast<int>(pt)];
        }

        // Returns the bitboard of a given piece type (includes both colors)
        template <PieceType pt>
        constexpr Bitboard bitboard() const {
            return bitboard<Color::WHITE, pt>() | bitboard<Color::BLACK, pt>();
        }

        // Returns the bitboard of a given piece type (includes both colors)
        constexpr Bitboard bitboard(PieceType pt) const {
            return bitboard(Color::WHITE, pt) | bitboard(Color::BLACK, pt);
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
            int n = 1;

            for (int i = static_cast<int>(m_history.size()) - 2; i >= 0; i -= 2) {
                if (m_history[i].hash == m_hash) {
                    n++;
                }

                if (n >= count) {
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

            constexpr State(CastlingRights castle, Square enPassant, Piece capturedPiece, int halfmoveClock, U64 hash)
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

        constexpr void _clearAllPieces() {
            m_pieces.clear();
            m_occupancy.zero();
            for (int i = 0; i < NUM_COLORS; ++i) {
                for (int j = 0; j < NUM_PIECE_TYPES; ++j) {
                    m_bitboards[i][j].zero();
                }
            }
        }

        constexpr void _recordState(Piece capturedPiece) {
            m_history.emplace_back(m_castlingRights, m_enPassantSq, capturedPiece, m_halfmoveClock, m_hash);
        }

        constexpr Piece _restoreState() {
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
        const Color     side           = sideToMove();
        const Piece     piece          = movedPiece(move);
        const PieceType pt             = pieceToPieceType(piece);
        const bool      is_capture     = isCapture(move) && move.type() != MoveType::CASTLING;
        const Piece     captured_piece = capturedPiece(move);

        _recordState(captured_piece);

        m_halfmoveClock++;
        m_ply++;

        if (m_enPassantSq.isValid()) {
            m_hash ^= Zobrist::enpassantKey(m_enPassantSq.file());
        }

        m_enPassantSq = Square();

        m_halfmoveClock *= (is_capture || pt == PieceType::PAWN);

        if (is_capture) {
            m_halfmoveClock = 0;

            removePiece(captured_piece, move.to());
        }

        if (is_capture && pieceToPieceType(captured_piece) == PieceType::ROOK && Square::isTheirBackRank(move.to(), side)) {
            const CastlingSide castleSide = CastlingRights::getCastlingSide(move.to(), kingSq(~side));
            m_castlingRights.setCastlingRights(~side, castleSide, 0);

            m_hash ^= Zobrist::castlingIndex(2 * static_cast<int>(~side) + static_cast<int>(castleSide));
        }

        if (pt == PieceType::KING && m_castlingRights.hasCastlingRights(side)) {
            m_hash ^= Zobrist::castlingKey(m_castlingRights.index());

            m_castlingRights.setCastlingRights(side, CastlingSide::KING_SIDE, 0);
            m_castlingRights.setCastlingRights(side, CastlingSide::QUEEN_SIDE, 0);

            m_hash ^= Zobrist::castlingKey(m_castlingRights.index());

        } else if (pt == PieceType::ROOK && Square::isOurBackRank(move.from(), side)) {
            const CastlingSide castleSide = CastlingRights::getCastlingSide(move.from(), kingSq(side));

            m_castlingRights.setCastlingRights(side, castleSide, 0);
            m_hash ^= Zobrist::castlingIndex(2 * static_cast<int>(side) + static_cast<int>(castleSide));
        }

        if (pt == PieceType::PAWN && Square::squareDistance(move.from(), move.to()) == 2) {
            Square   possible_ep = static_cast<Square>(int(move.to()) ^ 8);
            Bitboard ep_mask     = Attacks::pawnAttacks(possible_ep, side);

            if (ep_mask & bitboard(~side, PieceType::PAWN)) {
                m_enPassantSq = possible_ep;

                m_hash ^= Zobrist::enpassantKey(m_enPassantSq.file());
            }
        }

        if (move.type() == MoveType::CASTLING) {
            assert(at(move.from()) == makePiece(side, PieceType::KING));
            assert(at(move.to()) == makePiece(side, PieceType::ROOK));

            const CastlingSide castleSide = CastlingRights::getCastlingSide(move.to(), move.from());

            const Square rookTo = CastlingRights::rookTo(side, castleSide);
            const Square kingTo = CastlingRights::kingTo(side, castleSide);

            // captured piece = rook //
            // moved piece = king //

            removePiece(piece, move.from());
            removePiece(captured_piece, move.to());

            placePiece(piece, kingTo);
            placePiece(captured_piece, rookTo);

            m_hash ^= Zobrist::pieceKey(piece, move.from()) ^ Zobrist::pieceKey(piece, kingTo);
            m_hash ^= Zobrist::pieceKey(captured_piece, move.to()) ^ Zobrist::pieceKey(piece, rookTo);
        } else if (move.type() == MoveType::PROMOTION) {
            const Piece promoted = makePiece(side, move.promoted());
            removePiece(piece, move.from());
            placePiece(promoted, move.to());

            m_hash ^= Zobrist::pieceKey(piece, move.from()) ^ Zobrist::pieceKey(promoted, move.to());
        } else {
            removePiece(piece, move.from());
            placePiece(piece, move.to());

            m_hash ^= Zobrist::pieceKey(piece, move.from()) ^ Zobrist::pieceKey(piece, move.to());
        }

        if (move.type() == MoveType::ENPASSANT) {
            const Piece  piece = makePiece(~side, PieceType::PAWN);
            const Square sq    = Square(int(move.to()) ^ 8);
            removePiece(piece, sq);

            m_hash ^= Zobrist::pieceKey(piece, sq);
        }

        m_hash ^= Zobrist::sideKey();
        m_sideToMove = ~m_sideToMove;
    }

    inline void Board::unmakeMove(const Move& move) {
        Piece previouslyCaptured = _restoreState();

        m_sideToMove = ~m_sideToMove;
        m_ply--;

        if (move.type() == MoveType::CASTLING) {
            const CastlingSide castleSide = CastlingRights::getCastlingSide(move.to(), move.from());

            const Square rookFrom = CastlingRights::rookTo(m_sideToMove, castleSide);
            const Square kingTo   = CastlingRights::kingTo(m_sideToMove, castleSide);

            const Piece rook = at(rookFrom);
            const Piece king = at(kingTo);

            removePiece(rook, rookFrom);
            removePiece(king, kingTo);

            placePiece(rook, move.to());
            placePiece(king, move.from());

            return;
        } else if (move.type() == MoveType::PROMOTION) {
            removePiece(at(move.to()), move.to());
            placePiece(makePiece(m_sideToMove, PieceType::PAWN), move.from());

            if (previouslyCaptured != Piece::NONE) {
                placePiece(previouslyCaptured, move.to());
            }

            return;
        } else {
            const Piece movedPiece = at(move.to());
            removePiece(movedPiece, move.to());
            placePiece(movedPiece, move.from());
        }

        if (move.type() == MoveType::ENPASSANT) {
            placePiece(makePiece(~m_sideToMove, PieceType::PAWN), Square(int(move.to()) ^ 8));
        } else if (previouslyCaptured != Piece::NONE) {
            placePiece(previouslyCaptured, move.to());
        }
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