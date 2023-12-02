#pragma once

#include <cstdint>
#include <string>

namespace chess {

    using U64      = uint64_t;
    using Bitboard = uint64_t;

    // clang-format off

    enum class Direction : int8_t {
        NORTH      = 8,
        WEST       = -1,
        SOUTH      = -8,
        EAST       = 1,
        NORTH_EAST = 9,
        NORTH_WEST = 7,
        SOUTH_WEST = -9,
        SOUTH_EAST = -7
    };

    enum class PieceType : uint8_t { PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING, NONE };

    enum class Piece : uint8_t {
        WHITEPAWN,
        WHITEKNIGHT,
        WHITEBISHOP,
        WHITEROOK,
        WHITEQUEEN,
        WHITEKING,
        BLACKPAWN,
        BLACKKNIGHT,
        BLACKBISHOP,
        BLACKROOK,
        BLACKQUEEN,
        BLACKKING,
        NONE
    };



    enum class Color : uint8_t { WHITE, BLACK, NO_COLOR };

    // clang-format on

    constexpr Color operator~(Color c) {
        return Color(static_cast<uint8_t>(c) ^ 1);
    }

    constexpr uint8_t NUM_SQUARES     = 64;
    constexpr uint8_t NUM_COLORS      = 2;
    constexpr uint8_t NUM_PIECES      = 12;
    constexpr uint8_t NUM_PIECE_TYPES = 6;
    constexpr int     MAX_MOVES       = 256;

    // clang-format off
    constexpr std::string_view squareToString[64] = {
        "a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1",
        "a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
        "a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
        "a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
        "a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
        "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
        "a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
        "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8",
    };
    // clang-format on 

    constexpr Piece charToPiece(char c){

        if (c == 'P') return Piece::WHITEPAWN;
        if (c == 'N') return Piece::WHITEKNIGHT;
        if (c == 'B') return Piece::WHITEBISHOP;
        if (c == 'R') return Piece::WHITEROOK;
        if (c == 'Q') return Piece::WHITEQUEEN;
        if (c == 'K') return Piece::WHITEKING;

        if (c == 'p') return Piece::BLACKPAWN;
        if (c == 'n') return Piece::BLACKKNIGHT;
        if (c == 'b') return Piece::BLACKBISHOP;
        if (c == 'r') return Piece::BLACKROOK;
        if (c == 'q') return Piece::BLACKQUEEN;
        if (c == 'k') return Piece::BLACKKING;

        return Piece::NONE;
    }

    constexpr char pieceToChar(Piece p){
        if (p == Piece::WHITEPAWN) return 'P';
        if (p == Piece::WHITEKNIGHT) return 'N';
        if (p == Piece::WHITEBISHOP) return 'B';
        if (p == Piece::WHITEROOK) return 'R';
        if (p == Piece::WHITEQUEEN) return 'Q';
        if (p == Piece::WHITEKING) return 'K';

        if (p == Piece::BLACKPAWN) return 'p';
        if (p == Piece::BLACKKNIGHT) return 'n';
        if (p == Piece::BLACKBISHOP) return 'b';
        if (p == Piece::BLACKROOK) return 'r';
        if (p == Piece::BLACKQUEEN) return 'q';
        if (p == Piece::BLACKKING) return 'k';

        return '.';
    }

    constexpr char pieceTypeToChar(PieceType pt){
        if (pt == PieceType::PAWN) return 'P';
        if (pt == PieceType::KNIGHT) return 'N';
        if (pt == PieceType::BISHOP) return 'B';
        if (pt == PieceType::ROOK) return 'R';
        if (pt == PieceType::QUEEN) return 'Q';
        if (pt == PieceType::KING) return 'K';

        return '.';
    }

    constexpr PieceType charToPieceType(char c){
        if (c == 'P' || c == 'p') return PieceType::PAWN;
        if (c == 'N' || c == 'n') return PieceType::KNIGHT;
        if (c == 'B' || c == 'b') return PieceType::BISHOP;
        if (c == 'R' || c == 'r') return PieceType::ROOK;
        if (c == 'Q' || c == 'q') return PieceType::QUEEN;
        if (c == 'K' || c == 'k') return PieceType::KING;

        return PieceType::NONE;
    }

    constexpr PieceType pieceToPieceType(Piece p) {
        return static_cast<PieceType>(static_cast<uint8_t>(p) % 6);
    }

    constexpr Color pieceToColor(Piece p) {
        return static_cast<Color>(static_cast<uint8_t>(p) / 6);
    }

    constexpr Piece colorPiece(Color c, PieceType pt) {
        return static_cast<Piece>(static_cast<uint8_t>(c) * 6 + static_cast<uint8_t>(pt));
    }

    static constexpr std::string_view START_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

} // namespace chess