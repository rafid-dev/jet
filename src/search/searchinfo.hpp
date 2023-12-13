#pragma once

#include "types.hpp"

namespace jet {
    namespace search {
        class SearchInfo {
        public:
            inline constexpr types::Depth depth() const {
                return m_depth;
            }

            inline constexpr void setDepth(types::Depth d) {
                m_depth = d;
            }

        private:
            types::Depth m_depth;
        };
    } // namespace search
} // namespace jet