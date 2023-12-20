#pragma once

#include "../chess/board.hpp"
#include "../chess/moves.hpp"

namespace jet {

    namespace search {

        class MoveOrdering {
        private:
        public:
            static void captures(const chess::Board& board, chess::Movelist& movelist) {
                for (auto& move : movelist) {
                    const auto attacker = board.at(move.from());
                    const auto target   = board.at(move.to());

                    if (target != chess::Piece::NONE) {
                        move.setScore(static_cast<int16_t>(target) * static_cast<int16_t>(10) - static_cast<int16_t>(attacker));
                    }
                }
            }
        };

    } // namespace search

} // namespace jet
