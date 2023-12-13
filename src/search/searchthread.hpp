#pragma once

#include "../chess/board.hpp"

namespace jet {
    namespace search {
        class SearchThread {
        public:
            uint64_t nodes    = 0;
            Move     bestmove = Move::none();

            SearchThread() = default;
            SearchThread(const chess::Board& b) : m_board{b} {
            }

            constexpr chess::Board& board() {
                return m_board;
            }

            constexpr void setFen(std::string_view fen) {
                m_board.setFen(fen);
            }

        private:
            chess::Board m_board;
        };
    } // namespace search
} // namespace jet
