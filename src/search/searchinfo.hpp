#pragma once

#include "constants.hpp"
#include "types.hpp"

namespace jet {
    namespace search {
        class SearchInfo {
        public:
            inline constexpr types::Depth depth() const {
                return m_depth;
            }

            inline void setDepth(types::Depth d) {
                m_depth = d;
            }

            inline void setPrintInfo(bool b) {
                printInfo = b;
            }

            inline bool shouldPrintInfo() const {
                return printInfo;
            }

        private:
            types::Depth m_depth = constants::DEPTH_MAX;

            bool printInfo = true;
        };
    } // namespace search
} // namespace jet