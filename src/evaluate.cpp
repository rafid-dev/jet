#include "evaluation/evaluate.hpp"
#include "evaluation/material.hpp"
#include "evaluation/psqt.hpp"
#include <array>

using chess::Bitboard;
using chess::Board;
using chess::Color;
using chess::PieceType;
using jet::search::SearchThread;
using jet::types::Value;

namespace jet {
    namespace evaluation {

        Value evaluate(Board& board) {
            Value sum = 0;

            if (board.sideToMove() == Color::WHITE) {
                sum += evaluateMaterial<Color::WHITE>(board);
                sum += evaluateMaterial<Color::BLACK>(board);

                // sum += evaluatePSQT<Color::WHITE>(board);
                // sum += evaluatePSQT<Color::BLACK>(board);
            } else {
                sum -= evaluateMaterial<Color::WHITE>(board);
                sum -= evaluateMaterial<Color::BLACK>(board);

                // sum -= evaluatePSQT<Color::WHITE>(board);
                // sum -= evaluatePSQT<Color::BLACK>(board);
            }

            return sum;
        }

        Value evaluate(search::SearchThread& st) {
            Board& board = st.board();

            return evaluate(board);
        }

    } // namespace evaluation
} // namespace jet
