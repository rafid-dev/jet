#pragma once

#include "../misc/utils.hpp"
#include "types.hpp"

namespace jet {
    namespace search {
        enum class TimeType : uint8_t {
            WTIME,
            BTIME,
            WINC,
            BINC,
            MOVETIME,
        };
        class TimeManager {
        public:
            using Time = types::Time;

            static constexpr inline Time OVERHEAD = 50;

            TimeManager() = default;

            void start(chess::Color c) {
                m_start = misc::tick();

                Time t = (c == chess::Color::WHITE) ? wtime : btime;

                if (movestogo) {
                    t -= OVERHEAD;

                    stoptime = t / static_cast<Time>(movestogo);
                    return;
                } else if (movetime) {
                    stoptime = movetime - OVERHEAD;
                    return;
                }

                t -= OVERHEAD;
                t /= 20;

                Time inc = (c == chess::Color::WHITE) ? winc : binc;

                stoptime = t + inc;
            }

            bool canStop() const {
                return misc::tick() > (m_start + stoptime);
            }

            template <TimeType tt>
            void setTime(Time t) {
                if constexpr (tt == TimeType::BTIME) {
                    btime = t;
                } else if constexpr (tt == TimeType::WTIME) {
                    wtime = t;
                } else if constexpr (tt == TimeType::WINC) {
                    winc = t;
                } else if constexpr (tt == TimeType::BINC) {
                    binc = t;
                } else if constexpr (tt == TimeType::MOVETIME) {
                    movetime = t;
                }

                timeset = true;
            }

            void setMTG(int x) {
                movestogo = x;
            }

            bool timeset = false;

        private:
            Time m_start{};
            Time wtime{};
            Time btime{};
            Time winc{};
            Time binc{};
            Time movetime{};
            int  movestogo = 0;

            Time stoptime{};
        };
    } // namespace search
} // namespace jet