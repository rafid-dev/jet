#pragma once

#include <array>
#include <cstdint>
#include <cstring>

#include "../chess/square.hpp"
#include "../chess/types.hpp"

#include "constants.hpp"

namespace jet {

    namespace nnue {

        inline int kingSquareIndex(chess::Square kingSq, chess::Color view) {
            kingSq = chess::Square::relativeSquare(kingSq, view);

            if constexpr (constants::BUCKETS != 0) {
                return constants::KING_BUCKET[kingSq];
            } else {
                return 0;
            }
        }

        template <chess::Color view>
        inline int index(chess::PieceType pieceType, chess::Color pieceColor, chess::Square sq, chess::Square kingSq) {
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
