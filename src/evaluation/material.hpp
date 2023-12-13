#pragma once

#include "../chess/board.hpp"
#include "../chess/types.hpp"
#include "../search/types.hpp"

namespace jet {
    namespace evaluation {
        using chess::Bitboard;
        using chess::Board;
        using chess::Color;
        using chess::PieceType;

        constexpr std::array<types::Value, 6> material = {100, 300, 300, 500, 900, 0};

        template <PieceType pt, Color c>
        types::Value evaluateMaterial(Board& board) {
            Bitboard pieceBB = board.bitboard<c, pt>();
            return pieceBB.popcount() * material[static_cast<int>(pt)];
        }

        template <Color c>
        types::Value evaluateMaterial(Board& board) {
            types::Value sum = 0;

            if constexpr (c == Color::WHITE) {
                sum += evaluateMaterial<PieceType::PAWN, c>(board);
                sum += evaluateMaterial<PieceType::KNIGHT, c>(board);
                sum += evaluateMaterial<PieceType::BISHOP, c>(board);
                sum += evaluateMaterial<PieceType::ROOK, c>(board);
                sum += evaluateMaterial<PieceType::QUEEN, c>(board);
            } else {
                sum -= evaluateMaterial<PieceType::PAWN, c>(board);
                sum -= evaluateMaterial<PieceType::KNIGHT, c>(board);
                sum -= evaluateMaterial<PieceType::BISHOP, c>(board);
                sum -= evaluateMaterial<PieceType::ROOK, c>(board);
                sum -= evaluateMaterial<PieceType::QUEEN, c>(board);
            }

            return sum;
        }
    } // namespace evaluation
} // namespace jet