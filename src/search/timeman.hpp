#pragma once

#include "../misc/utils.hpp"
#include "types.hpp"

namespace jet {
    namespace search {
        class TimeManager {
        public:
            using Time = types::Time;

            TimeManager() = default;

            constexpr void start() {
                m_start = misc::tick();
            }

        private:
            Time m_start{};
            Time wtime{};
            Time btime{};
            Time winc{};
            Time binc{};
        };
    } // namespace search
} // namespace jet