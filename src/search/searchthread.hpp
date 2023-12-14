#pragma once

#include "../chess/board.hpp"
#include "constants.hpp"

namespace jet {
    namespace search {

        class SearchThread {
        public:
            uint64_t       nodes = 0;
            types::PvTable pv_table;

            SearchThread() = default;
            SearchThread(const chess::Board& b) : m_board{b} {
            }

            constexpr chess::Board& board() {
                return m_board;
            }

            constexpr void setFen(std::string_view fen) {
                m_board.setFen(fen);
            }

            constexpr void reset() {
                std::fill(pv_table.begin(), pv_table.end(), chess::Move::none());
            }

        private:
            chess::Board m_board;
        };
    } // namespace search
} // namespace jet
