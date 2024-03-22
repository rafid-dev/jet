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
                start_time = misc::tick();

                Time t   = (c == chess::Color::WHITE) ? wtime : btime;
                Time inc = (c == chess::Color::WHITE) ? winc : binc;

                t -= OVERHEAD;

                if (movestogo) {
                    Time x = t / static_cast<Time>(movestogo) + inc / 2;

                    stoptime_max = average_time = stoptime_opt = x;
                } else if (movetime) {
                    stoptime_max = average_time = stoptime_opt = (movetime - OVERHEAD);
                } else {
                    t /= 20;

                    Time x = average_time = t + inc / 2;
                    Time base = x;

                    stoptime_opt = base * 0.6;
                    stoptime_max = std::min(t, base * 2);
                }
            }

            bool shouldStop() const {
                return misc::tick() > (start_time + stoptime_max);
            }

            bool shouldStopEarly() const {
                return misc::tick() > (start_time + stoptime_opt);
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

            void setNodes(uint64_t n) {
                nodes = n;
            }

            bool checkNodes(uint64_t n) const {
                return nodes && n >= nodes;
            }

            bool nodeset() const {
                return nodes;
            }

            bool timeset = false;

        private:
            Time     wtime{};
            Time     btime{};
            Time     winc{};
            Time     binc{};
            Time     movetime{};
            int      movestogo = 0;
            uint64_t nodes     = 0;

            Time start_time{};
            Time stoptime_opt{};
            Time stoptime_max{};
            Time average_time{};
        };
    } // namespace search
} // namespace jet