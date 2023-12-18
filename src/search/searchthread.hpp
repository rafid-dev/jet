#pragma once

#include "../chess/board.hpp"
#include "constants.hpp"
#include "searchinfo.hpp"
#include "timeman.hpp"

namespace jet {
    namespace search {

        class SearchThread {
        public:
            uint64_t nodes = 0;

            SearchThread() = default;
            SearchThread(const chess::Board& b) : m_board{b} {
            }

            constexpr chess::Board& board() {
                return m_board;
            }

            constexpr void setFen(std::string_view fen) {
                m_board.setFen(fen);
            }

            constexpr void start() {
                stop_flag = false;
                nodes     = 0;
                timeman.start(m_board.sideToMove());
            }

            TimeManager& timeManager() {
                return timeman;
            }

            bool stop() const {
                return stop_flag && (timeman.timeset || timeman.nodeset());
            }

            void checkup() {
                if (timeman.nodeset()) {
                    stop_flag = timeman.checkNodes(nodes);
                    return;
                }
                if ((nodes & 2047) == 0) {
                    stop_flag = timeman.shouldStop();
                }
            }

        private:
            chess::Board m_board;
            TimeManager  timeman;
            bool         stop_flag = false;
        };
    } // namespace search
} // namespace jet
