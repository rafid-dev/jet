#pragma once

#include "../chess/board.hpp"
#include "../chess/moves.hpp"

namespace jet {

    namespace search {

        class History {
        private:
            std::array<std::array<int16_t, 12>, 64> m_table;

        public:
            static constexpr inline int16_t MAX_HISTORY = 2 << 13;

            auto& index(const chess::Board& board, const chess::Move& move) {
                const auto piece = board.at(move.from());
                return m_table[static_cast<int>(piece)][move.to()];
            }

            auto& index(const chess::Board& board, const chess::Move& move) const {
                const auto piece = board.at(move.from());
                return m_table[static_cast<int>(piece)][move.to()];
            }

            void clear() {
                std::memset(m_table.data(), 0, sizeof(m_table));
            }

            static auto bonus(int depth) {
                return 32 * depth;
            }

            void update(const chess::Board& board, const chess::Move& move, int bonus) {
                auto& historyScore = index(board, move);

                historyScore += bonus - historyScore * std::abs(bonus) / MAX_HISTORY;
            }
        };

    } // namespace search

} // namespace jet
