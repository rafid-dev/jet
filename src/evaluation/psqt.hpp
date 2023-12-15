#pragma once

#include "../chess/board.hpp"
#include "../chess/types.hpp"
#include "../search/types.hpp"

namespace jet {

    namespace evaluation {
        // clang-format off
        constexpr types::Value PSQTs[6][64] = {
            { 0,  0,  0,  0,  0,  0,  0,  0,
            50, 50, 50, 50, 50, 50, 50, 50,
            10, 10, 20, 30, 30, 20, 10, 10,
            5,  5, 10, 25, 25, 10,  5,  5,
            0,  0,  0, 20, 20,  0,  0,  0,
            5, -5,-10,  0,  0,-10, -5,  5,
            5, 10, 10,-20,-20, 10, 10,  5,
            0,  0,  0,  0,  0,  0,  0,  0},
            {-50,-40,-30,-30,-30,-30,-40,-50,
            -40,-20,  0,  0,  0,  0,-20,-40,
            -30,  0, 10, 15, 15, 10,  0,-30,
            -30,  5, 15, 20, 20, 15,  5,-30,
            -30,  0, 15, 20, 20, 15,  0,-30,
            -30,  5, 10, 15, 15, 10,  5,-30,
            -40,-20,  0,  5,  5,  0,-20,-40,
            -50,-40,-30,-30,-30,-30,-40,-50},
            {-20,-10,-10,-10,-10,-10,-10,-20,
            -10,  0,  0,  0,  0,  0,  0,-10,
            -10,  0,  5, 10, 10,  5,  0,-10,
            -10,  5,  5, 10, 10,  5,  5,-10,
            -10,  0, 10, 10, 10, 10,  0,-10,
            -10, 10, 10, 10, 10, 10, 10,-10,
            -10,  5,  0,  0,  0,  0,  5,-10,
            -20,-10,-10,-10,-10,-10,-10,-20,},
        {0,  0,  0,  0,  0,  0,  0,  0,
            5, 10, 10, 10, 10, 10, 10,  5,
            -5,  0,  0,  0,  0,  0,  0, -5,
            -5,  0,  0,  0,  0,  0,  0, -5,
            -5,  0,  0,  0,  0,  0,  0, -5,
            -5,  0,  0,  0,  0,  0,  0, -5,
            -5,  0,  0,  0,  0,  0,  0, -5,
            0,  0,  0,  5,  5,  0,  0,  0},
            {-20,-10,-10, -5, -5,-10,-10,-20,
            -10,  0,  0,  0,  0,  0,  0,-10,
            -10,  0,  5,  5,  5,  5,  0,-10,
            -5,  0,  5,  5,  5,  5,  0, -5,
            0,  0,  5,  5,  5,  5,  0, -5,
            -10,  5,  5,  5,  5,  5,  0,-10,
            -10,  0,  5,  0,  0,  0,  0,-10,
            -20,-10,-10, -5, -5,-10,-10,-20},
            {-30,-40,-40,-50,-50,-40,-40,-30,
            -30,-40,-40,-50,-50,-40,-40,-30,
            -30,-40,-40,-50,-50,-40,-40,-30,
            -30,-40,-40,-50,-50,-40,-40,-30,
            -20,-30,-30,-40,-40,-30,-30,-20,
            -10,-20,-20,-20,-20,-20,-20,-10,
            20, 20,  0,  0,  0,  0, 20, 20,
            20, 30, 10,  0,  0, 10, 30, 20},
        };

        // clang-format on

        template <chess::PieceType pt, chess::Color c>
        types::Value evaluatePSQT(const chess::Board& board) {
            chess::Bitboard bb = board.bitboard<c, pt>();

            types::Value sum = 0;

            BitboardIterator(bb) {
                chess::Square sq = chess::Square::relativeSquare<~c>(bb.poplsb());

                sum += PSQTs[static_cast<int>(pt)][sq];
            }

            return sum;
        }

        template <chess::Color c>
        types::Value evaluatePSQT(const chess::Board& board) {
            types::Value sum = 0;

            constexpr int sign = (c == chess::Color::WHITE) ? 1 : -1;

            sum += evaluatePSQT<chess::PieceType::PAWN, c>(board);
            sum += evaluatePSQT<chess::PieceType::KNIGHT, c>(board);
            sum += evaluatePSQT<chess::PieceType::BISHOP, c>(board);
            sum += evaluatePSQT<chess::PieceType::ROOK, c>(board);
            sum += evaluatePSQT<chess::PieceType::QUEEN, c>(board);
            sum += evaluatePSQT<chess::PieceType::KING, c>(board);

            return sum * sign;
        }
    } // namespace evaluation

} // namespace jet
