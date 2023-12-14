#pragma once

#include "../chess/moves.hpp"
#include "types.hpp"
#include <cstdint>

namespace jet {

    namespace search {
        struct SearchStack {
            int ply = 0;

            types::PvTable pv;

            SearchStack() {
                std::fill(pv.begin(), pv.end(), chess::Move::none());
            }

            void updatePV(chess::Move bestmove, SearchStack* next) {
                int i;

                pv[0] = bestmove;

                for (i = 0; next->pv[i].isValid(); ++i) {
                    pv[i + 1] = next->pv[i];
                }

                pv[i + 1] = chess::Move::none();
            };
        };

    } // namespace search

} // namespace jet
