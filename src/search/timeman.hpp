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

                Time t   = (c == chess::Color::WHITE) ? wtime : btime;
                Time inc = (c == chess::Color::WHITE) ? winc : binc;

                t -= OVERHEAD;

                if (movestogo) {
                    stoptime = t / static_cast<Time>(movestogo) + inc / 2;
                } else if (movetime) {
                    stoptime = movetime - OVERHEAD;
                } else {
                    t /= 20;

                    stoptime = t + inc / 2;
                }
            }

            bool shouldStop() const {
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
            Time     m_start{};
            Time     wtime{};
            Time     btime{};
            Time     winc{};
            Time     binc{};
            Time     movetime{};
            int      movestogo = 0;
            uint64_t nodes     = 0;

            Time stoptime{};
        };
    } // namespace search
} // namespace jet