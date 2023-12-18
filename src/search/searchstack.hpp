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

            types::Depth ply      = 0;
            int          pvLength = 0;
            PvTable      pv;

            void updatePV(chess::Move bestmove, SearchStack* next) {
                pv[ply] = bestmove;

                for (int i = ply + 1; i < next->pvLength; ++i) {
                    pv[i] = next->pv[i];
                }

                pvLength = next->pvLength;
            };

            static pointer init(List& list) {
                for (auto& ss : list) {
                    std::fill(ss.pv.begin(), ss.pv.end(), chess::Move::none());
                }

                return list.data() + 7;
            }

            static void printPVs(pointer ss) {
                for (int i = 0; i < ss->pvLength; i++) {
                    std::cout << " " << ss->pv[i];
                }
            }
        };

    } // namespace search

} // namespace jet
