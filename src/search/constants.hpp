#pragma once

#include "types.hpp"

namespace jet {

    namespace constants {
        static inline constexpr types::Depth MAX_DEPTH = 63;
        static inline constexpr types::Depth MAX_PLY   = 63;

        static inline constexpr types::Value VALUE_INFINITY = 32001;
        static inline constexpr types::Value VALUE_NONE     = 32002;

        static inline constexpr types::Value IS_MATE  = 30000;
        static inline constexpr types::Value IS_MATED = -IS_MATE;

        static inline constexpr types::Value IS_MATE_IN_MAX_PLY  = (IS_MATE - MAX_PLY);
        static inline constexpr types::Value IS_MATED_IN_MAX_PLY = -IS_MATE_IN_MAX_PLY;

    } // namespace constants

} // namespace jet
