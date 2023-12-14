#pragma once

#include "types.hpp"

namespace jet {

    namespace constants {
        static inline constexpr types::Depth DEPTH_MAX = 63;
        static inline constexpr types::Depth PLY_MAX   = 63;

        static inline constexpr types::Value VALUE_INFINITY = 32001;
        static inline constexpr types::Value VALUE_NONE     = 32002;

        static inline constexpr types::Value IS_MATE  = 30000;
        static inline constexpr types::Value IS_MATED = -IS_MATE;

        static inline constexpr types::Value IS_MATE_IN_PLY_MAX  = (IS_MATE - PLY_MAX);
        static inline constexpr types::Value IS_MATED_IN_PLY_MAX = -IS_MATE_IN_PLY_MAX;

        static inline constexpr types::Depth SEARCH_STACK_SIZE = PLY_MAX + 10;

    } // namespace constants

} // namespace jet
