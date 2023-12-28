#pragma once

#include <array>
#include <cstdint>

#include "../chess/square.hpp"
#include "../chess/types.hpp"

namespace jet {

    namespace nnue {

        namespace constants {

            constexpr int BUCKETS     = 4;
            constexpr int INPUT_SIZE  = 64 * 6 * 2 * BUCKETS;
            constexpr int HIDDEN_SIZE = 768;
            constexpr int OUTPUT_SIZE = 1;

            constexpr int INPUT_LAYER_SIZE  = INPUT_SIZE * HIDDEN_SIZE;
            constexpr int HIDDEN_LAYER_SIZE = HIDDEN_SIZE * 2;
            constexpr int OUTPUT_LAYER_SIZE = OUTPUT_SIZE;

            // clang-format off
            constexpr std::array<int, 64> KING_BUCKET {
                0, 0, 1, 1, 1, 1, 0, 0,
                0, 0, 1, 1, 1, 1, 0, 0,
                0, 0, 1, 1, 1, 1, 0, 0,
                0, 0, 1, 1, 1, 1, 0, 0,
                2, 2, 3, 3, 3, 3, 2, 2,
                2, 2, 3, 3, 3, 3, 2, 2,
                2, 2, 3, 3, 3, 3, 2, 2,
                2, 2, 3, 3, 3, 3, 2, 2,
            };
            // clang-format on

        } // namespace constants

        constexpr inline int kingSquareIndex(chess::Square kingSq, chess::Color view) {
            kingSq = chess::Square::relativeSquare(kingSq, view);

            if constexpr (constants::BUCKETS != 0) {
                return constants::KING_BUCKET[kingSq];
            } else {
                return 0;
            }
        }

        template <chess::Color view>
        constexpr inline int index(chess::PieceType pieceType, chess::Color pieceColor, chess::Square sq,
                                   chess::Square kingSq) {
            const int kingIndex = kingSquareIndex(kingSq, view);

            sq = chess::Square::relativeSquare(sq, view);
            sq = chess::Square(sq ^ (7 * !!(kingSq & 0x4)));

            // clang-format off
            return static_cast<int>(sq)
                + static_cast<int>(pieceType) * 64
                + !(static_cast<int>(pieceColor) ^ static_cast<int>(view)) * 64 * 6 + kingIndex * 64 * 6 * 2;
            // clang-format on
        }
    } // namespace nnue

} // namespace jet
