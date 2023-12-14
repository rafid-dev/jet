#pragma once

#include "../chess/moves.hpp"
#include "constants.hpp"
#include <cstdint>

namespace jet {

    namespace search {
        struct SearchStack {
            using List    = std::array<SearchStack, constants::SEARCH_STACK_SIZE>;
            using pointer = SearchStack*;

            types::Depth   ply = 0;
            types::PvTable pv;

            void updatePV(chess::Move bestmove, pointer next) {
                int i;

                pv[0] = bestmove;

                for (i = 0; next->pv[i].isValid(); ++i) {
                    pv[i + 1] = next->pv[i];
                }

                pv[i + 1] = chess::Move::none();
            };

            static pointer init(List& list) {
                for (auto& ss : list) {
                    std::fill(ss.pv.begin(), ss.pv.end(), chess::Move::none());
                }

                return list.data() + 7;
            }

            static void printPVs(pointer ss) {
                for (int i = 0; ss->pv[i].isValid(); i++) {
                    std::cout << " " << ss->pv[i];
                }
            }
        };

    } // namespace search

} // namespace jet
