#pragma once

#include "movegen.hpp"

namespace chess {

    // Compatibility wrapper. All legality logic now lives in MoveGen.
    class MoveGenCountOnly {
    public:
        template <MoveGenType mt>
        static inline int legalmoves(const Board& board) {
            return MoveGen::countLegalMoves<mt>(board);
        }
    };

} // namespace chess