#pragma once

#include "../chess/moves.hpp"
#include "constants.hpp"
#include <cstdint>

namespace jet {

    namespace search {
        using PvTable = std::array<chess::Move, constants::PLY_MAX + 1>;
        struct SearchStack {
            using List    = std::array<SearchStack, constants::SEARCH_STACK_SIZE>;
            using pointer = SearchStack*;

            types::Depth ply         = 0;
            int          pv_length   = 0;
            types::Value static_eval = 0;
            chess::Move  move        = chess::Move::none();
            chess::Move  killers[2]  = {chess::Move::none(), chess::Move::none()};
            chess::Move  excluded    = chess::Move::none();
            PvTable      pv;

            void updateKiller(chess::Move move) {
                killers[1] = killers[0];
                killers[0] = move;
            }

            void updatePV(chess::Move bestmove, SearchStack* next) {
                pv[ply] = bestmove;

                for (int i = ply + 1; i < next->pv_length; ++i) {
                    pv[i] = next->pv[i];
                }

                pv_length = next->pv_length;
            };

            static pointer init(List& list) {
                for (auto& ss : list) {
                    std::fill(ss.pv.begin(), ss.pv.end(), chess::Move::none());
                }

                return list.data() + 7;
            }

            static void printPVs(pointer ss) {
                for (int i = 0; i < ss->pv_length; i++) {
                    std::cout << " " << ss->pv[i];
                }
            }
        };

    } // namespace search

} // namespace jet
